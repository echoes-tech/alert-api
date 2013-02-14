/* 
 * HEADER OF API PluginRessource
 * @author ECHOES Technologies (GDR)
 * @date 10/02/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */


#ifndef PLUGINRESSOURCE_H
#define	PLUGINRESSOURCE_H

#include "includeFile.h"

class PluginRessource : public PublicApiResource
{
    public :
        PluginRessource();
        virtual ~PluginRessource();
        
    protected :
        
        int plgId;
        Wt::WString alertOption;
        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
            
};

#endif	/* PLUGINRESSOURCE_H */

