/**
 * @file AddonResource.cpp
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

#include "AddonResource.h"

using namespace std;

AddonResource::AddonResource(Echoes::Dbo::Session& session) : PublicApiResource::PublicApiResource(session)
{
}

AddonResource::~AddonResource()
{
}

EReturnCode AddonResource::getAddonsList(const long long &orgId, string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Addon>> adoPtrCol = m_session.find<Echoes::Dbo::Addon>()
                .where(QUOTE(TRIGRAM_ADDON SEP "DELETE") " IS NULL")
                .orderBy(QUOTE(TRIGRAM_ADDON ID));

        res = serialize(adoPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode AddonResource::getAddon(const vector<string> &pathElements, const long long &orgId, string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::Addon> adoPtr = m_session.find<Echoes::Dbo::Addon>()
                .where(QUOTE(TRIGRAM_ADDON ID) " = ?").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_ADDON SEP "DELETE") " IS NULL");

        res = serialize(adoPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode AddonResource::getSearchTypeForAddon(const vector<string> &pathElements, const long long &orgId, string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        const string queryStr =
        " SELECT sty"
        "   FROM " QUOTE(TABLE_REFERENCE_PREFIX SEP "SEARCH_TYPE" SEP TRIGRAM_SEARCH_TYPE) " sty"
        "   WHERE"
        "     " QUOTE(TRIGRAM_SEARCH_TYPE ID) " IN"
        "       ("
        "         SELECT " QUOTE(TABLE_REFERENCE_PREFIX SEP "SEARCH_TYPE" SEP TRIGRAM_SEARCH_TYPE SEP TRIGRAM_SEARCH_TYPE ID)
        "           FROM " QUOTE(TABLE_JOINT_PREFIX SEP TRIGRAM_ADDON SEP TRIGRAM_SEARCH_TYPE)
        "           WHERE " QUOTE(TABLE_REFERENCE_PREFIX SEP "ADDON" SEP TRIGRAM_ADDON SEP TRIGRAM_ADDON ID) " ="
        "             ("
        "               SELECT " QUOTE(TRIGRAM_ADDON ID)
        "                 FROM " QUOTE(TABLE_REFERENCE_PREFIX SEP "ADDON" SEP TRIGRAM_ADDON)
        "                 WHERE"
        "                   " QUOTE(TRIGRAM_ADDON ID) " = " + pathElements[1] +
        "                   AND " QUOTE(TRIGRAM_ADDON SEP "DELETE") " IS NULL"
        "             )"
        "       )"
        "     AND " QUOTE(TRIGRAM_SEARCH_TYPE SEP "DELETE") " IS NULL"
        " ORDER BY " QUOTE(TRIGRAM_SEARCH_TYPE ID);

        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::SearchType>> queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::SearchType>>(queryStr);        
        
        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SearchType>> styPtrCol = queryRes.resultList();

        res = serialize(styPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode AddonResource::processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    string nextElement = "";
    unsigned short indexPathElement = 1;
    vector<string> pathElements;
    map<string, long long> parameters;

    const string sRequest = processRequestParameters(request, pathElements, parameters);

    nextElement = getNextElementFromPath(indexPathElement, pathElements);

    if (nextElement.empty())
    {
        res = getAddonsList(orgId, responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);
            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.empty())
            {
                res = getAddon(pathElements, orgId, responseMsg);
            }
            else if (nextElement.compare("search_types") == 0)
            {
                res = getSearchTypeForAddon(pathElements, orgId, responseMsg);                
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Addon Resource] bad nextElement";
                responseMsg = httpCodeToJSON(res, err);
            }
        }
        catch (boost::bad_lexical_cast const& e)
        {
            res = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(res, e);
        }
    }

    return res;
}
