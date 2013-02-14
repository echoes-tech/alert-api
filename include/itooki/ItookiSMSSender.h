/* 
 * Header of Itooki SMS Sender
 * @author ECHOES Technologies (FPO)
 * @date 15/11/2012
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2012 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef ITOOKISMSSENDER_H
#define	ITOOKISMSSENDER_H

#include "PublicApiResource.h"

class ItookiSMSSender : public PublicApiResource
{
    public:
        ItookiSMSSender();
        ItookiSMSSender(const ItookiSMSSender& orig);
        virtual ~ItookiSMSSender();

    protected:
        std::string alertId, alertTrackingId, number, messageText;
    
        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        void handle(boost::system::error_code err, const Wt::Http::Message& response);
};

#endif	/* ITOOKISMSSENDER_H */

