/**
 * @file AssetResource.h
 * @author Thomas Saquet, Florent Poinsaut
 * @date 
 * @brief File containing example of doxygen usage for quick reference.
 *
 * Alert - API is a part of the Alert software
 * Copyright (C) 2013-2017
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
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
        AssetResource(Echoes::Dbo::Session& session);
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
