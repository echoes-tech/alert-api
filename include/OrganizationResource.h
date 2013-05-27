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
        OrganizationResource(const OrganizationResource& orig);
        virtual ~OrganizationResource();
        
    protected :
        
        unsigned short getOrganization(std::string &responseMsg) const;
        unsigned short getUsersForOrganization(std::string &responseMsg) const;
        unsigned short getRolesForOrganization(std::string &responseMsg) const;
        unsigned short getMediasForUserForOrganization(std::string &responseMsg) const;
        unsigned short getMediasValuesForUserForOrganization(std::string &responseMsg) const;
        unsigned short getQuotasSms(std::string &responseMsg) const;
        unsigned short getQuotasAsset(std::string &responseMsg) const;
        virtual void processGetRequest(Wt::Http::Response &response);

        virtual void processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        
        virtual void processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);          
};

#endif	/* ORGANIZATIONRESOURCE_H */

