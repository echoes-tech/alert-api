/**
 * @file ItookiSMSSender.cpp
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

#include "itooki/ItookiSMSSender.h"

using namespace std;

ItookiSMSSender::ItookiSMSSender(Echoes::Dbo::Session& session) : PublicItookiResource::PublicItookiResource(session)
{
}

ItookiSMSSender::~ItookiSMSSender()
{
}

int ItookiSMSSender::send(const string &number, const string &message, const long long alertID, Wt::Dbo::ptr<Echoes::Dbo::Message> msgPtr)
{
    int res = -1;
    string ackRecv = "", ackSlvd = "";
    
    try
    {
        if(msgPtr)
        {
            Wt::Http::Client *client = new Wt::Http::Client(this);
            long long msgPtrId = msgPtr.id();
            client->done().connect(boost::bind(&ItookiSMSSender::handleHttpResponse, this, _1, _2, msgPtrId));
            
            string url = "http";
            if (conf.isSmsHttps())
            {
                url += "s";
            }
            url += "://" + conf.getRouteurHost() +":" + boost::lexical_cast<std::string>(conf.getRouteurPort()) + "/send";
            string json = "{";
            json += "\"number\" : \"" + number + "\",";
            json += "\"message\" : \"" + message + "\",";
            json += "\"port_back\" : " + boost::lexical_cast<std::string>(conf.getServerPort()) + ",";
            json += "\"FQDN_back\" : \"" + boost::lexical_cast<std::string>(conf.getFQDN()) + "\"";
            json += "}";
            
            Wt::Http::Message httpMessage;
            httpMessage.addBodyText(json);
            
            Wt::log("debug") << " [Itooki Sms Sender Resource] preparation ok : " << url << " => " << json;
            
            if(client->post(url, httpMessage))
            {
                const Wt::WDateTime now = Wt::WDateTime::currentDateTime();

                Wt::Dbo::ptr<Echoes::Dbo::Message> msgPtr = m_session.find<Echoes::Dbo::Message>()
                        .where(QUOTE(TRIGRAM_MESSAGE ID) " = ?").bind(msgPtrId)
                        .where(QUOTE(TRIGRAM_MESSAGE SEP "DELETE") " IS NULL");
                
                Echoes::Dbo::MessageTrackingEvent *newStateMsg = new Echoes::Dbo::MessageTrackingEvent();

                newStateMsg->date = now;
                newStateMsg->message = msgPtr;
                Wt::Dbo::ptr<Echoes::Dbo::MessageStatus> mstPtr = m_session.find<Echoes::Dbo::MessageStatus>()
                                .where(QUOTE(TRIGRAM_MESSAGE_STATUS ID) " = ?").bind(Echoes::Dbo::EMessageStatus::LINKING)
                                .where(QUOTE(TRIGRAM_MESSAGE_STATUS SEP "DELETE") " IS NULL");
                newStateMsg->statut = mstPtr;
                newStateMsg->text = "";

                Wt::Dbo::ptr<Echoes::Dbo::MessageTrackingEvent> newMsgTrEv = m_session.add<Echoes::Dbo::MessageTrackingEvent>(newStateMsg);
            }
            res = 0;
            
            Wt::log("debug") << "fin";
        }
        else 
        {
            Wt::log("error") << "[Itooki SMS Sender] atrPtr is empty. No SMS sent";
        }
    }
    catch (Wt::Dbo::Exception const& e)
    {
        Wt::log("error") << "[Itooki SMS Sender] " << e.what();
    }

    return res;
}

void ItookiSMSSender::handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response, const long long msgId)
{
    const Wt::WDateTime now = Wt::WDateTime::currentDateTime();
    
    Wt::Dbo::Transaction transaction(m_session, true);
    
    const string queryStr =
" SELECT msg"
"   FROM " QUOTE("T_MESSAGE_MSG") " msg"
"   WHERE"
"     " QUOTE(TRIGRAM_MESSAGE ID) " = " + boost::lexical_cast<string>(msgId) +
"   LIMIT 1";

    Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Message>> queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::Message>> (queryStr);

    Wt::Dbo::ptr<Echoes::Dbo::Message> msgPtr = queryRes.resultValue();
      
    if (!err && response.status() == 200)
    {
        const string responseBody = response.body();
        Wt::WString ref = "parse error";
        bool isOk = false;
        
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(responseBody, result);	
            if (result.contains("ref"))
            {
                ref = result.get("ref");
            }
            if (result.contains("ok"))
            {
                isOk = result.get("ok");
            }
        }
        catch (Wt::Json::ParseError const& e)
        {
            Wt::log("warning") << "JsonParseError reading back of send request";
        }
        catch (Wt::Json::TypeException const& e)	
        {
            Wt::log("warning") << "JsonTypeError reading back of send request";
        }
        if(isOk)
        {
            msgPtr.modify()->refAck = ref;
            Wt::log("debug") << "[Itooki SMS Sender] routeur ok " ;
            Echoes::Dbo::MessageTrackingEvent *newStateMsg = new Echoes::Dbo::MessageTrackingEvent();

            newStateMsg->date = now;
            newStateMsg->message = msgPtr;
            Wt::Dbo::ptr<Echoes::Dbo::MessageStatus> mstPtr = m_session.find<Echoes::Dbo::MessageStatus>()
                            .where(QUOTE(TRIGRAM_MESSAGE_STATUS ID) " = ?").bind(Echoes::Dbo::EMessageStatus::CREATED)
                            .where(QUOTE(TRIGRAM_MESSAGE_STATUS SEP "DELETE") " IS NULL");
            newStateMsg->statut = mstPtr;
            newStateMsg->text = response.body();

            Wt::Dbo::ptr<Echoes::Dbo::MessageTrackingEvent> newMsgTrEv = m_session.add<Echoes::Dbo::MessageTrackingEvent>(newStateMsg);
        }
        else
        {
            msgPtr.modify()->refAck = "failed";
            Wt::log("error") << "[Itooki SMS Sender] routeur error: " << err.message();
            Echoes::Dbo::MessageTrackingEvent *newStateMsg = new Echoes::Dbo::MessageTrackingEvent();

            newStateMsg->date = now;
            newStateMsg->message = msgPtr;
            Wt::Dbo::ptr<Echoes::Dbo::MessageStatus> mstPtr = m_session.find<Echoes::Dbo::MessageStatus>()
                            .where(QUOTE(TRIGRAM_MESSAGE_STATUS ID) " = ?").bind(Echoes::Dbo::EMessageStatus::SENDREFUSED)
                            .where(QUOTE(TRIGRAM_MESSAGE_STATUS SEP "DELETE") " IS NULL");
            newStateMsg->statut = mstPtr;
            newStateMsg->text = response.body();

            Wt::Dbo::ptr<Echoes::Dbo::MessageTrackingEvent> newMsgTrEv = m_session.add<Echoes::Dbo::MessageTrackingEvent>(newStateMsg);
        }
    }
    else
    {
        msgPtr.modify()->refAck = "failed";
        Wt::log("error") << "[Itooki SMS Sender][ACK] Http::Client error: " << err.message();
        Echoes::Dbo::MessageTrackingEvent *newStateMsg = new Echoes::Dbo::MessageTrackingEvent();
                    
        newStateMsg->date = now;
        newStateMsg->message = msgPtr;
        Wt::Dbo::ptr<Echoes::Dbo::MessageStatus> mstPtr = m_session.find<Echoes::Dbo::MessageStatus>()
                        .where(QUOTE(TRIGRAM_MESSAGE_STATUS ID) " = ?").bind(Echoes::Dbo::EMessageStatus::SENDREFUSED)
                        .where(QUOTE(TRIGRAM_MESSAGE_STATUS SEP "DELETE") " IS NULL");
        newStateMsg->statut = mstPtr;
        newStateMsg->text = response.body();
                    
        Wt::Dbo::ptr<Echoes::Dbo::MessageTrackingEvent> newMsgTrEv = m_session.add<Echoes::Dbo::MessageTrackingEvent>(newStateMsg);
    }
    
    transaction.commit();
}

void ItookiSMSSender::setParent(Wt::WObject* parent)
{
    m_parent = parent;
    return;
}

Wt::WObject* ItookiSMSSender::getParent() const
{
    return m_parent;
}

std::string ItookiSMSSender::urlEncodeMessage(const std::string message)
{
    std::string ret = "";

    //do the same work than Wt::Utils::urlEncode but transform
    //the caracter \n into *** wixh is the \n for itooki
    for (unsigned int i = 0; i < message.length(); i++)
    {
        ret += (message[i] == '\n') ? "***" : string(1, message[i]);
    }
    ret = Wt::Utils::urlEncode(ret);
    
    return (ret);
}
