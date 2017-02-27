/**
 * @file UnitResource.h
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

#ifndef UNITRESOURCE_H
#define	UNITRESOURCE_H

#include "PublicApiResource.h"

class UnitResource : public PublicApiResource
{
    public :
        UnitResource(Echoes::Dbo::Session& session);
        virtual ~UnitResource();

    protected :
        EReturnCode getUnitsList(const long long &orgId, std::string &responseMsg);
        EReturnCode getUnit(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode postUnit(const std::string &sRequest,  const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode putUnit(const std::vector<std::string> &pathElements, const std::string &sRequest, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processPutRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode deleteUnit(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processDeleteRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);
};


#endif	/* UNITRESOURCE_H */
