/* 
 * Header of API RoleResource
 * @author ECHOES Technologies (TSA)
 * @date 08/09/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef ROLERESOURCE_H
#define	ROLERESOURCE_H

#include "PublicApiResource.h"

class RoleResource : public PublicApiResource
{
    public :
        RoleResource();
        virtual ~RoleResource();
        
    protected :
        EReturnCode getRolesList(std::string &response);
        EReturnCode getRole(std::string &response);
        virtual void processGetRequest(Wt::Http::Response &response);

        EReturnCode postRole(std::string &responseMsg, const std::string &sRequest);
        virtual void processPostRequest(Wt::Http::Response &response);

        EReturnCode putRole(std::string &responseMsg, const std::string &sRequest);
        virtual void processPutRequest(Wt::Http::Response &response);

        virtual void processDeleteRequest(Wt::Http::Response &response);
};

#endif	/* ROLERESOURCE_H */

