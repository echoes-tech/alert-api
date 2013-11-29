/* 
 * HEADER OF API SearchTypeResource
 * @author ECHOES Technologies (GDR)
 * @date 14/03/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef SEARCHTYPERESOURCE_H
#define	SEARCHTYPERESOURCE_H

#include "PublicApiResource.h"


class SearchTypeResource : public PublicApiResource
{
    public :
        SearchTypeResource();
        virtual ~SearchTypeResource();
        
    protected :
       
        EReturnCode getSearchTypeList(std::string &responseMsg); 
        EReturnCode getParameterForSearchType(std::string &responseMsg);
        virtual void processGetRequest(Wt::Http::Response &response);

        EReturnCode postSearchType(std::string &responseMsg, const std::string &sRequest);
        virtual void processPostRequest(Wt::Http::Response &response);

        virtual void processPutRequest(Wt::Http::Response &response);
        
        virtual void processPatchRequest(Wt::Http::Response &response);

        EReturnCode deleteSearchType(std::string &responseMsg);
        virtual void processDeleteRequest(Wt::Http::Response &response);

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
            
};


#endif	/* SEARCHTYPERESOURCE_H */

