/* 
 * HEADER OF API AlertResource
 * @author ECHOES Technologies (GDR)
 * @date 05/02/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef ALERTRESOURCE_H
#define	ALERTRESOURCE_H

#include "PublicApiResource.h"

#include <Wt/Mail/Client>
#include <Wt/Mail/Message>

#include <tools/Enums.h>

#include "itooki/ItookiSMSSender.h"

class AlertResource : public PublicApiResource
{
    public :
        AlertResource();
        AlertResource(const AlertResource& orig);
        virtual ~AlertResource();
        
    protected:
        unsigned short getRecipientsForAlert(std::string &responseMsg) const;       
        unsigned short getTrackingAlertList(std::string &responseMsg) const;
        unsigned short getAlerts(std::string &responseMsg) const;
        virtual void processGetRequest(Wt::Http::Response &response);

        /**
         * method to send a MAIL 
         * @param collection of informations values that matches the alert
         * @param the alert
         * @param the alert tracking required and concerned by the sms
         * @param the media value concern by the alert
         * @param if the user as use all his sms, the value here is 1 if not it's 0
         * @return error -1 or sucess 0
         */
        int sendMAIL
        (
            Wt::Dbo::collection<Wt::Dbo::ptr<InformationValue>> ivaPtrCollection,
            Wt::Dbo::ptr<Alert> alertPtr,
            Wt::Dbo::ptr<AlertTracking> alertTrackingPtr,
            Wt::Dbo::ptr<AlertMediaSpecialization> amsPtr,
            bool overSMSQuota = false
        );

        /**
        * method to send an SMS with the call of the API
        * @param collection of informations values that matches the alert
        * @param the alert
        * @param the alert tracking required and concerned by the sms
        * @param the media value concern by the alert
        * @return error -1 or sucess 0
        */     
        int sendSMS
        (
            Wt::Dbo::collection<Wt::Dbo::ptr<InformationValue>> ivaPtrCollection,
            Wt::Dbo::ptr<Alert> alertPtr,
            Wt::Dbo::ptr<AlertTracking> alertTrackingPtr,
            Wt::Dbo::ptr<AlertMediaSpecialization> amsPtr
        );

        unsigned short postAlert(std::string &responseMsg, const std::string &sRequest);
        unsigned short postAlertTracking(std::string &responseMsg, const std::string &sRequest);
        virtual void processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        

        virtual void processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        unsigned short deleteAlert(std::string &responseMsg);
        virtual void processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
};
#endif	/* ALERTRESOURCE_H */

