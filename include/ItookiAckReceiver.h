/* 
 * File:   SrReception.h
 * Author: tsa
 *
 * Created on 8 août 2012, 15:40
 */

#ifndef ACKRECEIVER_H
#define	ACKRECEIVER_H

#include "includeFile.h"

class ItookiAckReceiver : public Wt::WResource
{
    public:
        virtual ~ItookiAckReceiver()
        {
            beingDeleted();
        }

    protected:
        std::string errorCode;
        std::string refSent;
        
        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
        {
            Session session(Utils::connection);
            
            Wt::log("notice") << "[SR] Client address : " << request.clientAddress();
            Wt::log("notice") << "[SR] Content type : " << request.contentType();
            Wt::log("notice") << "[SR] Content length : " << request.contentLength();
            Wt::log("notice") << "[SR] Url scheme : " << request.urlScheme();
            Wt::log("notice") << "[SR] Query string : " << request.queryString();
            Wt::log("notice") << "[SR] X-Forwarded-For : " << request.headerValue("X-Forwarded-For");
            
            /** string used to get the xml file sent in the request */
            std::string requestBodyString = "";
                       
            /** getting the input stream for the request char by char */
            char c;
            c = request.in().get();
            while (request.in())
            {
                requestBodyString.append(1,c);
                c = request.in().get();
            }
            
            Wt::log("notice") << "[ACK ITOOKI] " << requestBodyString;
            
                    
            if (!request.getParameterValues("erreur").empty())
            {
                this->errorCode = request.getParameterValues("erreur")[0];
            }
            else
            {
                Wt::log("error") << "[ACK ITOOKI]" << "no error code";
            }
            
            if (!request.getParameterValues("refenvoi").empty()) 
            {
                this->refSent = request.getParameterValues("refenvoi")[0];
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
                messageId = this->refSent;
                messageStatus = this->errorCode;
                eventReason = this->errorCode;
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
                    Wt::Dbo::Transaction transaction(session);
                    Wt::Dbo::ptr<AlertTracking> at = session.find<AlertTracking>().where("\"ATR_ACK_ID\" = ?").bind(messageId);
                    if (Utils::checkId<AlertTracking>(at))
                    {
                        at.modify()->receiverSrv = gateway;
                        at.modify()->ackGw = gateway;
//                        at.modify()->ackPort = port;

                        AlertTrackingEvent *ate = new AlertTrackingEvent();
                        ate->alertTracking = at;
                        ate->value = eventReason;
                        ate->date = Wt::WDateTime::currentDateTime();

                        Wt::Dbo::ptr<AlertTrackingEvent> ptrAte = session.add(ate);
                    }
                    else
                    {
                        Wt::log("error") << "[ACK ITOOKI] Alert tracking not found, inserting event without the tracking reference";
                        AlertTrackingEvent *ate = new AlertTrackingEvent();
                        ate->value = eventReason;
                        ate->date = Wt::WDateTime::currentDateTime();

                        Wt::Dbo::ptr<AlertTrackingEvent> ptrAte = session.add(ate);
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
};

#endif	/* ACKRECEIVER_H */

