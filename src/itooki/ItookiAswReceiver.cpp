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

#include "itooki/ItookiAswReceiver.h"

using namespace std;

ItookiAswReceiver::ItookiAswReceiver(Echoes::Dbo::Session& session): PublicItookiResource::PublicItookiResource(session)
{
    
}

ItookiAswReceiver::~ItookiAswReceiver()
{
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
        bool sameSupport = (AlertEventList.begin()->get()->user == msgTrEv->message->user);
        if(date_crea && pending && msgTrEv->text == "1")
        {
            Echoes::Dbo::AlertTrackingEvent *newStateAle = new Echoes::Dbo::AlertTrackingEvent();

            newStateAle->date = now;
            newStateAle->alert = ptrAle;
            Wt::Dbo::ptr<Echoes::Dbo::AlertStatus> alsPtr = m_session.find<Echoes::Dbo::AlertStatus>()
                .where(QUOTE(TRIGRAM_ALERT_STATUS ID) " = ?").bind(Echoes::Dbo::EAlertStatus::SUPPORTED)
                .where(QUOTE(TRIGRAM_ALERT_STATUS SEP "DELETE") " IS NULL");
            newStateAle->statut = alsPtr;
            newStateAle->user = msgTrEv->message->user;
            
            Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent> newAleTrEv = m_session.add<Echoes::Dbo::AlertTrackingEvent>(newStateAle);

        }
        //conditons pour la résolution :
        // - alert supported
        // c'est le meme user qui est en charge
        // - message reçu = 2
        else if(supported && sameSupport && msgTrEv->text == "2")
        {
            Echoes::Dbo::AlertTrackingEvent *newStateAle = new Echoes::Dbo::AlertTrackingEvent();

            newStateAle->date = now;
            newStateAle->alert = ptrAle;
            Wt::Dbo::ptr<Echoes::Dbo::AlertStatus> alsPtr = m_session.find<Echoes::Dbo::AlertStatus>()
                .where(QUOTE(TRIGRAM_ALERT_STATUS ID) " = ?").bind(Echoes::Dbo::EAlertStatus::BACKTONORMAL)
                .where(QUOTE(TRIGRAM_ALERT_STATUS SEP "DELETE") " IS NULL");
            newStateAle->statut = alsPtr;

            Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent> newAleTrEv = m_session.add<Echoes::Dbo::AlertTrackingEvent>(newStateAle);
            
            Wt::Http::Client *client = new Wt::Http::Client(this);
            
            string url = "http";
            if (conf.isSmsHttps())
            {
                url += "s";
            }
            url += "://" + conf.getRouteurHost() +":" + boost::lexical_cast<std::string>(conf.getRouteurPort()) + "/send/";
            url += boost::lexical_cast<std::string>(msgTrEv->message->refAck);
            Wt::Http::Message httpMessage;
            
            client->deleteRequest(url, httpMessage); 
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