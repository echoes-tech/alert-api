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

#include "includeFile.h"

class UserResource : public PublicApiResource
{
    public :
        UserResource();
        virtual ~UserResource();
        
    protected :
        int userId, medId;
        Wt::WString aleName;
        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);            
};

#endif	/* USERRESOURCE_H */

