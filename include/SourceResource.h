/**
 * @file SourceResource.h
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

#ifndef SOURCERESOURCE_H
#define	SOURCERESOURCE_H

#include "PublicApiResource.h"

class SourceResource : public PublicApiResource
{
    public:
        SourceResource(Echoes::Dbo::Session& session);
        virtual ~SourceResource();

        /**
         * Select a Source
         * @param srcId Identifier of Source
         * @param grpId Identifier of Group
         * @return Source Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Source> selectSource(const long long &srcId, const long long &grpId, Echoes::Dbo::Session &session);
        /**
         * Select an Source with a string of identifier of Source
         * @param srcId String of Identifier of Source
         * @param grpId Identifier of Group
         * @return Source Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Source> selectSource(const std::string &srcId, const long long &grpId, Echoes::Dbo::Session &session);
        /**
         * Select a Source with a ID string
         * @param srcId String of Identifier of Source
         * @param grpId String of Identifier of Group
         * @return Source Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Source> selectSource(const std::string &srcId, const std::string &grpId, Echoes::Dbo::Session &session);

    private:
        EReturnCode getSourcesList(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        EReturnCode getSource(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        EReturnCode getParametersList(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        EReturnCode getParametersListForSource(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);

        EReturnCode postSource(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);

        EReturnCode putSource(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        virtual EReturnCode processPutRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);

        EReturnCode deleteSource(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        virtual EReturnCode processDeleteRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);
};

#endif	/* SOURCERESOURCE_H */
