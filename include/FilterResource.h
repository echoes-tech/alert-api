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
         * @param orgId Identifier of Organization
         * @return Filter Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Filter> selectFilter(const long long &filId, const long long &orgId, Echoes::Dbo::Session &session);
        /**
         * Select an Filter with a string of identifier of Filter
         * @param filId String of Identifier of Filter
         * @param orgId Identifier of Organization
         * @return Filter Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Filter> selectFilter(const std::string &filId, const long long &orgId, Echoes::Dbo::Session &session);
        /**
         * Select a Filter with a ID string
         * @param filId String of Identifier of Filter
         * @param orgId String of Identifier of Organization
         * @return Filter Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Filter> selectFilter(const std::string &filId, const std::string &orgId, Echoes::Dbo::Session &session);

    private:
        EReturnCode getFiltersList(std::map<std::string, long long> &parameters, const long long &orgId, std::string &responseMsg);
        EReturnCode getFilter(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        EReturnCode getParametersList(std::map<std::string, long long> &parameters, const long long &orgId, std::string &responseMsg);
        EReturnCode getParametersListForFilter(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode postFilter(const std::string &sRequest, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode putFilter(const std::vector<std::string> &pathElements, const std::string &sRequest, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processPutRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode deleteFilter(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processDeleteRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);
};

#endif	/* FILTERRESOURCE_H */

