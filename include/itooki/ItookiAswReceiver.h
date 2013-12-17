/* 
 * Header of Itooki SMS Answer Receiver
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

#ifndef ASWRECEIVER_H
#define	ASWRECEIVER_H

#include <Wt/WResource>

#include <tools/Session.h>

#include "Conf.h"

class ItookiAswReceiver : public Wt::WResource
{
    public:
        ItookiAswReceiver();
        virtual ~ItookiAswReceiver();

    protected:
        std::string m_number;
        std::string m_message;
        
        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
};

#endif	/* ASWRECEIVER_H */

