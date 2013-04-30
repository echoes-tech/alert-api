/* 
 * HEADER OF API PluginResource
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
        PluginResource(const PluginResource& orig);
        virtual ~PluginResource();
        
    protected :
        
        unsigned short pluginIsAccessible(std::string& responseMsg) const;
        
        unsigned short getPluginJSON(std::string &responseMsg) const;
        unsigned short getPlugin(std::string &responseMsg) const;
        unsigned short getParameterValueForSearch(std::string &responseMsg) const;
        unsigned short getParameterValueForSource(std::string &responseMsg) const;
        unsigned short getInformationForSeaSrcAndPlg(std::string &responseMsg) const;
        unsigned short getSearchForSourceAndPlugin(std::string &responseMsg) const;
        unsigned short getSourceForPlugin(std::string &responseMsg) const;
        unsigned short getInformationListForPlugin(std::string &responseMsg) const;
        unsigned short getAliasForInformation(std::string &responseMsg) const;
        virtual void processGetRequest(Wt::Http::Response &response);

        unsigned short postInformationForSeaSrcAndPlg(std::string &responseMsg, const std::string &sRequest);
        unsigned short postSearchForSourceAndPlugin(std::string &responseMsg, const std::string &sRequest);
        unsigned short postSourceForPlugin(std::string &responseMsg, const std::string &sRequest);
        unsigned short postPlugin(std::string &responseMsg, const std::string &sRequest);
        virtual void processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        unsigned short putAliasForInformation(std::string &responseMsg, const std::string &sRequest);
        
        
        unsigned short patchInformationForSeaSrcAndPlg(std::string &responseMsg, const std::string &sRequest);
        unsigned short patchSearchForSourceAndPlugin(std::string &responseMsg, const std::string &sRequest);
        unsigned short patchParametersSourceForPlugin(std::string &responseMsg, const std::string &sRequest);
        virtual void processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        unsigned short deleteInformationForSeaSrcAndPlg(std::string &responseMsg);
        unsigned short deleteSearchForSourceAndPlugin(std::string &responseMsg);
        unsigned short deleteSourceForPlugin(std::string &responseMsg);
        unsigned short deletePlugin(std::string &responseMsg);
        virtual void processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        
        std::string role;
        std::string media;
        
};

#endif	/* PLUGINRESOURCE_H */

