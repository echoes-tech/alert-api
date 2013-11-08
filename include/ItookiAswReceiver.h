/* 
 * File:   SrReception.h
 * Author: tsa
 *
 * Created on 8 août 2012, 15:40
 */

#ifndef ASWRECEIVER_H
#define	ASWRECEIVER_H

#include <Wt/WResource>

#include "Conf.h"

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
            Echoes::Dbo::Session session(conf.getSessConnectParams());

            Wt::log("notice") << "[ASW] Query string : " << request.queryString();
            
            
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
                Echoes::Dbo::UserHistoricalAction *uha = new Echoes::Dbo::UserHistoricalAction;
                uha->dateTime = Wt::WDateTime::currentDateTime();
                uha->tableObject = this->number + " : " + this->message;
                session.add<Echoes::Dbo::UserHistoricalAction>(uha);
            }
            catch(Wt::Dbo::Exception const& e)
            {
                Wt::log("error") << "[ASW ITOOKI]" << e.what();
                //TODO : behaviour in error case
            }
            
            
        }
};

#endif	/* ASWRECEIVER_H */

