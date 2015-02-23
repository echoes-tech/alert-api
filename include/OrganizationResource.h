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
        OrganizationResource(Echoes::Dbo::Session& session);
        virtual ~OrganizationResource();
        
    protected :
        EReturnCode Error       (const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        
        EReturnCode getOrganizationsList(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        EReturnCode getOrganization(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);
};

#endif	/* ORGANIZATIONRESOURCE_H */

