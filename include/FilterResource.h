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
        FilterResource();
        virtual ~FilterResource();

        /**
         * Select a Filter
         * @param filId Identifier of Filter
         * @return Filter Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Filter> selectFilter(const long long &filId, Echoes::Dbo::Session &session);
        /**
         * Select a Filter with a ID string
         * @param filId String of Identifier of Filter
         * @return Filter Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Filter> selectFilter(const std::string &filId, Echoes::Dbo::Session &session);

    private:
        EReturnCode getFiltersList(std::string &responseMsg);
        EReturnCode getFilter(std::string &responseMsg);
        EReturnCode getParametersList(std::string &responseMsg);
        EReturnCode getParametersListForFilter(std::string &responseMsg);
        virtual void processGetRequest(Wt::Http::Response &response);

        EReturnCode postFilter(std::string &responseMsg, const std::string &sRequest);
        virtual void processPostRequest(Wt::Http::Response &response);

        EReturnCode putFilter(std::string &responseMsg, const std::string &sRequest);
        virtual void processPutRequest(Wt::Http::Response &response);

        EReturnCode deleteFilter(std::string &responseMsg);
        virtual void processDeleteRequest(Wt::Http::Response &response);
};

#endif	/* FILTERRESOURCE_H */

