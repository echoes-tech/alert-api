/* 
 * Itooki mail resolve resource
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
#include "mail/mailResolve.h"

using namespace std;

MailResolve::MailResolve(Echoes::Dbo::Session& session): PublicMailResource::PublicMailResource(session)
{
    
}

MailResolve::~MailResolve()
{
}

EReturnCode MailResolve::PostResolve(map<string, string> parameters, const vector<string> &pathElements, const string &sRequest, string &responseMsg, const Wt::Http::Request &request)
{
    EReturnCode res = EReturnCode::OK;
    bool answered = false;
    
    Wt::Http::Client *client = new Wt::Http::Client();
    client->done().connect(boost::bind(&MailResolve::handleHttpResponse, this, _1, _2, &res, &responseMsg, &answered));

    string url = "http";
    url += "://127.0.0.1:" + boost::lexical_cast<std::string>(conf.getServerPort()) + "/mail/resolve";
    string json = "{";
    json += "\"id\" : " + parameters["id"] + ",";
    json += "\"token\" : \"" + parameters["token"] + "\"";
    json += "}";
            
    Wt::Http::Message httpMessage;
    httpMessage.addBodyText(json);
    httpMessage.addHeader("content-type","application/json");
            
    client->post(url, httpMessage);
    if(!answered)
    {
        res = EReturnCode::OK;
        responseMsg = "<br/> La demande a bien ete envoyee et sera traitee dans les plus brefs delais.<br/><br/> Consultez votre interface, pour voir son avancement.";
    }
    
    return res;
}

void MailResolve::handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response, EReturnCode *res, string *responseMsg, bool * answered)
{
    *res = EReturnCode::OK;
    *responseMsg = response.body();
    *answered = true;
}

EReturnCode MailResolve::processGetRequest(const Wt::Http::Request &request, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    string nextElement = "";
    unsigned short indexPathElement = 1;
    vector<string> pathElements;
    map<string, string> parameters;
    
    parameters["id"] = "";
    parameters["token"] = "";
    
    const string sRequest = processRequestParameters(request, pathElements, parameters);

    nextElement = getNextElementFromPath(indexPathElement, pathElements);

    if (nextElement.empty())
    {
        res = PostResolve(parameters, pathElements, sRequest, responseMsg, request);
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        responseMsg = "chemin incorrect";
    }

    return res;
}

EReturnCode MailResolve::doMailResolve(map<string, string> parameters, const vector<string> &pathElements, const string &sRequest, string &responseMsg, const Wt::Http::Request &request)
{
   EReturnCode res = EReturnCode::OK;
    
    const Wt::WDateTime now = Wt::WDateTime::currentDateTime();
    
    Wt::Dbo::Transaction transaction(m_session, true);
    
    bool informationReceived = false;
    Wt::WString  token = "missing";
    long long  id = -1;
    
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
            newStateMsg->text = "ask for resolve by mail";

            Wt::Dbo::ptr<Echoes::Dbo::MessageTrackingEvent> newMsgTrEv = m_session.add<Echoes::Dbo::MessageTrackingEvent>(newStateMsg);
                
            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent>> atePtr = m_session.find<Echoes::Dbo::AlertTrackingEvent>()
                    .where(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP TRIGRAM_ALERT SEP TRIGRAM_ALERT ID) " = ?").bind(msgPtr->alert.id())
                    .where(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP "DELETE") " IS NULL")
                    .orderBy(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP "DATE") " DESC");
            
                if(atePtr.size() > 0)
                {
                    Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent>>::iterator itAtePtr = atePtr.begin();
                    if((itAtePtr->get()->statut.id() == Echoes::Dbo::EAlertStatus::SUPPORTED
                                && itAtePtr->get()->user.id() == msgPtr->user.id()))
                    {
                        Echoes::Dbo::AlertTrackingEvent *newStateAle = new Echoes::Dbo::AlertTrackingEvent();

                        newStateAle->date = now;
                        newStateAle->alert = itAtePtr->get()->alert;

                        newStateAle->statut = m_session.find<Echoes::Dbo::AlertStatus>()
                            .where(QUOTE(TRIGRAM_ALERT_STATUS ID) " = ?").bind(Echoes::Dbo::EAlertStatus::BACKTONORMAL)
                            .where(QUOTE(TRIGRAM_ALERT_STATUS SEP "DELETE") " IS NULL");

                        newStateAle->user = m_session.find<Echoes::Dbo::User>()
                        .where(QUOTE(TRIGRAM_USER ID) " = ?").bind(msgPtr->user.id())
                        .where(QUOTE(TRIGRAM_USER SEP "DELETE") " IS NULL");

                        Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent> newAleTrEv = m_session.add<Echoes::Dbo::AlertTrackingEvent>(newStateAle);
                        
                        res = EReturnCode::CREATED;
                        responseMsg = "Annonce de resolution reussie!";
                    }
                    else
                    {
                        res = EReturnCode::BAD_REQUEST;
                        Wt::log("error") << "[Mail Resolve Resource] user is not supporter";
                        responseMsg = "Resolve impossible. . ";
                        if(itAtePtr->get()->statut.id() == Echoes::Dbo::EAlertStatus::SUPPORTED)
                        {
                            responseMsg += "Vous n'etes pas le responsable.";
                        }
                        else if(itAtePtr->get()->statut.id() == Echoes::Dbo::EAlertStatus::BACKTONORMAL)
                        {
                            responseMsg += "L'alerte a deja ete resolue.";
                        }
                        else if(itAtePtr->get()->statut.id() == Echoes::Dbo::EAlertStatus::FORWARDING)
                        {
                            responseMsg += "L'alerte est forwardee et vous n'etes pas la personne forwardee.";
                        }
                    }
                }
                else
                {
                    res = EReturnCode::BAD_REQUEST;
                    Wt::log("error") << "[Mail Resolve Resource] Alert is not pending";
                    responseMsg = "Une erreur est survenue dans la demande.  L'alerte existe mais elle n'a pas de statut";
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

EReturnCode MailResolve::processPostRequest(const Wt::Http::Request &request, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    string nextElement = "";
    unsigned short indexPathElement = 1;
    vector<string> pathElements;
    map<string, string> parameters;
    
    parameters["id"] = "";
    parameters["token"] = "";
    
    const string sRequest = processRequestParameters(request, pathElements, parameters);

    nextElement = getNextElementFromPath(indexPathElement, pathElements);

    if (nextElement.empty())
    {
        res = doMailResolve(parameters, pathElements, sRequest, responseMsg, request);
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        responseMsg = "chemin incorrect";
    }

    return res;
}
