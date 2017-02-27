/**
 * @file SrReception.h
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
                    Wt::Dbo::ptr<Echoes::Dbo::Message> msg = session.find<Echoes::Dbo::Message>().where("\"MSG_ACK_ID\" = ?").bind(messageId);
                    if (Utils::checkId<Echoes::Dbo::Message>(msg))
                    {
                        msg.modify()->receiverSrv = gateway;
                        msg.modify()->ackGw = gateway;
                        msg.modify()->ackPort = port;
//                        at.modify()->ackEvent = eventReason;

                        Echoes::Dbo::MessageTrackingEvent *mte = new Echoes::Dbo::MessageTrackingEvent();
                        mte->message = msg;
                        mte->value = eventReason;
                        mte->date = Wt::WDateTime::currentDateTime();

                        Wt::Dbo::ptr<Echoes::Dbo::MessageTrackingEvent> ptrAte = session.add(mte);
                    }
                    else
                    {
                        Wt::log("error") << "[SR] Alert tracking not found, inserting event without the tracking reference";
                        Echoes::Dbo::MessageTrackingEvent *ate = new Echoes::Dbo::MessageTrackingEvent();
                        ate->value = eventReason;
                        ate->date = Wt::WDateTime::currentDateTime();

                        Wt::Dbo::ptr<Echoes::Dbo::MessageTrackingEvent> ptrAte = session.add(ate);
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
