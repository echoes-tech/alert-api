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

#include <Wt/Mail/Client>
#include <Wt/Mail/Message>

#include "Conf.h"
#include "PublicApiResource.h"
#include "itooki/ItookiSMSSender.h"

class AlertResource : public PublicApiResource
{
    public :
        AlertResource();
        virtual ~AlertResource();
        
    protected:
        std::string m_media;
        
        EReturnCode getRecipientsForAlert(std::string &responseMsg);       
        EReturnCode getTrackingAlertMessage(std::string &responseMsg);
        EReturnCode getTrackingAlertList(std::string &responseMsg);
        EReturnCode getAlerts(std::string &responseMsg);
        EReturnCode getAlert(std::string &responseMsg);
        virtual void processGetRequest(Wt::Http::Response &response);

        /**
         * method to send a MAIL 
         * @param collection of informations values that matches the alert
         * @param the alert
         * @param the alert tracking required and concerned by the sms
         * @param the media value concern by the alert
         * @param if the user as use all his sms, the value here is 1 if not it's 0
         * @return HTTP Code
         */
        EReturnCode sendMAIL
        (
            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrCollection,
            Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr,
            Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> atrPtr,
            Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr,
            bool overSMSQuota = false
        );

        /**
         * method to send an SMS with the call of the API
         * @param collection of informations values that matches the alert
         * @param the alert
         * @param the alert tracking required and concerned by the sms
         * @param the media value concern by the alert
         * @return HTTP Code
         */
        EReturnCode sendSMS
        (
            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrCollection,
            Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr,
            Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> atrPtr,
            Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr
        );

        /**
         * method to put a date and content in Alertetracking table 
         * @param collection of informations values that matches the alert
         * @param the alert
         * @param the alert tracking required and concerned by the sms
         * @param the media value concern by the alert
         * @return HTTP Code
         */
        EReturnCode sendMobileApp
        (
         Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrCollection,
         Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr,
         Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> atrPtr,
         Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr
        );
        
        EReturnCode postAlert(std::string &responseMsg, const std::string &sRequest);
        EReturnCode postAlertTracking(std::string &responseMsg, const std::string &sRequest);
        virtual void processPostRequest(Wt::Http::Response &response);

        virtual void processPutRequest(Wt::Http::Response &response);
        

        virtual void processPatchRequest(Wt::Http::Response &response);

        EReturnCode deleteAlert(std::string &responseMsg);
        virtual void processDeleteRequest(Wt::Http::Response &response);

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        
        
};
#endif	/* ALERTRESOURCE_H */

