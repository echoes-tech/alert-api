/* 
 * HEADER OF API UserResource
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

        unsigned short getInformationForUser(std::string &response);
        virtual void processGetRequest(Wt::Http::Response &response);

        unsigned short postActionForUser(std::string &responseMsg, const std::string &sRequest);
        unsigned short postRoleForUser(std::string &responseMsg, const std::string &sRequest);
        
        virtual void processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        
        virtual void processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);          
};

#endif	/* USERRESOURCE_H */

