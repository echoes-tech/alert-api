/* 
 * Itooki mail forward resource
 * @author ECHOES Technologies (CDE)
 * @date 15/09/2015
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2015 BY ECHOES TECHNOLGIES SAS
 * 
 */
#include "mail/mailForward.h"

using namespace std;

MailForward::MailForward(Echoes::Dbo::Session& session): PublicMailResource::PublicMailResource(session)
{
    
}

MailForward::~MailForward()
{
}

EReturnCode MailForward::PostForward(map<string, string> parameters, const vector<string> &pathElements, const string &sRequest, string &responseMsg, const Wt::Http::Request &request)
{
    EReturnCode res = EReturnCode::OK;
    bool answered = false;
    
    Wt::Http::Client *client = new Wt::Http::Client();
    client->done().connect(boost::bind(&MailForward::handleHttpResponse, this, _1, _2, &res, &responseMsg, &answered));

    string url = "http";
    url += "://127.0.0.1:" + boost::lexical_cast<std::string>(conf.getServerPort()) + "/mail/forward";
    string json = "{";
    json += "\"id\" : " + parameters["id"] + ",";
    json += "\"token\" : \"" + parameters["token"] + "\",";
    json += "\"idForward\" : " + parameters["idForward"];
    json += "}";
            
    Wt::Http::Message httpMessage;
    httpMessage.addBodyText(json);
    httpMessage.addHeader("content-type","application/json");
            
    client->post(url, httpMessage);
    if(!answered)
    {
        res = EReturnCode::OK;
        responseMsg = "<br/>La demande a bien été envoyée et sera traitée dans les plus brefs délais.<br/><br/> Consultez votre interface, pour voir son avancement.";
    }
    
    return res;
}

void MailForward::handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response, EReturnCode *res, string *responseMsg, bool * answered)
{
    *res = EReturnCode::OK;
    *responseMsg = response.body();
    *answered = true;
}

EReturnCode MailForward::processGetRequest(const Wt::Http::Request &request, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    string nextElement = "";
    unsigned short indexPathElement = 1;
    vector<string> pathElements;
    map<string, string> parameters;
    
    parameters["id"] = "";
    parameters["token"] = "";
    parameters["idForward"] = "";
    
    const string sRequest = processRequestParameters(request, pathElements, parameters);

    nextElement = getNextElementFromPath(indexPathElement, pathElements);

    if (nextElement.empty())
    {
        res = PostForward(parameters, pathElements, sRequest, responseMsg, request);
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        responseMsg = "chemin incorrect";
    }

    return res;
}

