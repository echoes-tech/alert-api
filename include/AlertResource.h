/**
 * @file AlertResource.h
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
         * @param orgId Identifier of Organization
         * @return Alert Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Alert> selectAlert(const long long &aleId, const long long &orgId, Echoes::Dbo::Session &session);
        /**
         * Select an Alert with a string of identifier of alert
         * @param aleId String of Identifier of Alert
         * @param orgId Identifier of Organization
         * @return Alert Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Alert> selectAlert(const std::string &aleId, const long long &orgId, Echoes::Dbo::Session &session);
        /**
         * Select an Alert with string IDs
         * @param aleId String of Identifier of Alert
         * @param orgId String of Identifier of Organization
         * @return Alert Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Alert> selectAlert(const std::string &aleId, const std::string &orgId, Echoes::Dbo::Session &session);

    protected:
//        EReturnCode getRecipientsForAlert(const long long &orgId, std::string &responseMsg);       
        EReturnCode getTrackingsForAlertsList(std::map<std::string, long long> &parameters, const long long &orgId, std::string &responseMsg);
//        EReturnCode getTrackingAlertList(const long long &orgId, std::string &responseMsg);
        EReturnCode getAlertsList(std::map<std::string, long long> &parameters, const long long &orgId, std::string &responseMsg);
        EReturnCode getAlert(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

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
            std::vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrVector,
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
            std::vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrVector,
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
         std::vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue >> ivaPtrVector,
         Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr,
         Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> atrPtr,
         Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr
        );
        
        EReturnCode postAlert(const std::string &sRequest, const long long &orgId, std::string &responseMsg);
        EReturnCode postAlertTracking(map<string, long long> parameters, const std::vector<std::string> &pathElements, const std::string &sRequest, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

//        virtual EReturnCode processPutRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode deleteAlert(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processDeleteRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg); 
};
#endif	/* ALERTRESOURCE_H */
