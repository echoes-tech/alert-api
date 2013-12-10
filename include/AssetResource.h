/* 
 * API Asset Resource
 * @author ECHOES Technologies (FPO)
 * @date 27/09/2012
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2012-2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef ASSETRESOURCE_H
#define	ASSETRESOURCE_H

#include "PublicApiResource.h"
#include "ProbeResource.h"

class AssetResource : public PublicApiResource
{
    public:
        AssetResource();
        virtual ~AssetResource();

    protected:
        EReturnCode getAssetsList(std::string &responseMsg);
        EReturnCode getAsset(std::string &responseMsg);
        EReturnCode getProbesListForAsset(std::string  &responseMsg);
        EReturnCode getAliasForAsset(std::string  &responseMsg);
        virtual void processGetRequest(Wt::Http::Response &response);

        EReturnCode postAsset(std::string &responseMsg, const std::string &sRequest);
        virtual void processPostRequest(Wt::Http::Response &response);
        
        EReturnCode putAsset(std::string &responseMsg, const std::string &sRequest);
        EReturnCode putAliasForAsset(std::string &responseMsg, const std::string &sRequest);
        virtual void processPutRequest(Wt::Http::Response &response);

        EReturnCode deleteAsset(std::string &responseMsg);
        virtual void processDeleteRequest(Wt::Http::Response &response);
};


#endif	/* ASSETRESOURCE_H */

