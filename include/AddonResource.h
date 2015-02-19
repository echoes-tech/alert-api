/* 
 * Header of API AddonResource
 * @author ECHOES Technologies (GDR)
 * @date 06/03/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef ADDONRESOURCE_H
#define	ADDONRESOURCE_H

#include "PublicApiResource.h"

class AddonResource : public PublicApiResource
{
    public :
        AddonResource(Echoes::Dbo::Session& session);
        virtual ~AddonResource();

    protected :
        EReturnCode getAddonsList(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = NULL);
        EReturnCode getAddon(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = NULL);
        EReturnCode getSearchTypeForAddon(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = NULL);
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);
        EReturnCode Error       (const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = NULL);
};

#endif	/* ADDONRESOURCE_H */

