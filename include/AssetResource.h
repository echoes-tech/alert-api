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
#include "FilterResource.h"
#include "ProbeResource.h"

class AssetResource : public PublicApiResource
{
    public:
        AssetResource();
        virtual ~AssetResource();

        /**
         * Select a Asset
         * @param astId Identifier of Asset
         * @param orgId Identifier of Organization
         * @return Asset Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Asset> selectAsset(const long long &astId, const long long &orgId, Echoes::Dbo::Session &session);
        /**
         * Select an Asset with a string of identifier of Asset
         * @param astId String of Identifier of Asset
         * @param orgId Identifier of Organization
         * @return Asset Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Asset> selectAsset(const std::string &astId, const long long &orgId, Echoes::Dbo::Session &session);
        /**
         * Select a Asset with a ID string
         * @param astId String of Identifier of Asset
         * @param orgId String of Identifier of Organization
         * @return Asset Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Asset> selectAsset(const std::string &astId, const std::string &orgId, Echoes::Dbo::Session &session);

    protected:
        EReturnCode getAssetsList(const long long &orgId, std::string &responseMsg);
        EReturnCode getAsset(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        EReturnCode getAliasForAsset(const std::vector<std::string> &pathElements, std::map<std::string, long long> &parameters, const long long &orgId, std::string  &responseMsg);
        EReturnCode getPluginsForAsset(const std::vector<std::string> &pathElements, std::map<std::string, long long> &parameters, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode postAsset(const std::string &sRequest, const long long &orgId, std::string &responseMsg);
        EReturnCode postPluginForAsset(const std::vector<std::string> &pathElements, const std::string &sRequest, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);
        
        EReturnCode putAsset(const std::vector<std::string> &pathElements, const std::string &sRequest, const long long &orgId, std::string &responseMsg);
        EReturnCode putAliasForAsset(const std::vector<std::string> &pathElements, const std::string &sRequest, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processPutRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode deleteAsset(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processDeleteRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);
};


#endif	/* ASSETRESOURCE_H */

