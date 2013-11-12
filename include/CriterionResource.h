/* 
 * HEADER OF API CriterionResource
 * @author ECHOES Technologies (GDR)
 * @date 21/02/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef CRITERIONRESOURCE_H
#define	CRITERIONRESOURCE_H

#include "PublicApiResource.h"

class CriterionResource : public PublicApiResource
{
    public :
        CriterionResource();
        virtual ~CriterionResource();
        
    protected :

        unsigned short getCriteria(std::string &responseMsg);
        virtual void processGetRequest(Wt::Http::Response &response);

        virtual void processPostRequest(Wt::Http::Response &response);

        virtual void processPutRequest(Wt::Http::Response &response);
        
        virtual void processPatchRequest(Wt::Http::Response &response);

        virtual void processDeleteRequest(Wt::Http::Response &response);

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);          
};

#endif	/* CRITERIONRESOURCE_H */

