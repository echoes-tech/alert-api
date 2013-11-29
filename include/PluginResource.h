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
        
        EReturnCode pluginIsAccessible(std::string& responseMsg);
        
        EReturnCode getPluginJSON(std::string &responseMsg);
        EReturnCode getPlugin(std::string &responseMsg);
//        EReturnCode getParameterValueForSearch(std::string &responseMsg);
//        EReturnCode getParameterValueForSource(std::string &responseMsg);
//        EReturnCode getInformationForSeaSrcAndPlg(std::string &responseMsg);
//        EReturnCode getSearchForSourceAndPlugin(std::string &responseMsg);
//        EReturnCode getSearchForSearchIdForSourceAndPlugin(std::string &responseMsg);
        EReturnCode getSourceForPlugin(std::string &responseMsg);
        EReturnCode getInformationListForPlugin(std::string &responseMsg);
//        EReturnCode getAliasForInformation(std::string &responseMsg);
//        EReturnCode getAliasForCriteria(std::string &responseMsg);
//        EReturnCode getCriteriaForInformation(std::string &responseMsg);
        EReturnCode getAliasForPlugin(std::string &responseMsg);
        virtual void processGetRequest(Wt::Http::Response &response);

//        EReturnCode postInformationForSeaSrcAndPlg(std::string &responseMsg, const std::string &sRequest);
//        EReturnCode postSearchForSourceAndPlugin(std::string &responseMsg, const std::string &sRequest);
        EReturnCode postSourceForPlugin(std::string &responseMsg, const std::string &sRequest);
        EReturnCode postPlugin(std::string &responseMsg, const std::string &sRequest);
        virtual void processPostRequest(Wt::Http::Response &response);

        virtual void processPutRequest(Wt::Http::Response &response);
//        EReturnCode putAliasForInformation(std::string &responseMsg, const std::string &sRequest);
//        EReturnCode putAliasForCriterion(std::string &responseMsg, const std::string &sRequest);
        EReturnCode putAliasForPlugin(std::string &responseMsg, const std::string &sRequest);
        
        
//        EReturnCode patchInformationForSeaSrcAndPlg(std::string &responseMsg, const std::string &sRequest);
//        EReturnCode patchSearchForSourceAndPlugin(std::string &responseMsg, const std::string &sRequest);
//        EReturnCode patchParametersSourceForPlugin(std::string &responseMsg, const std::string &sRequest);
//        virtual void processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

//        EReturnCode deleteInformationForSeaSrcAndPlg(std::string &responseMsg);
//        EReturnCode deleteSearchForSourceAndPlugin(std::string &responseMsg);
        EReturnCode deleteSourceForPlugin(std::string &responseMsg);
        EReturnCode deletePlugin(std::string &responseMsg);
        virtual void processDeleteRequest(Wt::Http::Response &response);

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
       
};

#endif	/* PLUGINRESOURCE_H */

