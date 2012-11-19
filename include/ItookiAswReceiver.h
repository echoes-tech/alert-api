/* 
 * File:   SrReception.h
 * Author: tsa
 *
 * Created on 8 août 2012, 15:40
 */

#ifndef ASWRECEIVER_H
#define	ASWRECEIVER_H

#include "includeFile.h"

class ItookiAswReceiver : public Wt::WResource
{
    public:
        virtual ~ItookiAswReceiver()
        {
            beingDeleted();
        }

    protected:
        std::string number;
        std::string message;
        
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
            
            
            this->number = "";
            this->message = "";
                    
            if (!request.getParameterValues("numero").empty())
            {
                this->number = request.getParameterValues("numero")[0];
            }
            if (!request.getParameterValues("message").empty()) 
            {
                this->message = request.getParameterValues("message")[0];
            }
            
            


            
            // new transaction
            
            try
            {
                Wt::Dbo::Transaction transaction(session);
                UserHistoricalAction *uha = new UserHistoricalAction;
                uha->dateTime = Wt::WDateTime::currentDateTime();
                uha->tableObject = this->number + " : " + this->message;
                session.add<UserHistoricalAction>(uha);
            }
            catch(Wt::Dbo::Exception const& e)
            {
                Wt::log("error") << "[ACK ITOOKI]" << e.what();
                //TODO : behaviour in error case
            }
            
            
        }
};

#endif	/* ASWRECEIVER_H */

