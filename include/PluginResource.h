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
        virtual ~PluginResource();
        
    protected :
        std::string m_role;
        std::string m_media;
        
        unsigned short pluginIsAccessible(std::string& responseMsg);
        
        unsigned short getPluginJSON(std::string &responseMsg);
        unsigned short getPlugin(std::string &responseMsg);
//        unsigned short getParameterValueForSearch(std::string &responseMsg);
//        unsigned short getParameterValueForSource(std::string &responseMsg);
//        unsigned short getInformationForSeaSrcAndPlg(std::string &responseMsg);
//        unsigned short getSearchForSourceAndPlugin(std::string &responseMsg);
//        unsigned short getSearchForSearchIdForSourceAndPlugin(std::string &responseMsg);
        unsigned short getSourceForPlugin(std::string &responseMsg);
        unsigned short getInformationListForPlugin(std::string &responseMsg);
//        unsigned short getAliasForInformation(std::string &responseMsg);
//        unsigned short getAliasForCriteria(std::string &responseMsg);
//        unsigned short getCriteriaForInformation(std::string &responseMsg);
        unsigned short getAliasForPlugin(std::string &responseMsg);
        virtual void processGetRequest(Wt::Http::Response &response);

//        unsigned short postInformationForSeaSrcAndPlg(std::string &responseMsg, const std::string &sRequest);
//        unsigned short postSearchForSourceAndPlugin(std::string &responseMsg, const std::string &sRequest);
        unsigned short postSourceForPlugin(std::string &responseMsg, const std::string &sRequest);
        unsigned short postPlugin(std::string &responseMsg, const std::string &sRequest);
        virtual void processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
//        unsigned short putAliasForInformation(std::string &responseMsg, const std::string &sRequest);
//        unsigned short putAliasForCriterion(std::string &responseMsg, const std::string &sRequest);
        unsigned short putAliasForPlugin(std::string &responseMsg, const std::string &sRequest);
        
        
//        unsigned short patchInformationForSeaSrcAndPlg(std::string &responseMsg, const std::string &sRequest);
//        unsigned short patchSearchForSourceAndPlugin(std::string &responseMsg, const std::string &sRequest);
//        unsigned short patchParametersSourceForPlugin(std::string &responseMsg, const std::string &sRequest);
//        virtual void processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

//        unsigned short deleteInformationForSeaSrcAndPlg(std::string &responseMsg);
//        unsigned short deleteSearchForSourceAndPlugin(std::string &responseMsg);
        unsigned short deleteSourceForPlugin(std::string &responseMsg);
        unsigned short deletePlugin(std::string &responseMsg);
        virtual void processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
       
};

#endif	/* PLUGINRESOURCE_H */

