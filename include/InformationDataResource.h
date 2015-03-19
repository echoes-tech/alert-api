/* 
 * Header of API InformationDataResource
 * @author ECHOES Technologies (TSA)
 * @date 24/01/2014
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef INFORMATIONDATARESOURCE_H
#define	INFORMATIONDATARESOURCE_H

#include "PublicApiResource.h"

class InformationDataResource : public PublicApiResource
{
    public :
        InformationDataResource(Echoes::Dbo::Session& session);
        virtual ~InformationDataResource();

        /**
         * Select a Media
         * @param medId Identifier of Media
         * @param orgId Identifier of Organization
         * @return Media Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::InformationData> selectInformationData(const long long &medId, const long long &orgId, Echoes::Dbo::Session &session);
        /**
         * Select an Media with a string of identifier of Media
         * @param medId String of Identifier of Media
         * @param orgId Identifier of Organization
         * @return Media Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::InformationData> selectInformationData(const std::string &medId, const long long &orgId, Echoes::Dbo::Session &session);
        /**
         * Select a Media with a ID string
         * @param medId String of Identifier of Media
         * @param orgId String of Identifier of Organization
         * @return Media Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::InformationData> selectInformationData(const std::string &medId, const std::string &orgId, Echoes::Dbo::Session &session);

    protected :       
        EReturnCode getInformationData(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        EReturnCode getInformationDataList(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode postInformationData(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode putInformationData(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        virtual EReturnCode processPutRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode deleteInformationData(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        virtual EReturnCode processDeleteRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);
};

#endif	/* INFORMATIONDATARESOURCE_H */

