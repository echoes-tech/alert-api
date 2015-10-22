/* 
 * Header of API GroupResource
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

#ifndef GROUPRESOURCE_H
#define	GROUPRESOURCE_H

#include "PublicApiResource.h"

class GroupResource : public PublicApiResource
{
    public :
        GroupResource(Echoes::Dbo::Session& session);
        virtual ~GroupResource();
        
    protected :
        EReturnCode getGroupsList(const long long &grpId, std::string &responseMsg);
        EReturnCode getGroup(const std::vector<std::string> &pathElements, const long long &grpId, std::string &responseMsg);
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);
};

#endif	/* GROUPRESOURCE_H */

