/* 
 * HEADER OF API InformationResource
 * @author ECHOES Technologies (GDR)
 * @date 13/02/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef INFORMATIONRESOURCE_H
#define	INFORMATIONRESOURCE_H

#include "PublicApiResource.h"

class InformationResource : public PublicApiResource
{
    public :
        InformationResource();
        virtual ~InformationResource();
        
    protected :
        
        virtual void processGetRequest(Wt::Http::Response &response);

        virtual void processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        
        virtual void processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
            
};


#endif	/* INFORMATIONRESOURCE_H */

