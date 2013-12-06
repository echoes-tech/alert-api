/* 
 * Header of API OrganizationResource
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

#include "PublicApiResource.h"

class OrganizationResource : public PublicApiResource
{
    public :
        OrganizationResource();
        virtual ~OrganizationResource();
        
    protected :
        EReturnCode getOrganizationsList(std::string &responseMsg);
        EReturnCode getOrganization(std::string &responseMsg);
        virtual void processGetRequest(Wt::Http::Response &response);

        virtual void processPostRequest(Wt::Http::Response &response);

        virtual void processPutRequest(Wt::Http::Response &response);

        virtual void processDeleteRequest(Wt::Http::Response &response);
};

#endif	/* ORGANIZATIONRESOURCE_H */

