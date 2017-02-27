/**
 * @file OptionResource.cpp
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

#include "OptionResource.h"

using namespace std;

OptionResource::OptionResource(Echoes::Dbo::Session& session) : PublicApiResource::PublicApiResource(session)
{
    resourceClassName = "OptionResource";

    functionMap["getOptionsList"] = boost::bind(&OptionResource::getOptionsList, this, _1, _2, _3, _4, _5); 
    functionMap["getOption"] = boost::bind(&OptionResource::getOption, this, _1, _2, _3, _4, _5);
    
    calls = FillCallsVector();
}

OptionResource::~OptionResource()
{
}

EReturnCode OptionResource::getOptionsList(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Option>> queryRes = m_session.find<Echoes::Dbo::Option>()
                .where(QUOTE(TRIGRAM_OPTION SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_OPTION SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = ?").bind(grpId)
                .orderBy(QUOTE(TRIGRAM_OPTION ID));

        if (parameters["type_id"] > 0)
        {
            queryRes = queryRes.where(QUOTE(TRIGRAM_OPTION SEP TRIGRAM_OPTION_TYPE SEP TRIGRAM_OPTION_TYPE ID) " = ?");
            // FPO: I don't know why but we have to bind every marker here to make the binding.
            queryRes.bind(grpId).bind(parameters["type_id"]);
        }

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Option>> optPtrCol =  queryRes.resultList();

        res = serialize(optPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode OptionResource::getOption(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::Option> optPtr = m_session.find<Echoes::Dbo::Option>()
                .where(QUOTE(TRIGRAM_OPTION ID) " = ?").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_OPTION SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_OPTION SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = ?").bind(grpId);

        res = serialize(optPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode OptionResource::processGetRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    string nextElement = "";
    unsigned short indexPathElement = 1;
    vector<string> pathElements;
    map<string, long long> parameters;

    parameters["type_id"] = 0;
    
    const string sRequest = processRequestParameters(request, pathElements, parameters);

    nextElement = getNextElementFromPath(indexPathElement, pathElements);
    if (nextElement.empty())
    {
        res = getOptionsList(grpId, responseMsg, pathElements, sRequest, parameters);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.empty())
            {
                res = getOption(grpId, responseMsg, pathElements);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Option Resource] bad nextElement";
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
