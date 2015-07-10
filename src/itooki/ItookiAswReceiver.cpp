/* 
 * Itooki SMS Answer Receiver
 * @author ECHOES Technologies (TSA)
 * @date 08/08/2012
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2012-2013 BY ECHOES TECHNOLGIES SAS
 * 
 */
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <Wt/Dbo/SqlConnection>
#include <Wt/Dbo/Session>

#include "itooki/ItookiAswReceiver.h"

using namespace std;

ItookiAswReceiver::ItookiAswReceiver(Echoes::Dbo::Session& session): Wt::WResource(),
m_session(session)
{
    
}

ItookiAswReceiver::~ItookiAswReceiver()
{
}

string ItookiAswReceiver::getNextElementFromPath(unsigned short &indexPathElement, vector<string> &pathElements)
{
    string res = "";

    if (pathElements.size() > indexPathElement)
    {
        res = pathElements[indexPathElement++];
    }

    return res;
}

string ItookiAswReceiver::request2string(const Wt::Http::Request &request)
{
    char c;
    string s;

    // Getting the input stream for the request char by char
    c = request.in().get();
    while (request.in())
    {
        s.append(1, c);
        c = request.in().get();
    }

    // WHY: Dans l'appli mobile lorsqu'on fait un post les premiers caractères de la requête sont remplacés par des caractères spéciaux. 
    // N'ayant pas su résoudre ce probléme, l'appli mobile envoie ses requêtes avec des caractères en trop au début du JSON. Il faut donc les supprimer.
    // On supprime donc tous les caractére avant "[" ou "{" suivant les cas pour éviter les problèmes de parse.
    const size_t pos1 = s.find("{", 0);
    const size_t pos2 = s.find("[", 0);
    if (pos1 != 0 || pos2 != 0)
    {
        if (pos2 != string::npos && pos2 < pos1)
        {
            s = s.erase(0, pos2);
        }
        else
        {
            s = s.erase(0, pos1);
        }
    }

    return s;
}

string ItookiAswReceiver::processRequestParameters(const Wt::Http::Request &request, vector<string> &pathElements, map<string, long long> &parameters)
{
    const string path = request.pathInfo();
    boost::split(pathElements, path, boost::is_any_of("/"), boost::token_compress_on);

    for (map<std::string, long long>::iterator it = parameters.begin(); it != parameters.end(); ++it)
    {
        if (request.getParameter(it->first) != 0)
        {
            try
            {
                it->second = boost::lexical_cast<unsigned long long>(*request.getParameter(it->first));
            }
            catch (boost::bad_lexical_cast const& e)
            {
                Wt::log("warning") << "[PUBLIC API] Bad URL parameter: " << e.what();
            }
        }
    }
    
    return request2string(request);
}

