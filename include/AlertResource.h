/* 
 * HEADER OF API AlertResource
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

#ifndef ALERTRESOURCE_H
#define	ALERTRESOURCE_H

#include "includeFile.h"

class AlertResource : public PublicApiResource
{
    public :
        AlertResource();
        virtual ~AlertResource();
        
    protected :
        
        int alertId;
        Wt::WString alertOption;
               
        std::string getAlertList();
        virtual void processGetRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        std::string postAlert(std::string sRequest);
        virtual void processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        

        virtual void processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        std::string deleteAlert(std::string sRequest);
        virtual void processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
};
#endif	/* ALERTRESOURCE_H */

