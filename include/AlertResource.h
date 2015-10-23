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
        EReturnCode getAlertsList(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = map<string, long long>());
        EReturnCode getAlert(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = map<string, long long>());
        EReturnCode getAlertEvents(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = map<string, long long>());
        EReturnCode getAlertEvent(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = map<string, long long>());
        EReturnCode getAlertStatus(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = map<string, long long>());
        EReturnCode getAlertMessages(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = map<string, long long>());
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);
        
        std::string AlertStatusToString(int AlertStatusId);
        
        EReturnCode postAlert(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = map<string, long long>());
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);

        EReturnCode startAlert(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = map<string, long long>());
        EReturnCode assignAlert(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = map<string, long long>());
        EReturnCode resolveAlert(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = map<string, long long>());
        EReturnCode forwardAlert(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = map<string, long long>());
        virtual EReturnCode processPutRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);

        EReturnCode deleteAlert(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = map<string, long long>());
        virtual EReturnCode processDeleteRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);
};
#endif	/* ALERTRESOURCE_H */

