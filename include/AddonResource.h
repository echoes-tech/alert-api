/* 
 * HEADER OF API AddonResource
 * @author ECHOES Technologies (GDR)
 * @date 06/03/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef ADDONRESOURCE_H
#define	ADDONRESOURCE_H

#include "PublicApiResource.h"

class AddonResource : public PublicApiResource
{
    public :
        AddonResource();
        virtual ~AddonResource();
        
    protected :
        
        unsigned short getSearchTypeForAddon(std::string &responseMsg);    
        unsigned short getParameterForAddon(std::string &responseMsg);    
        unsigned short getAddonList(std::string &responseMsg);       
        virtual void processGetRequest(Wt::Http::Response &response);

        unsigned short postAddon(std::string &responseMsg, const std::string &sRequest);
        virtual void processPostRequest(Wt::Http::Response &response);

        virtual void processPutRequest(Wt::Http::Response &response);
        
        virtual void processPatchRequest(Wt::Http::Response &response);

        unsigned short deleteAddon(std::string &responseMsg);
        virtual void processDeleteRequest(Wt::Http::Response &response);

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
            
};

#endif	/* ADDONRESOURCE_H */

