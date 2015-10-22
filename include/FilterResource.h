/* 
 * Header of API FilterResource
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

#ifndef FILTERRESOURCE_H
#define	FILTERRESOURCE_H

#include "PublicApiResource.h"
#include "SearchResource.h"

class FilterResource : public PublicApiResource
{
    public:
        FilterResource(Echoes::Dbo::Session& session);
        virtual ~FilterResource();

        /**
         * Select a Filter
         * @param filId Identifier of Filter
         * @param grpId Identifier of Group
         * @return Filter Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Filter> selectFilter(const long long &filId, const long long &grpId, Echoes::Dbo::Session &session);
        /**
         * Select an Filter with a string of identifier of Filter
         * @param filId String of Identifier of Filter
         * @param grpId Identifier of Group
         * @return Filter Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Filter> selectFilter(const std::string &filId, const long long &grpId, Echoes::Dbo::Session &session);
        /**
         * Select a Filter with a ID string
         * @param filId String of Identifier of Filter
         * @param grpId String of Identifier of Group
         * @return Filter Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Filter> selectFilter(const std::string &filId, const std::string &grpId, Echoes::Dbo::Session &session);

    private:
        EReturnCode getFiltersList(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        EReturnCode getFilter(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        EReturnCode getParametersListForFilters(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        EReturnCode getParametersForFilter(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);
       
        EReturnCode postFilters(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);

        EReturnCode putFilters(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        virtual EReturnCode processPutRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);

        EReturnCode deleteFilters(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        virtual EReturnCode processDeleteRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);
};

#endif	/* FILTERRESOURCE_H */

