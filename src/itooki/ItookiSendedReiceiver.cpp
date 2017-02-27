/**
 * @file ItookiSendedReiceiver.cpp
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

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <Wt/Dbo/SqlConnection>
#include <Wt/Dbo/Session>
#include <Wt/WTimer>

#include "itooki/ItookiSendedReceiver.h"

using namespace std;

ItookiSendedReceiver::ItookiSendedReceiver(Echoes::Dbo::Session& session): PublicItookiResource::PublicItookiResource(session)
{
    
}

ItookiSendedReceiver::~ItookiSendedReceiver()
{
}

EReturnCode ItookiSendedReceiver::postSended(map<string, long long> parameters, const vector<string> &pathElements, const string &sRequest, string &responseMsg)
{
    const Wt::WDateTime now = Wt::WDateTime::currentDateTime();
    
    EReturnCode res = EReturnCode::OK;
    
    bool    sended = false;
    Wt::WString  error = "missing";
    Wt::WString  refenvoi = "missing";
    Wt::WString  refenvoiToChange = "missing";
    
    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            if (result.contains("refenvoiToChange"))
            {
                refenvoiToChange = result.get("refenvoiToChange");
            }
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
                sended = result.get("sended");
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
        
        Wt::Dbo::Transaction transaction(m_session, true);
        
        Wt::Dbo::ptr<Echoes::Dbo::Message> msgPtr = m_session.find<Echoes::Dbo::Message>()
                .where(QUOTE(TRIGRAM_MESSAGE SEP "REF" SEP "ACK") " = ?").bind(refenvoiToChange)
                .where(QUOTE(TRIGRAM_MESSAGE SEP "DELETE") " IS NULL");
        
        if(msgPtr)
        {
            msgPtr.modify()->refAck = refenvoi;
            Echoes::Dbo::MessageTrackingEvent *newStateMsg = new Echoes::Dbo::MessageTrackingEvent();

            newStateMsg->date = now;
            newStateMsg->message = msgPtr;
            Wt::Dbo::ptr<Echoes::Dbo::MessageStatus> mstPtr;
            if(sended)
            {
                mstPtr = m_session.find<Echoes::Dbo::MessageStatus>()
                    .where(QUOTE(TRIGRAM_MESSAGE_STATUS ID) " = ?").bind(Echoes::Dbo::EMessageStatus::SENDED)
                    .where(QUOTE(TRIGRAM_MESSAGE_STATUS SEP "DELETE") " IS NULL");
            }
            else
            {
                mstPtr = m_session.find<Echoes::Dbo::MessageStatus>()
                    .where(QUOTE(TRIGRAM_MESSAGE_STATUS ID) " = ?").bind(Echoes::Dbo::EMessageStatus::SENDFAILED)
                    .where(QUOTE(TRIGRAM_MESSAGE_STATUS SEP "DELETE") " IS NULL");
                newStateMsg->text = error;
            }
            newStateMsg->statut = mstPtr;

            Wt::Dbo::ptr<Echoes::Dbo::MessageTrackingEvent> newMsgTrEv = m_session.add<Echoes::Dbo::MessageTrackingEvent>(newStateMsg);
        }
        else
        {
            Wt::log("error") << "[Itooki Sended Receiver] No Message " << refenvoi << " and " << refenvoiToChange <<". Lauch of timer.";
            timer = new Wt::WTimer;
            timer->setInterval(60000);
            timer->timeout().connect(boost::bind(&ItookiSendedReceiver::endTimer, this, refenvoiToChange, refenvoi, error, sended));
            timer->start();
            res = EReturnCode::BAD_REQUEST;
            const string err = "[Itooki Sended Receiver] No Message";
            responseMsg = httpCodeToJSON(res, err);
        }
        
        transaction.commit();
    }
    else
    {
        Wt::log("error") << "[Itooki Sended Receiver] sRequest is empty ";
            res = EReturnCode::BAD_REQUEST;
            const string err = "[Itooki Sended Receiver] sRequest is empty";
            responseMsg = httpCodeToJSON(res, err);
    }
    return (res);
}

EReturnCode ItookiSendedReceiver::processPostRequest(const Wt::Http::Request &request, std::string &responseMsg)
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
        res = postSended(parameters, pathElements, sRequest, responseMsg);
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        responseMsg = "chemin incorrect";
    }

    return res;
}

void ItookiSendedReceiver::endTimer(Wt::WString refenvoiToChange, Wt::WString refenvoi, Wt::WString error, bool sended)
{
    const Wt::WDateTime now = Wt::WDateTime::currentDateTime();
    Wt::Dbo::Transaction transaction(m_session, true);
        
    Wt::Dbo::ptr<Echoes::Dbo::Message> msgPtr = m_session.find<Echoes::Dbo::Message>()
            .where(QUOTE(TRIGRAM_MESSAGE SEP "REF" SEP "ACK") " = ?").bind(refenvoiToChange)
            .where(QUOTE(TRIGRAM_MESSAGE SEP "DELETE") " IS NULL");
    
    if(msgPtr)
        {
            msgPtr.modify()->refAck = refenvoi;
            Echoes::Dbo::MessageTrackingEvent *newStateMsg = new Echoes::Dbo::MessageTrackingEvent();

            newStateMsg->date = now;
            newStateMsg->message = msgPtr;
            Wt::Dbo::ptr<Echoes::Dbo::MessageStatus> mstPtr;
            if(sended)
            {
                mstPtr = m_session.find<Echoes::Dbo::MessageStatus>()
                    .where(QUOTE(TRIGRAM_MESSAGE_STATUS ID) " = ?").bind(Echoes::Dbo::EMessageStatus::SENDED)
                    .where(QUOTE(TRIGRAM_MESSAGE_STATUS SEP "DELETE") " IS NULL");
            }
            else
            {
                mstPtr = m_session.find<Echoes::Dbo::MessageStatus>()
                    .where(QUOTE(TRIGRAM_MESSAGE_STATUS ID) " = ?").bind(Echoes::Dbo::EMessageStatus::SENDFAILED)
                    .where(QUOTE(TRIGRAM_MESSAGE_STATUS SEP "DELETE") " IS NULL");
                newStateMsg->text = error;
            }
            newStateMsg->statut = mstPtr;

            Wt::Dbo::ptr<Echoes::Dbo::MessageTrackingEvent> newMsgTrEv = m_session.add<Echoes::Dbo::MessageTrackingEvent>(newStateMsg);
        }
        else
        {
            Wt::log("error") << "[Itooki Sended Receiver] End of timer for " << refenvoiToChange << ". Declared as lost";
            
        }
    
    transaction.commit();
    timer->stop();
}
