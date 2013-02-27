/* 
 * HEADER OF API OrganizationResource
 * @author ECHOES Technologies (GDR)
 * @date 18/02/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef ORGANIZATIONRESOURCE_H
#define	ORGANIZATIONRESOURCE_H

#include "includeFile.h"

class OrganizationResource : public PublicApiResource
{
    public :
        OrganizationResource();
        virtual ~OrganizationResource();
        
    protected :
        
        std::string getOrganization();
        std::string getUsersForOrganization();
        std::string getQuotasSms();
        std::string getQuotasAsset();
        virtual void processGetRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        
        virtual void processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);          
};

#endif	/* ORGANIZATIONRESOURCE_H */

