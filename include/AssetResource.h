/* 
 * API Asset Resource
 * @author ECHOES Technologies (FPO)
 * @date 27/09/2012
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2012 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef ASSETRESOURCE_H
#define	ASSETRESOURCE_H

#include "PublicApiResource.h"

class AssetResource : public PublicApiResource
{
    public:
        AssetResource();
        virtual ~AssetResource();

    protected:
        std::string m_role;
        std::string m_media;
        
        unsigned short getKeyValueForInformation(std::string &responseMsg);
        unsigned short getAssetsList(std::string &responseMsg);
        unsigned short getAsset(std::string &responseMsg);
        unsigned short getPluginsListForAsset(std::string &responseMsg);
        unsigned short getProbesListForAsset(std::string  &responseMsg);
        unsigned short getAliasForAsset(std::string  &responseMsg);
        virtual void processGetRequest(Wt::Http::Response &response);

        unsigned short postProbeForAsset(std::string &responseMsg, const std::string &sRequest);
        virtual void processPostRequest(Wt::Http::Response &response);
        
        unsigned short putAsset(std::string &responseMsg, const std::string &sRequest);
        unsigned short putAlias(std::string &responseMsg, const std::string &sRequest);
        virtual void processPutRequest(Wt::Http::Response &response);

        virtual void processDeleteRequest(Wt::Http::Response &response);

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        
        
};


#endif	/* ASSETRESOURCE_H */

