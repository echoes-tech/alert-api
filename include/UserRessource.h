/* 
 * HEADER OF API UserRessource
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

#ifndef USERRESSOURCE_H
#define	USERRESSOURCE_H

#include "includeFile.h"

class UserRessource : public PublicApiResource
{
    public :
        UserRessource();
        virtual ~UserRessource();
        
    protected :
        int userId, medId;
        Wt::WString aleName;
        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);            
};

#endif	/* USERRESSOURCE_H */

