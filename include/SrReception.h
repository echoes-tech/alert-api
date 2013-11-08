/* 
 * File:   SrReception.h
 * Author: tsa
 *
 * Created on 8 août 2012, 15:40
 */

#ifndef SRRECEPTION_H
#define	SRRECEPTION_H

#include <Wt/WResource>

#include "Conf.h"

class SrReception : public Wt::WResource
{
    public:
        virtual ~SrReception()
        {
            beingDeleted();
        }

    protected:
        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
        {
            Echoes::Dbo::Session session(conf.getSessConnectParams());
            
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
            
            Wt::log("notice") << "[SR] " << requestBodyString;
            
            /** stream : mandatory for boost ptree */
            std::istringstream requestBodyStream(requestBodyString);
            
            /** publishing status=0 on the web page as result : netsize compliant */
            response.out() << "status=0";
                        
            /** boost ptree to parse the xml */
            boost::property_tree::ptree ptree;
            boost::property_tree::xml_parser::read_xml(requestBodyStream, ptree, boost::property_tree::xml_parser::trim_whitespace);
            
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
                gateway = ptree.get<std::string>("SR.GW");
                messageId = ptree.get<std::string>("SR.P1");
                messageStatus = ptree.get<std::string>("SR.P3");
                eventReason = ptree.get<std::string>("SR.P4");
                notificationDate = ptree.get<std::string>("SR.P5");
                port = ptree.get<std::string>("SR.PORT");
            }
            catch(std::exception const& e)
            {
                std::cout << e.what() << std::endl;
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
                    Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> at = session.find<Echoes::Dbo::AlertTracking>().where("\"ATR_ACK_ID\" = ?").bind(messageId);
                    if (Utils::checkId<Echoes::Dbo::AlertTracking>(at))
                    {
                        at.modify()->receiverSrv = gateway;
                        at.modify()->ackGw = gateway;
                        at.modify()->ackPort = port;
//                        at.modify()->ackEvent = eventReason;

                        Echoes::Dbo::AlertTrackingEvent *ate = new Echoes::Dbo::AlertTrackingEvent();
                        ate->alertTracking = at;
                        ate->value = eventReason;
                        ate->date = Wt::WDateTime::currentDateTime();

                        Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent> ptrAte = session.add(ate);
                    }
                    else
                    {
                        Wt::log("error") << "[SR] Alert tracking not found, inserting event without the tracking reference";
                        Echoes::Dbo::AlertTrackingEvent *ate = new Echoes::Dbo::AlertTrackingEvent();
                        ate->value = eventReason;
                        ate->date = Wt::WDateTime::currentDateTime();

                        Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent> ptrAte = session.add(ate);
                        //TODO error behavior
                    }
                }
                catch(Wt::Dbo::Exception const& e)
                {
                    Wt::log("error") << e.what();
                    //TODO : behaviour in error case
                }
            }
            
        }
};

#endif	/* SRRECEPTION_H */

