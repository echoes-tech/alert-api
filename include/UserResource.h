/* 
 * Header of API UserResource
 * @author ECHOES Technologies (GDR)
 * @date 05/02/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef USERRESOURCE_H
#define	USERRESOURCE_H

#include "PublicApiResource.h"

class UserResource : public PublicApiResource
{
    public :
        UserResource(Echoes::Dbo::Session& session);
        virtual ~UserResource();
        
    protected :
        EReturnCode getUsersList(const long long &orgId, std::string &response);
        EReturnCode getUser(const std::vector<std::string> &pathElements, const long long &orgId, std::string &response);
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode postActionForUser(const std::string &sRequest,  const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode putUser(const std::vector<std::string> &pathElements, const std::string &sRequest, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processPutRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);
};

#endif	/* USERRESOURCE_H */

