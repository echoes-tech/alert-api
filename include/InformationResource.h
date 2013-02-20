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

#include "includeFile.h"

class InformationResource : public PublicApiResource
{
    public :
        InformationResource();
        virtual ~InformationResource();
        
    protected :
        
        int unitId;
        Wt::WString alertOption;
        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
            
};


#endif	/* INFORMATIONRESOURCE_H */
