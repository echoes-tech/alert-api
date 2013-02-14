/* 
 * HEADER OF API AlertRessource
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

#ifndef ALERTRESSOURCE_H
#define	ALERTRESSOURCE_H

#include "includeFile.h"

class AlertRessource : public PublicApiResource
{
    public :
        AlertRessource();
        virtual ~AlertRessource();
        
    protected :
        
        int alertId;
        Wt::WString alertOption;
        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        /*
               
        virtual void processGetRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        std::string postProbeForAsset(std::string sRequest);
        virtual void processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        

        virtual void processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
            */
};
#endif	/* ALERTRESSOURCE_H */

