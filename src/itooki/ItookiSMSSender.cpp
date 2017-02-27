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

ItookiSMSSender::ItookiSMSSender(Echoes::Dbo::Session& session, Wt::WObject *parent) :
m_parent(parent),
m_session(session)
{
}

ItookiSMSSender::~ItookiSMSSender()
{
}

int ItookiSMSSender::send(const string &number, const string &message, Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> atrPtr)
{
    int res = -1;

    try
    {
        if(atrPtr)
        {
            Wt::Http::Client *client = new Wt::Http::Client(m_parent);
//            client->done().connect(boost::bind(&ItookiSMSSender::handleHttpResponse, this, client, _1, _2, atrPtr.id()));

            string url = "http";
            if (conf.isSmsHttps())
            {
                url += "s";
            }
            url += "://www.itooki.fr/http.php"
                    "?email=" + Wt::Utils::urlEncode(conf.getSmsLogin()) +
                    "&pass=" + Wt::Utils::urlEncode(conf.getSmsPassword()) +
                    "&numero=" + Wt::Utils::urlEncode(number) +
                    "&message=" + Wt::Utils::urlEncode(message) +
                    "&refaccus=o";

            Wt::log("info") << "[Itooki SMS Sender] Trying to send request to Itooki API";
            Wt::log("debug") << "[Itooki SMS Sender] Address : " << url;
            if (client->get(url))
            {
                Wt::log("info") << "[Itooki SMS Sender] Message sent to Itooki API";

                //TODO : hostname cpp way
                char hostname[255];
                gethostname(hostname, 255);
                atrPtr.modify()->senderSrv = hostname;
                atrPtr.modify()->content = Wt::WString::fromUTF8(message);
                atrPtr.modify()->sendDate = Wt::WDateTime::currentDateTime();

                res = 0;
            } 
            else 
            {
                Wt::log("error") << "[Itooki SMS Sender] Failed to send message to Itooki API";
            }
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

void ItookiSMSSender::handleHttpResponse(Wt::Http::Client *client, boost::system::error_code err, const Wt::Http::Message& response, const long long atrId)
{
    if (!err && response.status() == 200)
    {
        const string resultCode = response.body();

        Wt::log("debug") << "[Itooki SMS Sender][ACK] result code : " << resultCode;
        vector<string> splitResult;
        boost::split(splitResult, resultCode, boost::is_any_of("-"), boost::token_compress_on);

        switch (splitResult.size())
        {
            case 0:
                Wt::log("error") << "[Itooki SMS Sender][ACK] Unexpected answer from itooki, no result code.";
                break;
            case 2:
            {
                try
                {
                    Wt::Dbo::Transaction transaction(m_session, true);

                    Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> atrPtr = m_session.find<Echoes::Dbo::AlertTracking>()
                        .where(QUOTE(TRIGRAM_ALERT_TRACKING ID) " = ?").bind(atrId)
                        .where(QUOTE(TRIGRAM_ALERT_TRACKING SEP "DELETE") " IS NULL");

                    if (atrPtr)
                    {
                            atrPtr.modify()->ackId = splitResult[1];
                            atrPtr.modify()->ackGw = "itooki.fr";
                            atrPtr.modify()->receiveDate = Wt::WDateTime::currentDateTime();

                            Echoes::Dbo::AlertTrackingEvent *newAte = new Echoes::Dbo::AlertTrackingEvent();
                            newAte->alertTracking = atrPtr;
                            newAte->date = Wt::WDateTime::currentDateTime();
                            newAte->value = splitResult[0];

                            Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent> newAtePtr = m_session.add<Echoes::Dbo::AlertTrackingEvent>(newAte);
                            newAtePtr.flush();
                    }
                    else
                    {
                        Wt::log("error") << "[Itooki SMS Sender][ACK] Alert tracking not found";
                        //TODO error behavior
                    }
                    transaction.commit();
                }
                catch (Wt::Dbo::Exception const& e)
                {
                    Wt::log("error") << "[Itooki SMS Sender][ACK] " << e.what();
                    //TODO : behaviour in error case
                }
                break;
            }
            default:
                Wt::log("error") << "[Itooki SMS Sender][ACK] Unexpected answer from itooki.";
                break;
        }
    }
    else
    {
        Wt::log("error") << "[Itooki SMS Sender][ACK] Http::Client error: " << err.message();
    }

    delete client;
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
