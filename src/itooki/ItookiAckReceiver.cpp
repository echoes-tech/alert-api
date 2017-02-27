/**
 * @file ItookiAckReceiver.cpp
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

#include "itooki/ItookiAckReceiver.h"

using namespace std;

ItookiAckReceiver::ItookiAckReceiver(Echoes::Dbo::Session& session): PublicItookiResource::PublicItookiResource(session)
{
    
}

ItookiAckReceiver::~ItookiAckReceiver()
{
}


EReturnCode ItookiAckReceiver::postAck(map<string, long long> parameters, const vector<string> &pathElements, const string &sRequest, string &responseMsg)
{
    const Wt::WDateTime now = Wt::WDateTime::currentDateTime();
    
    Wt::Dbo::Transaction transaction(m_session, true);
    
    EReturnCode res = EReturnCode::OK;
    
    bool    sended = false;
    Wt::WString  error = "missing";
    Wt::WString  refenvoi = "missing";
    
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
                if (result.contains("error"))
                {
                    error = result.get("error");
                }
                if (result.contains("sended"))
                {
                    Wt::WString recu = result.get("sended");                 
                    sended = (recu == "true");
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
            if(sended)
            {
                Wt::Dbo::ptr<Echoes::Dbo::Message> msgPtr = m_session.find<Echoes::Dbo::Message>()
                        .where(QUOTE(TRIGRAM_MESSAGE SEP "REF" SEP "ACK") " = ?").bind(refenvoi)
                        .where(QUOTE(TRIGRAM_MESSAGE SEP "DELETE") " IS NULL");
                
                if(msgPtr)
                {
                    Echoes::Dbo::MessageTrackingEvent *newStateMsg = new Echoes::Dbo::MessageTrackingEvent();

                    newStateMsg->date = now;
                    newStateMsg->message = msgPtr;
                    Wt::Dbo::ptr<Echoes::Dbo::MessageStatus> mstPtr = m_session.find<Echoes::Dbo::MessageStatus>()
                                    .where(QUOTE(TRIGRAM_MESSAGE_STATUS ID) " = ?").bind(Echoes::Dbo:: EMessageStatus::RECEIVED)
                                    .where(QUOTE(TRIGRAM_MESSAGE_STATUS SEP "DELETE") " IS NULL");
                    newStateMsg->statut = mstPtr;

                    Wt::Dbo::ptr<Echoes::Dbo::MessageTrackingEvent> newMsgTrEv = m_session.add<Echoes::Dbo::MessageTrackingEvent>(newStateMsg);
                }
                else
                {
                    Wt::log("error") << "[Itooki ack Receiver] No Message " << refenvoi;
                    res = EReturnCode::BAD_REQUEST;
                    responseMsg = "no message";   
                }
            }
            else
            {
                Wt::Dbo::ptr<Echoes::Dbo::Message> msgPtr = m_session.find<Echoes::Dbo::Message>()
                        .where(QUOTE(TRIGRAM_MESSAGE SEP "REF" SEP "ACK") " = ?").bind(refenvoi)
                        .where(QUOTE(TRIGRAM_MESSAGE SEP "DELETE") " IS NULL");
                
                if(msgPtr)
                {
                    Echoes::Dbo::MessageTrackingEvent *newStateMsg = new Echoes::Dbo::MessageTrackingEvent();

                    newStateMsg->date = now;
                    newStateMsg->message = msgPtr;
                    Wt::Dbo::ptr<Echoes::Dbo::MessageStatus> mstPtr = m_session.find<Echoes::Dbo::MessageStatus>()
                                    .where(QUOTE(TRIGRAM_MESSAGE_STATUS ID) " = ?").bind(Echoes::Dbo:: EMessageStatus::ACKFAILED)
                                    .where(QUOTE(TRIGRAM_MESSAGE_STATUS SEP "DELETE") " IS NULL");
                    newStateMsg->statut = mstPtr;

                    Wt::Dbo::ptr<Echoes::Dbo::MessageTrackingEvent> newMsgTrEv = m_session.add<Echoes::Dbo::MessageTrackingEvent>(newStateMsg);
                }
                else
                {
                    Wt::log("error") << "[Itooki Ack Receiver] No Message " << refenvoi ;
                    res = EReturnCode::BAD_REQUEST;
                    responseMsg = "no message";   
                }
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

EReturnCode ItookiAckReceiver::processPostRequest(const Wt::Http::Request &request, std::string &responseMsg)
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
        res = postAck(parameters, pathElements, sRequest, responseMsg);
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        responseMsg = "chemin incorrect";
    }

    return res;
}
