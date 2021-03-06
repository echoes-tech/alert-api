/**
 * @file FilterResource.h
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

#ifndef FILTERRESOURCE_H
#define	FILTERRESOURCE_H

#include "PublicApiResource.h"
#include "SearchResource.h"

class FilterResource : public PublicApiResource
{
    public:
        FilterResource(Echoes::Dbo::Session& session);
        virtual ~FilterResource();

        /**
         * Select a Filter
         * @param filId Identifier of Filter
         * @param orgId Identifier of Organization
         * @return Filter Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Filter> selectFilter(const long long &filId, const long long &orgId, Echoes::Dbo::Session &session);
        /**
         * Select an Filter with a string of identifier of Filter
         * @param filId String of Identifier of Filter
         * @param orgId Identifier of Organization
         * @return Filter Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Filter> selectFilter(const std::string &filId, const long long &orgId, Echoes::Dbo::Session &session);
        /**
         * Select a Filter with a ID string
         * @param filId String of Identifier of Filter
         * @param orgId String of Identifier of Organization
         * @return Filter Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Filter> selectFilter(const std::string &filId, const std::string &orgId, Echoes::Dbo::Session &session);

    private:
        EReturnCode getFiltersList(std::map<std::string, long long> &parameters, const long long &orgId, std::string &responseMsg);
        EReturnCode getFilter(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        EReturnCode getParametersList(std::map<std::string, long long> &parameters, const long long &orgId, std::string &responseMsg);
        EReturnCode getParametersListForFilter(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode postFilter(const std::string &sRequest, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode putFilter(const std::vector<std::string> &pathElements, const std::string &sRequest, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processPutRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode deleteFilter(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processDeleteRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);
};

#endif	/* FILTERRESOURCE_H */
