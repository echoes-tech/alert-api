/* 
 * Header of API SearchResource
 * @author ECHOES Technologies (FPO)
 * @date 09/12/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef SEARCHRESOURCE_H
#define	SEARCHRESOURCE_H

#include "PublicApiResource.h"
#include "SourceResource.h"

class SearchResource : public PublicApiResource
{
    public:
        SearchResource(Echoes::Dbo::Session& session);
        virtual ~SearchResource();

        /**
         * Select a Search
         * @param seaId Identifier of Search
         * @param orgId Identifier of Organization
         * @return Search Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Search> selectSearch(const long long &seaId, const long long &orgId, Echoes::Dbo::Session &session);
        /**
         * Select an Search with a string of identifier of Search
         * @param seaId String of Identifier of Search
         * @param orgId Identifier of Organization
         * @return Search Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Search> selectSearch(const std::string &seaId, const long long &orgId, Echoes::Dbo::Session &session);
        /**
         * Select a Search with a ID string
         * @param seaId String of Identifier of Search
         * @param orgId String of Identifier of Organization
         * @return Search Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Search> selectSearch(const std::string &seaId, const std::string &orgId, Echoes::Dbo::Session &session);

    private:
        EReturnCode getSearchsList(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        EReturnCode getSearch(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        EReturnCode getParametersList(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        EReturnCode getParametersListForSearch(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode postSearch(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode putSearch(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        virtual EReturnCode processPutRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode deleteSearch(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        virtual EReturnCode processDeleteRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);
};

#endif	/* SEARCHRESOURCE_H */

