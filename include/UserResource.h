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
        UserResource();
        virtual ~UserResource();
        
    protected :
        EReturnCode getUsersList(std::string &response);
        EReturnCode getUser(std::string &response);
        virtual void processGetRequest(Wt::Http::Response &response);

        EReturnCode postActionForUser(std::string &responseMsg, const std::string &sRequest);
        virtual void processPostRequest(Wt::Http::Response &response);

        EReturnCode putUser(std::string &responseMsg, const std::string &sRequest);
        virtual void processPutRequest(Wt::Http::Response &response);
};

#endif	/* USERRESOURCE_H */

