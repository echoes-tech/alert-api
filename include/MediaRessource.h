/* 
 * HEADER OF API PluginRessource
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

#ifndef MEDIARESSOURCE_H
#define	MEDIARESSOURCE_H

#include "includeFile.h"

class MediaRessource : public PublicApiResource
{
    public :
        MediaRessource();
        virtual ~MediaRessource();
        
    protected :
        
        Wt::WString alertOption;
        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
            
};

#endif	/* MEDIARESSOURCE_H */

