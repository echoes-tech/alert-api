/* 
 * Itooki SMS Acknowledgement Receiver
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

#include "itooki/ItookiAckReceiver.h"

using namespace std;

ItookiAckReceiver::ItookiAckReceiver(Echoes::Dbo::Session& session) : Wt::WResource(),
m_session(session)
{
}

ItookiAckReceiver::~ItookiAckReceiver()
{
    beingDeleted();
}

void ItookiAckReceiver::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    Wt::log("notice") << "[ACK] Query string : " << request.queryString();

    if (!request.getParameterValues("erreur").empty())
    {
        m_errorCode = request.getParameterValues("erreur")[0];
    }
    else
    {
        Wt::log("error") << "[ACK ITOOKI]" << "no error code";
    }

    if (!request.getParameterValues("msgid").empty()) 
    {
        m_refSent = request.getParameterValues("msgid")[0];
    }
    else
    {
        Wt::log("error") << "[ACK ITOOKI]" << "no ref sent";
    }


    /** everything we need in the xml */
    std::string gateway = "";
    std::string messageId = "";
    std::string messageStatus = "";
    std::string eventReason = "";
    std::string notificationDate = "";
    std::string port = "";

    /** try catch to handle a corrupted file */
    try
    {
        gateway = "itooki.fr";
        messageId = m_refSent;
        messageStatus = m_errorCode;
        eventReason = m_errorCode;
//                notificationDate = ptree.get<std::string>("SR.P5");
//                port = ptree.get<std::string>("SR.PORT");
    }
    catch(std::exception const& e)
    {
        Wt::log("error") << "[ACK ITOOKI]" << e.what();
        //TODO : behaviour in error case
        return;
    }


    Wt::log("notice") << "[SR] " << "gateway : " << gateway;
    Wt::log("notice") << "[SR] " << "messageId : " << messageId;
    Wt::log("notice") << "[SR] " << "messageStatus : " << messageStatus;
    Wt::log("notice") << "[SR] " << "eventReason : " << eventReason;
    Wt::log("notice") << "[SR] " << "notificationDate : " << notificationDate;
    Wt::log("notice") << "[SR] " << "port : " << port;

    // new transaction
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session, true);
            Wt::Dbo::ptr<Echoes::Dbo::Message> atrPtr = m_session.find<Echoes::Dbo::Message>().where("\"MSG_ACK_ID\" = ?").bind(messageId);
            if (atrPtr)
            {
                atrPtr.modify()->receiverSrv = gateway;
                atrPtr.modify()->ackGw = gateway;
//                        at.modify()->ackPort = port;

                Echoes::Dbo::AlertTrackingEvent *ate = new Echoes::Dbo::AlertTrackingEvent();
                ate->alertTracking = atrPtr;
                ate->value = eventReason;
                ate->date = Wt::WDateTime::currentDateTime();

                Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent> ptrAte = m_session.add(ate);
            }
            else
            {
                Wt::log("error") << "[ACK ITOOKI] Alert tracking not found, inserting event without the tracking reference";
                Echoes::Dbo::AlertTrackingEvent *ate = new Echoes::Dbo::AlertTrackingEvent();
                ate->value = eventReason;
                ate->date = Wt::WDateTime::currentDateTime();

                Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent> ptrAte = m_session.add(ate);
                //TODO error behavior
            }
        }
        catch(Wt::Dbo::Exception const& e)
        {
            Wt::log("error") << "[ACK ITOOKI]" << e.what();
            //TODO : behaviour in error case
        }
    }
}

