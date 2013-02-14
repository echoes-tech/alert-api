/* 
 * API Asset Ressource
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

#ifndef ASSETRESSOURCE_H
#define	ASSETRESSOURCE_H

#include "PublicApiResource.h"

class AssetRessource : public PublicApiResource
{
    public:
        AssetRessource();
        AssetRessource(const AssetRessource& orig);
        virtual ~AssetRessource();

    protected:
        int assetId, probeId, pluginId;
        
        std::string getAssetsList();
        std::string getAsset();
        std::string getPluginsListForAsset();
        std::string getProbesListForAsset();
        virtual void processGetRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        std::string postProbeForAsset(std::string sRequest);
        virtual void processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        
        std::string patchAsset(std::string sRequest);
        virtual void processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
};


#endif	/* ASSETRESSOURCE_H */

