/* 
 * Header of API AlertResource
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
#include "MediaResource.h"
#include "itooki/ItookiSMSSender.h"

class AlertResource : public PublicApiResource
{
    public :
        AlertResource(Echoes::Dbo::Session& session);
        virtual ~AlertResource();

        /**
         * Select an Alert
         * @param aleId Identifier of Alert
         * @param grpId Identifier of Group
         * @return Alert Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Alert> selectAlert(const long long &aleId, const long long &grpId, Echoes::Dbo::Session &session);
        /**
         * Select an Alert with a string of identifier of alert
         * @param aleId String of Identifier of Alert
         * @param grpId Identifier of Group
         * @return Alert Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Alert> selectAlert(const std::string &aleId, const long long &grpId, Echoes::Dbo::Session &session);
        /**
         * Select an Alert with string IDs
         * @param aleId String of Identifier of Alert
         * @param grpId String of Identifier of Group
         * @return Alert Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Alert> selectAlert(const std::string &aleId, const std::string &grpId, Echoes::Dbo::Session &session);

    protected:
//        EReturnCode getRecipientsForAlert(const long long &grpId, std::string &responseMsg);       
//        EReturnCode getTrackingsForAlertsList(std::map<std::string, long long> &parameters, const long long &grpId, std::string &responseMsg);
//        EReturnCode getTrackingAlertList(const long long &grpId, std::string &responseMsg);
        EReturnCode getAlertsList(std::map<std::string, long long> &parameters, const long long &grpId, std::string &responseMsg);
        EReturnCode getAlert(const std::vector<std::string> &pathElements, const long long &grpId, std::string &responseMsg);
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);
        
//        void replaceVariablesInMessage(vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrVector, Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr, std::string &message);
//
//        /**
//         * method to send a MAIL 
//         * @param collection of informations values that matches the alert
//         * @param the alert
//         * @param the alert tracking required and concerned by the sms
//         * @param the media value concern by the alert
//         * @param if the user as use all his sms, the value here is 1 if not it's 0
//         * @return HTTP Code
//         */
//        EReturnCode sendMAIL
//        (
//            std::vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrVector,
//            Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr,
//            Wt::Dbo::ptr<Echoes::Dbo::Message> atrPtr,
//            Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr,
//            bool overSMSQuota = false
//        );
//
//        /**
//         * method to send an SMS with the call of the API
//         * @param collection of informations values that matches the alert
//         * @param the alert
//         * @param the alert tracking required and concerned by the sms
//         * @param the media value concern by the alert
//         * @return HTTP Code
//         */
//        EReturnCode sendSMS
//        (
//            std::vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrVector,
//            Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr,
//            Wt::Dbo::ptr<Echoes::Dbo::Message> atrPtr,
//            Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr
//        );
//
//        /**
//         * method to put a date and content in Alertetracking table 
//         * @param collection of informations values that matches the alert
//         * @param the alert
//         * @param the alert tracking required and concerned by the sms
//         * @param the media value concern by the alert
//         * @return HTTP Code
//         */
//        EReturnCode sendMobileApp
//        (
//         std::vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue >> ivaPtrVector,
//         Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr,
//         Wt::Dbo::ptr<Echoes::Dbo::Message> atrPtr,
//         Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr
//        );
        
        EReturnCode postAlert(const std::string &sRequest, const long long &grpId, std::string &responseMsg);
//        EReturnCode postAlertTracking(map<string, long long> parameters, const std::vector<std::string> &pathElements, const std::string &sRequest, const long long &grpId, std::string &responseMsg);
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);

//        virtual EReturnCode processPutRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);

        EReturnCode deleteAlert(const std::vector<std::string> &pathElements, const long long &grpId, std::string &responseMsg);
        virtual EReturnCode processDeleteRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg); 
};
#endif	/* ALERTRESOURCE_H */

