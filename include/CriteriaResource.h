/* 
 * HEADER OF API CriteriaResource
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

#ifndef CRITERIARESOURCE_H
#define	CRITERIARESOURCE_H

#include "PublicApiResource.h"

class CriteriaResource : public PublicApiResource
{
    public :
        CriteriaResource();
        CriteriaResource(const CriteriaResource& orig);
        virtual ~CriteriaResource();
        
    protected :

        unsigned short getCriterias(std::string &responseMsg) const;
        virtual void processGetRequest(Wt::Http::Response &response);

        virtual void processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        
        virtual void processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);          
};

#endif	/* CRITERIARESOURCE_H */

