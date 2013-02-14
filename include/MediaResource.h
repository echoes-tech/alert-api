/* 
 * HEADER OF API PluginResource
 * @author ECHOES Technologies (GDR)
 * @date 11/02/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef MEDIARESOURCE_H
#define	MEDIARESOURCE_H

#include "includeFile.h"

class MediaResource : public PublicApiResource
{
    public :
        MediaResource();
        virtual ~MediaResource();
        
    protected :
        
        Wt::WString alertOption;
        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
            
};

#endif	/* MEDIARESOURCE_H */

