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
        AssetResource(const AssetResource& orig);
        virtual ~AssetResource();

    protected:
        unsigned short getKeyValueForInformation(std::string &responseMsg) const;
        unsigned short getAssetsList(std::string &responseMsg) const;
        unsigned short getAsset(std::string &responseMsg) const;
        unsigned short getPluginsListForAsset(std::string &responseMsg) const;
        unsigned short getProbesListForAsset(std::string  &responseMsg) const;
        unsigned short getAliasForAsset(std::string  &responseMsg) const;
        virtual void processGetRequest(Wt::Http::Response &response);

        unsigned short postProbeForAsset(std::string &responseMsg, const std::string &sRequest);
        virtual void processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        
        unsigned short putAsset(std::string &responseMsg, const std::string &sRequest);
        unsigned short putAlias(std::string &responseMsg, const std::string &sRequest);
        virtual void processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        
        std::string role;
        std::string media;
};


#endif	/* ASSETRESOURCE_H */

