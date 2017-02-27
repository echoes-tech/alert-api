/**
 * @file ItookiAswReceiver.cpp
 * @author Thomas Saquet, Florent Poinsaut
 * @date 
 * @brief File containing example of doxygen usage for quick reference.
 *
 * Alert - API is a part of the Alert software
 * Copyright (C) 2013-2017
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
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
    
    Wt::Dbo::Transaction transaction(m_session, true);
    
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
            if(msgPtr)
            {
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
                Wt::log("error") << "[Itooki asw Receiver] No Message " << refenvoi << " " << message;
                res = EReturnCode::BAD_REQUEST;
                responseMsg = "no message";  
            }
     }
     else
        {
            res = EReturnCode::BAD_REQUEST;
            const string err = "[Send Resource] sRequest is not empty";
            responseMsg = httpCodeToJSON(res, err);
        }
    transaction.commit();
    return (res);
}

void ItookiAswReceiver::operationOnAsw(Wt::Dbo::ptr<Echoes::Dbo::MessageTrackingEvent> msgTrEv)
{
    Wt::Dbo::ptr<Echoes::Dbo::Alert> ptrAle = msgTrEv->message->alert;
    
    const Wt::WDateTime now = Wt::WDateTime::currentDateTime();
    
    
    Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent>> AlertEventList = m_session.find<Echoes::Dbo::AlertTrackingEvent>()
                    .where(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP TRIGRAM_ALERT SEP TRIGRAM_ALERT ID) " = ?").bind(ptrAle.id())
                    .where(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP "DELETE") " IS NULL")
                    .orderBy(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP "DATE") " DESC");
            
    
    Wt::Dbo::ptr<Echoes::Dbo::MessageTrackingEvent> msgEvCreaPtr = m_session.find<Echoes::Dbo::MessageTrackingEvent>()
            .where(QUOTE(TRIGRAM_MESSAGE_TRACKING_EVENT SEP TRIGRAM_MESSAGE_STATUS SEP TRIGRAM_MESSAGE_STATUS ID) " = ?").bind(Echoes::Dbo::EMessageStatus::CREATED)
            .where(QUOTE(TRIGRAM_MESSAGE_TRACKING_EVENT SEP TRIGRAM_MESSAGE SEP TRIGRAM_MESSAGE ID) " = ?").bind(msgTrEv->message.id())
            .where(QUOTE(TRIGRAM_MESSAGE_TRACKING_EVENT SEP "DELETE") " IS NULL");
    
    if(AlertEventList.size() > 0 && msgEvCreaPtr)
    {
        // conditions pour la prise en charge:
        // - alert pending
        // - date cration du pending < date etat created du message
        // - message reçu = 1
        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent>>::iterator AlertEventIt = AlertEventList.begin();
        bool date_crea = (AlertEventIt->get()->date < msgEvCreaPtr->date);
        bool pending = (AlertEventIt->get()->statut.id() == Echoes::Dbo::EAlertStatus::PENDING);
        bool supported = (AlertEventIt->get()->statut.id() == Echoes::Dbo::EAlertStatus::SUPPORTED);
        bool sameSupport = (AlertEventIt->get()->user == msgTrEv->message->user);
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

            newStateAle->user = msgTrEv->message->user;
            
            Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent> newAleTrEv = m_session.add<Echoes::Dbo::AlertTrackingEvent>(newStateAle);
            
            Wt::Http::Client *client = new Wt::Http::Client(this);
            
            string url = "http";
            if (conf.isSmsHttps())
            {
                url += "s";
            }
            Wt::log("info") << "[Itooki asw Receiver] ref ack to send : " << msgTrEv->message->refAck;
            url += "://" + conf.getRouteurHost() +":" + boost::lexical_cast<std::string>(conf.getRouteurPort()) + "/send/" + msgTrEv->message->refAck.get().toUTF8();
            Wt::Http::Message httpMessage;
            
            client->deleteRequest(url, httpMessage); 
        }
        else
        {
            Wt::log("error") << "[Itooki asw Receiver] error coonditions " << msgTrEv->text;

        }
    }
    else
    {
        Wt::log("error") << "[Itooki asw Receiver] error database " << msgTrEv->text; 
        if(AlertEventList.size() <= 0)
            Wt::log("error") << "[Itooki asw Receiver] no alert event for id " << ptrAle.id();
        else
            Wt::log("error") << "[Itooki asw Receiver] message created event not found for id " << msgTrEv->message.id();
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
