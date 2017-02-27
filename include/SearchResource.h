/**
 * @file SearchResource.h
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

#ifndef SEARCHRESOURCE_H
#define	SEARCHRESOURCE_H

#include "PublicApiResource.h"
#include "SourceResource.h"

class SearchResource : public PublicApiResource
{
    public:
        SearchResource(Echoes::Dbo::Session& session);
        virtual ~SearchResource();

        /**
         * Select a Search
         * @param seaId Identifier of Search
         * @param orgId Identifier of Organization
         * @return Search Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Search> selectSearch(const long long &seaId, const long long &orgId, Echoes::Dbo::Session &session);
        /**
         * Select an Search with a string of identifier of Search
         * @param seaId String of Identifier of Search
         * @param orgId Identifier of Organization
         * @return Search Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Search> selectSearch(const std::string &seaId, const long long &orgId, Echoes::Dbo::Session &session);
        /**
         * Select a Search with a ID string
         * @param seaId String of Identifier of Search
         * @param orgId String of Identifier of Organization
         * @return Search Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Search> selectSearch(const std::string &seaId, const std::string &orgId, Echoes::Dbo::Session &session);

    private:
        EReturnCode getSearchsList(std::map<std::string, long long> &parameters, const long long &orgId, std::string &responseMsg);
        EReturnCode getSearch(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        EReturnCode getParametersList(std::map<std::string, long long> &parameters, const long long &orgId, std::string &responseMsg);
        EReturnCode getParametersListForSearch(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode postSearch(const std::string &sRequest,  const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode putSearch(const std::vector<std::string> &pathElements, const std::string &sRequest,  const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processPutRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode deleteSearch(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processDeleteRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);
};

#endif	/* SEARCHRESOURCE_H */
