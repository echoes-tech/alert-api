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
            Wt::Http::Client *client = new Wt::Http::Client(m_parent);
            client->done().connect(boost::bind(&ItookiSMSSender::handleHttpResponse, this, client, _1, _2, msgPtr.id()));

            string url = "http";
            if (conf.isSmsHttps())
            {
                url += "s";
            }
            url += "://" + conf.getRouteurHost() +":" + boost::lexical_cast<std::string>(conf.getRouteurPort()) + "/send";
            string json = "{";
            json += "\"number\" : \"" + number + "\"";
            json += "\"message\" : \"" + message + "\"";
            json += "\"port\" : \""+ boost::lexical_cast<std::string>(conf.getServerPort()) + "\"";
            json += "}";
            
            Wt::Http::Message httpMessage;
            httpMessage.addBodyText(json);
            
            client->post(url, httpMessage); 
            res = 0;
            
            
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

void ItookiSMSSender::handleHttpResponse(Wt::Http::Client *client, boost::system::error_code err, const Wt::Http::Message& response, const long long msgId)
{
    const Wt::WDateTime now = Wt::WDateTime::currentDateTime();
    
    Wt::Dbo::Transaction transaction(m_session);
    
    Wt::Dbo::ptr<Echoes::Dbo::Message> msgPtr = m_session.find<Echoes::Dbo::Message>()
                        .where(QUOTE(TRIGRAM_MESSAGE ID) " = ?").bind(msgId)
                        .where(QUOTE(TRIGRAM_MESSAGE SEP "DELETE") " IS NULL");
      
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
            Wt::log("error") << "[Itooki SMS Sender] routeur ok: " ;
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
    cout << "ok back of send" << endl;
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

