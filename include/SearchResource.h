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
        SearchResource();
        virtual ~SearchResource();

        /**
         * Select a Search
         * @param srcId Identifier of Search
         * @return Search Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Search> selectSearch(const long long &seaId, Echoes::Dbo::Session &session);
        /**
         * Select a Search with a ID string
         * @param srcId String of Identifier of Search
         * @return Search Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Search> selectSearch(const std::string &seaId, Echoes::Dbo::Session &session);

    private:
        EReturnCode getSearchsList(std::string &responseMsg);
        EReturnCode getSearch(std::string &responseMsg);
        virtual void processGetRequest(Wt::Http::Response &response);

        EReturnCode postSearch(std::string &responseMsg, const std::string &sRequest);
        virtual void processPostRequest(Wt::Http::Response &response);

        EReturnCode putSearch(std::string &responseMsg, const std::string &sRequest);
        virtual void processPutRequest(Wt::Http::Response &response);

        EReturnCode deleteSearch(std::string &responseMsg);
        virtual void processDeleteRequest(Wt::Http::Response &response);
};

#endif	/* SEARCHRESOURCE_H */

