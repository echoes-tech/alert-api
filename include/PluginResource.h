/* 
 * Header of API PluginResource
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

#ifndef PLUGINRESOURCE_H
#define	PLUGINRESOURCE_H

#include "PublicApiResource.h"

class PluginResource : public PublicApiResource
{
    public :
        PluginResource();
        virtual ~PluginResource();
        
    protected :
        std::string m_role;
        std::string m_media_type;
        
        EReturnCode getPlugin(std::string &responseMsg);
//        EReturnCode getInformationForSeaSrcAndPlg(std::string &responseMsg);
        EReturnCode getInformationListForPlugin(std::string &responseMsg);
        EReturnCode getAliasForPlugin(std::string &responseMsg);
        virtual void processGetRequest(Wt::Http::Response &response);

//        EReturnCode postInformationForSeaSrcAndPlg(std::string &responseMsg, const std::string &sRequest);
        EReturnCode postPlugin(std::string &responseMsg, const std::string &sRequest);
        virtual void processPostRequest(Wt::Http::Response &response);

        virtual void processPutRequest(Wt::Http::Response &response);
        EReturnCode putAliasForPlugin(std::string &responseMsg, const std::string &sRequest);

//        EReturnCode deleteInformationForSeaSrcAndPlg(std::string &responseMsg);
        EReturnCode deletePlugin(std::string &responseMsg);
        virtual void processDeleteRequest(Wt::Http::Response &response);

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
       
};

#endif	/* PLUGINRESOURCE_H */

