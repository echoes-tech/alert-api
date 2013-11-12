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

#include "PublicApiResource.h"

class OrganizationResource : public PublicApiResource
{
    public :
        OrganizationResource();
        virtual ~OrganizationResource();
        
    protected :
        
        unsigned short getOrganization(std::string &responseMsg);
        unsigned short getUsersForOrganization(std::string &responseMsg);
        unsigned short getRolesForOrganization(std::string &responseMsg);
        unsigned short getMediasForUserForOrganization(std::string &responseMsg);
        unsigned short getMediasValuesForUserForOrganization(std::string &responseMsg);
        unsigned short getQuotasSms(std::string &responseMsg);
        unsigned short getQuotasAsset(std::string &responseMsg);
        virtual void processGetRequest(Wt::Http::Response &response);

        virtual void processPostRequest(Wt::Http::Response &response);

        virtual void processPutRequest(Wt::Http::Response &response);
        
        virtual void processPatchRequest(Wt::Http::Response &response);

        virtual void processDeleteRequest(Wt::Http::Response &response);

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);          
};

#endif	/* ORGANIZATIONRESOURCE_H */

