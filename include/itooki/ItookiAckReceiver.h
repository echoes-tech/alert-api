/* 
 * Header of Itooki SMS Acknowledgement Receiver
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

#ifndef ACKRECEIVER_H
#define	ACKRECEIVER_H

#include <Wt/WResource>

#include <tools/Session.h>

#include "Conf.h"

class ItookiAckReceiver : public Wt::WResource
{
    public:
        ItookiAckReceiver(Echoes::Dbo::Session& session);
        virtual ~ItookiAckReceiver();

    protected:
        Echoes::Dbo::Session& m_session;
        
        std::string m_errorCode;
        std::string m_refSent;
        
        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
};

#endif	/* ACKRECEIVER_H */

