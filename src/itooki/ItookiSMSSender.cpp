/* 
 * Itooki SMS Sender
 * @author ECHOES Technologies (FPO)
 * @date 15/11/2012
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2012-2013 BY ECHOES TECHNOLGIES SAS
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

int ItookiSMSSender::send(const string &number, const string &message, const long long alertID, Wt::Dbo::ptr<Echoes::Dbo::Message> atrPtr)
{
    int res = -1;
    string ackRecv = "", ackSlvd = "", adresseGUI = "127.0.0:8080";
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
            cout << alertID << endl;
            ackRecv += "***click here : http://" + 
                    adresseGUI + 
                    "/alert/"+ to_string(alertID) + 
                    "/received to confirm reception";
            ackSlvd += "***click here : http://" + 
                    adresseGUI + 
                    "/alert/"+ to_string(alertID) + 
                    "/solved when problem is solved";
            url += "://www.itooki.fr/http.php"
                    "?email=" + Wt::Utils::urlEncode(conf.getSmsLogin()) +
                    "&pass=" + Wt::Utils::urlEncode(conf.getSmsPassword()) +
                    "&numero=" + Wt::Utils::urlEncode(number) +
                    "&message=" + Wt::Utils::urlEncode(message) + 
                    Wt::Utils::urlEncode(ackRecv) +
                    Wt::Utils::urlEncode(ackSlvd) +
                    "&refaccus=o";

            Wt::log("info") << "[Itooki SMS Sender] Trying to send request to Itooki API";
            Wt::log("debug") << "[Itooki SMS Sender] Address : " << url;
            /*if (client->get(url))
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
            }*/
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

                    Wt::Dbo::ptr<Echoes::Dbo::Message> atrPtr = m_session.find<Echoes::Dbo::Message>()
                        .where(QUOTE(TRIGRAM_MESSAGE ID) " = ?").bind(atrId)
                        .where(QUOTE(TRIGRAM_MESSAGE SEP "DELETE") " IS NULL");

                    if (atrPtr)
                    {
                            atrPtr.modify()->ackId = splitResult[1];
                            atrPtr.modify()->ackGw = "itooki.fr";
                            atrPtr.modify()->receiveDate = Wt::WDateTime::currentDateTime();

                            Echoes::Dbo::MessageTrackingEvent *newMte = new Echoes::Dbo::MessageTrackingEvent();
                            newMte->message = atrPtr;
                            newMte->date = Wt::WDateTime::currentDateTime();
                            newMte->value = splitResult[0];

                            Wt::Dbo::ptr<Echoes::Dbo::MessageTrackingEvent> newAtePtr = m_session.add<Echoes::Dbo::MessageTrackingEvent>(newMte);
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