EReturnCode MailForward::doMailForward(map<string, string> parameters, const vector<string> &pathElements, const string &sRequest, string &responseMsg, const Wt::Http::Request &request)
{
   EReturnCode res = EReturnCode::OK;
    
    const Wt::WDateTime now = Wt::WDateTime::currentDateTime();
    
    Wt::Dbo::Transaction transaction(m_session, true);
    
    bool informationReceived = false;
    Wt::WString  token = "missing";
    long long  id = -1;
    long long idForward = -1;
    
    if(request.contentType().compare("application/json") == 0)
    {
        if (!sRequest.empty())
        {
            try
            {
                Wt::Json::Object result;
                Wt::Json::parse(sRequest, result);

                if (result.contains("token"))
                {
                    token = result.get("token");
                }
                if (result.contains("id"))
                {
                    id = result.get("id");
                }
                if (result.contains("idForward"))
                {
                    idForward = result.get("idForward");
                }
                informationReceived = true;
            }
            catch (Wt::Json::ParseError const& e)
            {
                res = EReturnCode::BAD_REQUEST;
                responseMsg = httpCodeToJSON(res, e);
            }
            catch (Wt::Json::TypeException const& e)
            {
                res = EReturnCode::BAD_REQUEST;
                responseMsg = httpCodeToJSON(res, e);
            }
        }
        else
        {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Send Resource] sRequest is empty";
                responseMsg = httpCodeToJSON(res, err);
        }
    }
    else if(request.contentType().compare("application/x-www-form-urlencoded") == 0)
    {
        token = parameters["token"];
        id = boost::lexical_cast<long long>(parameters["id"]);
        idForward = boost::lexical_cast<long long>(parameters["idForward"]);
        informationReceived = true;
    }
    else
    {
        Wt::log("error") << "[Mail Assign Resource] bad content type ";
        res = EReturnCode::BAD_REQUEST;
        responseMsg = "Une erreur est survenue dans la demande. Contactez le support applicatif. Erreur : request.contentType";   
    }
    
    if(informationReceived)
    {
        Wt::Dbo::ptr<Echoes::Dbo::Message> msgPtr = m_session.find<Echoes::Dbo::Message>()
                        .where(QUOTE(TRIGRAM_MESSAGE ID) " = ?").bind(id);
        if(msgPtr && msgPtr->refAck == token)
        {
            Echoes::Dbo::MessageTrackingEvent *newStateMsg = new Echoes::Dbo::MessageTrackingEvent();

            newStateMsg->date = now;
            newStateMsg->message = msgPtr;
            Wt::Dbo::ptr<Echoes::Dbo::MessageStatus> mstPtr = m_session.find<Echoes::Dbo::MessageStatus>()
                            .where(QUOTE(TRIGRAM_MESSAGE_STATUS ID) " = ?").bind(Echoes::Dbo::EMessageStatus::ANSWERED)
                            .where(QUOTE(TRIGRAM_MESSAGE_STATUS SEP "DELETE") " IS NULL");
            newStateMsg->statut = mstPtr;
            newStateMsg->text = "ask for forward by mail";

            Wt::Dbo::ptr<Echoes::Dbo::MessageTrackingEvent> newMsgTrEv = m_session.add<Echoes::Dbo::MessageTrackingEvent>(newStateMsg);
                
            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent>> atePtr = m_session.find<Echoes::Dbo::AlertTrackingEvent>()
                    .where(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP TRIGRAM_ALERT SEP TRIGRAM_ALERT ID) " = ?").bind(msgPtr->alert.id())
                    .where(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP "DELETE") " IS NULL")
                    .orderBy(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP "DATE") " DESC");
            
                if(atePtr.size() > 0)
                {
                    Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent>>::iterator itAtePtr = atePtr.begin();
                    if(itAtePtr->get()->statut.id() == Echoes::Dbo::EAlertStatus::PENDING
                            || (itAtePtr->get()->statut.id() == Echoes::Dbo::EAlertStatus::SUPPORTED
                                && itAtePtr->get()->user.id() == msgPtr->user.id()))
                    {
                        Echoes::Dbo::AlertTrackingEvent *newStateAle = new Echoes::Dbo::AlertTrackingEvent();

                        newStateAle->date = now;
                        newStateAle->alert = itAtePtr->get()->alert;

                        newStateAle->statut = m_session.find<Echoes::Dbo::AlertStatus>()
                            .where(QUOTE(TRIGRAM_ALERT_STATUS ID) " = ?").bind(Echoes::Dbo::EAlertStatus::FORWARDING)
                            .where(QUOTE(TRIGRAM_ALERT_STATUS SEP "DELETE") " IS NULL");

                        newStateAle->user = m_session.find<Echoes::Dbo::User>()
                        .where(QUOTE(TRIGRAM_USER ID) " = ?").bind(idForward)
                        .where(QUOTE(TRIGRAM_USER SEP "DELETE") " IS NULL");

                        Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent> newAleTrEv = m_session.add<Echoes::Dbo::AlertTrackingEvent>(newStateAle);
                        
                        res = EReturnCode::CREATED;
                        responseMsg = "Forward a " + newStateAle->user->firstName.toUTF8() + " " + newStateAle->user->lastName.toUTF8() + " reussie";
                    }
                    else
                    {
                        res = EReturnCode::BAD_REQUEST;
                        Wt::log("error") << "[Mail Forward Resource] Alert is not pending or user is not supporter";
                        responseMsg = "Forward impossible. ";
                        if(itAtePtr->get()->statut.id() == Echoes::Dbo::EAlertStatus::SUPPORTED)
                        {
                            responseMsg += "L'alerte a déjà été pourvue et vous n'êtes pas la personne responable.";
                        }
                        else if(itAtePtr->get()->statut.id() == Echoes::Dbo::EAlertStatus::BACKTONORMAL)
                        {
                            responseMsg += "L'alerte a déjà été résolue.";
                        }
                        else if(itAtePtr->get()->statut.id() == Echoes::Dbo::EAlertStatus::FORWARDING)
                        {
                            responseMsg += "L'alerte a déjà été forwardée et vous n'êtes pas la personne forwardée.";
                        }
                    }
                }
                else
                {
                    res = EReturnCode::BAD_REQUEST;
                    Wt::log("error") << "[Mail Forward Resource] Alert is not pending";
                    responseMsg = "Une erreur est survenue dans la demande. L'alerte existe mais elle n'a pas de statut";
                }
        }
        else
        {
            Wt::log("error") << "[Mail Assign Resource] No Message " << id << " " << token;
            res = EReturnCode::BAD_REQUEST;
            responseMsg = "Une erreur est survenue dans la demande. Le message n'existe pas";  
        }
    }
    else
    {
        Wt::log("error") << "[Mail Assign Resource] missing parameters";
        res = EReturnCode::BAD_REQUEST;
        responseMsg = "Une erreur est survenue dans la demande. Contactez le support applicatif. . Erreur : informations non reçues";
    }
    
    transaction.commit();
    
    return (res);
}

EReturnCode MailForward::processPostRequest(const Wt::Http::Request &request, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    string nextElement = "";
    unsigned short indexPathElement = 1;
    vector<string> pathElements;
    map<string, string> parameters;
    
    parameters["id"] = "";
    parameters["token"] = "";
    parameters["idForward"] = "";
    
    const string sRequest = processRequestParameters(request, pathElements, parameters);

    nextElement = getNextElementFromPath(indexPathElement, pathElements);

    if (nextElement.empty())
    {
        res = doMailForward(parameters, pathElements, sRequest, responseMsg, request);
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        responseMsg = "chemin incorrect";
    }

    return res;
}
