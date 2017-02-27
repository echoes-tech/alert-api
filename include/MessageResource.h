/**
 * @file MessageResource.h
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

#ifndef MESSAGERESOURCE_H
#define	MESSAGERESOURCE_H

#include <Wt/Mail/Client>
#include <Wt/Mail/Message>

#include "Conf.h"
#include "PublicApiResource.h"
#include "MediaResource.h"
#include "itooki/ItookiSMSSender.h"

class MessageResource : public PublicApiResource
{
    public :
        MessageResource(Echoes::Dbo::Session& session);
        virtual ~MessageResource();
        
        /**
         * Select a Message
         * @param prbId Identifier of Message
         * @param grpId Identifier of Group
         * @return Probe Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Message> selectMessage(const long long &msgId, const long long &grpId, Echoes::Dbo::Session &session);
        /**
         * Select an Message with a string of identifier of Message
         * @param prbId String of Identifier of Message
         * @param grpId Identifier of Group
         * @return Probe Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Message> selectMessage(const std::string &msgId, const long long &grpId, Echoes::Dbo::Session &session);
        /**
         * Select a Message with a ID string
         * @param prbId String of Identifier of Message
         * @param grpId Identifier of Group
         * @return Probe Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Message> selectMessage(const std::string &msgId, const std::string &grpId, Echoes::Dbo::Session &session);
        
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
        EReturnCode Error       (const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        
        Wt::WString generateToken();
        EReturnCode getMessageEvents(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        EReturnCode getMessageEvent(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        EReturnCode getMessageStatus(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        
        EReturnCode getMessages(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        EReturnCode getMessage(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        EReturnCode postSimpleMessage(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        EReturnCode postAlertMessage(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);

        void replaceVariablesInMessage(vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrVector, Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr, std::string &message);

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
            Wt::Dbo::ptr<Echoes::Dbo::Message> msgPtr,
            bool overSMSQuota = false,
            std::vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrVector = std::vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>>(),
            Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr = Wt::Dbo::ptr<Echoes::Dbo::Alert>(),
            Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr = Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization>()
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
            Wt::Dbo::ptr<Echoes::Dbo::Message> msgPtr,
            std::vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrVector = std::vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>>(),
            Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr = Wt::Dbo::ptr<Echoes::Dbo::Alert>(),
            Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr = Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization>()
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
            Wt::Dbo::ptr<Echoes::Dbo::Message> msgPtr,
            std::vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrVector = std::vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>>(),
            Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr = Wt::Dbo::ptr<Echoes::Dbo::Alert>(),
            Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr = Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization>()
        );


        
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);
   private:
        ItookiSMSSender* m_itookiSMSSender;
};
#endif	/* MESSAGERESOURCE_H */
