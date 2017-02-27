/**
 * @file ProbeResource.h
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

#ifndef PROBERESOURCE_H
#define	PROBERESOURCE_H

#include "PublicApiResource.h"
#include "AssetResource.h"

class ProbeResource : public PublicApiResource
{
    public:
        ProbeResource(Echoes::Dbo::Session& session);
        virtual ~ProbeResource();

        /**
         * Select a Probe
         * @param prbId Identifier of Probe
         * @param orgId Identifier of Organization
         * @return Probe Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Probe> selectProbe(const long long &prbId, const long long &orgId, Echoes::Dbo::Session &session);
        /**
         * Select an Probe with a string of identifier of Probe
         * @param prbId String of Identifier of Probe
         * @param orgId Identifier of Organization
         * @return Probe Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Probe> selectProbe(const std::string &prbId, const long long &orgId, Echoes::Dbo::Session &session);
        /**
         * Select a Probe with a ID string
         * @param prbId String of Identifier of Probe
         * @param orgId Identifier of Organization
         * @return Probe Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Probe> selectProbe(const std::string &prbId, const std::string &orgId, Echoes::Dbo::Session &session);

        /**
         * Select a Probe Package Parameter
         * @param prbId Identifier of Probe
         * @param orgId String of Identifier of Organization
         * @return Probe Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::ProbePackageParameter> selectProbePackageParameter(const Wt::Dbo::ptr<Echoes::Dbo::Asset> &astPtr, Echoes::Dbo::Session &session);
        
        static int                      m_defaultTimer;
        
    protected:
        std::map<long long, int>        m_mapTimer;
        
        EReturnCode getProbesList(const long long &orgId, std::string &responseMsg);
        EReturnCode getProbe(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        EReturnCode getAliveProbe(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        EReturnCode getJsonForProbe(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        EReturnCode getPackagesForProbe(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode postProbe(const std::string &sRequest,  const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode putProbe(const std::vector<std::string> &pathElements, const std::string &sRequest, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processPutRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode deleteProbe(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processDeleteRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);
};

#endif	/* PROBERESOURCE_H */
