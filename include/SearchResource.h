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
         * @param grpId Identifier of Group
         * @return Search Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Search> selectSearch(const long long &seaId, const long long &grpId, Echoes::Dbo::Session &session);
        /**
         * Select an Search with a string of identifier of Search
         * @param seaId String of Identifier of Search
         * @param grpId Identifier of Group
         * @return Search Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Search> selectSearch(const std::string &seaId, const long long &grpId, Echoes::Dbo::Session &session);
        /**
         * Select a Search with a ID string
         * @param seaId String of Identifier of Search
         * @param grpId String of Identifier of Group
         * @return Search Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Search> selectSearch(const std::string &seaId, const std::string &grpId, Echoes::Dbo::Session &session);

    private:
        EReturnCode getSearchsList(std::map<std::string, long long> &parameters, const long long &grpId, std::string &responseMsg);
        EReturnCode getSearch(const std::vector<std::string> &pathElements, const long long &grpId, std::string &responseMsg);
        EReturnCode getParametersList(std::map<std::string, long long> &parameters, const long long &grpId, std::string &responseMsg);
        EReturnCode getParametersListForSearch(const std::vector<std::string> &pathElements, const long long &grpId, std::string &responseMsg);
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);

        EReturnCode postSearch(const std::string &sRequest,  const long long &grpId, std::string &responseMsg);
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);

        EReturnCode putSearch(const std::vector<std::string> &pathElements, const std::string &sRequest,  const long long &grpId, std::string &responseMsg);
        virtual EReturnCode processPutRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);

        EReturnCode deleteSearch(const std::vector<std::string> &pathElements, const long long &grpId, std::string &responseMsg);
        virtual EReturnCode processDeleteRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);
};

#endif	/* SEARCHRESOURCE_H */

