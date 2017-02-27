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
         * @param grpId Identifier of Group
         * @return Asset Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Asset> selectAsset(const long long &astId, const long long &grpId, Echoes::Dbo::Session &session);
        /**
         * Select an Asset with a string of identifier of Asset
         * @param astId String of Identifier of Asset
         * @param grpId Identifier of Group
         * @return Asset Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Asset> selectAsset(const std::string &astId, const long long &grpId, Echoes::Dbo::Session &session);
        /**
         * Select a Asset with a ID string
         * @param astId String of Identifier of Asset
         * @param grpId String of Identifier of Group
         * @return Asset Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Asset> selectAsset(const std::string &astId, const std::string &grpId, Echoes::Dbo::Session &session);

    protected:
        
        EReturnCode getAssetsList(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        EReturnCode getAsset(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        EReturnCode getAliasForAsset(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        EReturnCode getPluginsForAsset(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);

        EReturnCode postAsset(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        EReturnCode postPluginsForAsset(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);
        
        EReturnCode putAsset(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        EReturnCode putAliasForAsset(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        virtual EReturnCode processPutRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);

        EReturnCode deleteAsset(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        virtual EReturnCode processDeleteRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);
};


#endif	/* ASSETRESOURCE_H */