EReturnCode ItookiAswReceiver::postAsw(map<string, long long> parameters, const vector<string> &pathElements, const string &sRequest, string &responseMsg)
{
    EReturnCode res = EReturnCode::OK;
    
    const Wt::WDateTime now = Wt::WDateTime::currentDateTime();
    
    Wt::Dbo::Transaction transaction(m_session);
    
    Wt::WString  refenvoi = "missing";
    Wt::WString  message = "missing";
    
    if (!sRequest.empty())
        {
            try
            {
                Wt::Json::Object result;
                Wt::Json::parse(sRequest, result);

                if (result.contains("refenvoi"))
                {
                    refenvoi = result.get("refenvoi");
                }
                if (result.contains("message"))
                {
                    message = result.get("message");
                }

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
            
            Wt::Dbo::ptr<Echoes::Dbo::Message> msgPtr = m_session.find<Echoes::Dbo::Message>()
                        .where(QUOTE(TRIGRAM_MESSAGE SEP "REF" SEP "ACK") " = ?").bind(refenvoi)
                        .where(QUOTE(TRIGRAM_MESSAGE SEP "DELETE") " IS NULL");
                
            Echoes::Dbo::MessageTrackingEvent *newStateMsg = new Echoes::Dbo::MessageTrackingEvent();

            newStateMsg->date = now;
            newStateMsg->message = msgPtr;
            Wt::Dbo::ptr<Echoes::Dbo::MessageStatus> mstPtr = m_session.find<Echoes::Dbo::MessageStatus>()
                            .where(QUOTE(TRIGRAM_MESSAGE_STATUS ID) " = ?").bind(Echoes::Dbo::EMessageStatus::ANSWERED)
                            .where(QUOTE(TRIGRAM_MESSAGE_STATUS SEP "DELETE") " IS NULL");
            newStateMsg->statut = mstPtr;
            newStateMsg->text = message;

            Wt::Dbo::ptr<Echoes::Dbo::MessageTrackingEvent> newMsgTrEv = m_session.add<Echoes::Dbo::MessageTrackingEvent>(newStateMsg);

            operationOnAsw(newMsgTrEv);
     }
     else
        {
            res = EReturnCode::BAD_REQUEST;
            const string err = "[Send Resource] sRequest is not empty";
            responseMsg = httpCodeToJSON(res, err);
        }
    return (res);
}

void ItookiAswReceiver::operationOnAsw(Wt::Dbo::ptr<Echoes::Dbo::MessageTrackingEvent> msgTrEv)
{
    Wt::Dbo::ptr<Echoes::Dbo::Alert> ptrAle = msgTrEv->message->alert;
    
    const Wt::WDateTime now = Wt::WDateTime::currentDateTime();
    
    Wt::Dbo::Transaction transaction(m_session);
    
    Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent>> query =
            m_session.find<Echoes::Dbo::AlertTrackingEvent>().where("\"id\" = ?").bind(ptrAle->id).orderBy("date");
    Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent>> AlertEventList = query.resultList();
    
    Wt::Dbo::ptr<Echoes::Dbo::MessageTrackingEvent> msgEvCreaPtr = m_session.find<Echoes::Dbo::MessageTrackingEvent>()
            .where(QUOTE(TRIGRAM_MESSAGE_TRACKING_EVENT ID) " = ?").bind(Echoes::Dbo::EMessageStatus::CREATED)
            .where(QUOTE(TRIGRAM_MESSAGE_TRACKING_EVENT SEP TRIGRAM_MESSAGE SEP TRIGRAM_MESSAGE SEP ID) " = ?").bind(msgTrEv->message->id)
            .where(QUOTE(TRIGRAM_MESSAGE_TRACKING_EVENT SEP "DELETE") " IS NULL");
    
    if(AlertEventList.size() != 0 && msgEvCreaPtr)
    {
        // conditions pour la prise en charge:
        // - alert pending
        // - date cration du pending < date etat created du message
        // - message reçu = 1
        bool date_crea = (AlertEventList.begin()->get()->date < msgEvCreaPtr->date);
        bool pending = (AlertEventList.begin()->get()->statut->id == Echoes::Dbo::EAlertStatus::PENDING);
        bool supported = (AlertEventList.begin()->get()->statut->id == Echoes::Dbo::EAlertStatus::SUPPORTED);
        if(date_crea && pending && msgTrEv->text == "1")
        {
            Echoes::Dbo::AlertTrackingEvent *newStateAle = new Echoes::Dbo::AlertTrackingEvent();

            newStateAle->date = now;
            newStateAle->alert = ptrAle;
            Wt::Dbo::ptr<Echoes::Dbo::AlertStatus> alsPtr = m_session.find<Echoes::Dbo::AlertStatus>()
                .where(QUOTE(TRIGRAM_ALERT_STATUS ID) " = ?").bind(Echoes::Dbo::EAlertStatus::SUPPORTED)
                .where(QUOTE(TRIGRAM_ALERT_STATUS SEP "DELETE") " IS NULL");
            newStateAle->statut = alsPtr;

            Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent> newAleTrEv = m_session.add<Echoes::Dbo::AlertTrackingEvent>(newStateAle);

        }
        //conditons pour la résolution :
        // - alert supported
        // - message reçu = 2
        else if(supported && msgTrEv->text == "2")
        {
            Echoes::Dbo::AlertTrackingEvent *newStateAle = new Echoes::Dbo::AlertTrackingEvent();

            newStateAle->date = now;
            newStateAle->alert = ptrAle;
            Wt::Dbo::ptr<Echoes::Dbo::AlertStatus> alsPtr = m_session.find<Echoes::Dbo::AlertStatus>()
                .where(QUOTE(TRIGRAM_ALERT_STATUS ID) " = ?").bind(Echoes::Dbo::EAlertStatus::BACKTONORMAL)
                .where(QUOTE(TRIGRAM_ALERT_STATUS SEP "DELETE") " IS NULL");
            newStateAle->statut = alsPtr;

            Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent> newAleTrEv = m_session.add<Echoes::Dbo::AlertTrackingEvent>(newStateAle);
        }
        else
        {
            Wt::log("warning") <<  "asw received but invalid";
        }
    }
    else
    {
        Wt::log("warning") << "no existing event for this alert or message not found";
    }
}

EReturnCode ItookiAswReceiver::processPostRequest(const Wt::Http::Request &request, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    string nextElement = "";
    unsigned short indexPathElement = 1;
    vector<string> pathElements;
    map<string, long long> parameters;
    
    const string sRequest = processRequestParameters(request, pathElements, parameters);

    nextElement = getNextElementFromPath(indexPathElement, pathElements);

    if (nextElement.empty())
    {
        res = postAsw(parameters, pathElements, sRequest, responseMsg);
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        responseMsg = "chemin incorrect";
    }

    return res;
}

void ItookiAswReceiver::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
        EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
        string responseMsg = "";
        if(!request.method().compare("POST"))
        {
            res = processPostRequest(request, responseMsg);
        }
        else
        {
                res = EReturnCode::METHOD_NOT_ALLOWED;
                responseMsg = "{\n\t\"message\": \"Only POST method is allowed.\n\"}";
        }

        response.setStatus(res);
        response.out() << responseMsg;
}