/**
 * @file RoleResource.cpp
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

#include "RoleResource.h"

using namespace std;

RoleResource::RoleResource(Echoes::Dbo::Session& session) : PublicApiResource::PublicApiResource(session)
{
    resourceClassName = "RoleResource";

    functionMap["getRolesList"] = boost::bind(&RoleResource::getRolesList, this, _1, _2, _3, _4, _5);
    functionMap["getRole"] = boost::bind(&RoleResource::getRole, this, _1, _2, _3, _4, _5);
    functionMap["postRole"] = boost::bind(&RoleResource::postRole, this, _1, _2, _3, _4, _5);
    functionMap["putRole"] = boost::bind(&RoleResource::putRole, this, _1, _2, _3, _4, _5);
    functionMap["deleteRole"] = boost::bind(&RoleResource::deleteRole, this, _1, _2, _3, _4, _5);
    
    calls = FillCallsVector();
}

RoleResource::~RoleResource()
{
}

EReturnCode RoleResource::getRolesList(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::UserRole>> uroPtrCol = m_session.find<Echoes::Dbo::UserRole>()
                .where(QUOTE(TRIGRAM_USER_ROLE SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_USER_ROLE SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = ?").bind(grpId)
                .orderBy(QUOTE(TRIGRAM_USER_ROLE ID));

        res = serialize(uroPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode RoleResource::getRole(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::UserRole> uroPtr = m_session.find<Echoes::Dbo::UserRole>()
                .where(QUOTE(TRIGRAM_USER_ROLE ID) " = ?").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_USER_ROLE SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_USER_ROLE SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = ?").bind(grpId);

        res = serialize(uroPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode RoleResource::processGetRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
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
        res = getRolesList(grpId, responseMsg, pathElements);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.empty())
            {
                res = getRole(grpId, responseMsg, pathElements);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Role Resource] bad nextElement";
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

EReturnCode RoleResource::postRole(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    Wt::WString name;

    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            name = result.get("name");
        }
        catch (Wt::Json::ParseError const& e)
        {
            res = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(res, e);
        }
        catch (Wt::Json::TypeException const& e)
        {
            res = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(res, e);
        }
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Role Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session, true);

            Wt::Dbo::ptr<Echoes::Dbo::Group> grpPtr = m_session.find<Echoes::Dbo::Group>()
                    .where(QUOTE(TRIGRAM_GROUP SEP "DELETE") " IS NULL")
                    .where(QUOTE(TRIGRAM_GROUP ID) " = ?").bind(grpId);
            if (!grpPtr)
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, grpPtr);
                return res;
            }

            Echoes::Dbo::UserRole *newUro = new Echoes::Dbo::UserRole();
            newUro->name = name;
            newUro->group = grpPtr;

            Wt::Dbo::ptr<Echoes::Dbo::UserRole> newUroPtr = m_session.add<Echoes::Dbo::UserRole>(newUro);
            newUroPtr.flush();

            res = serialize(newUroPtr, responseMsg, EReturnCode::CREATED);

            transaction.commit();
        }
        catch (Wt::Dbo::Exception const& e)
        {
            res = EReturnCode::SERVICE_UNAVAILABLE;
            responseMsg = httpCodeToJSON(res, e);
        }
    }

    return res;
}

EReturnCode RoleResource::processPostRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
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
        res = postRole(grpId, responseMsg, pathElements, sRequest);
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Role Resource] bad nextElement";
        responseMsg = httpCodeToJSON(res, err);
    }

    return res;
}

EReturnCode RoleResource::putRole(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    Wt::WString name;

    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            if (result.contains("name"))
            {
                name = result.get("name");
            }
        }
        catch (Wt::Json::ParseError const& e)
        {
            res = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(res, e);
        }
        catch (Wt::Json::TypeException const& e)
        {
            res = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(res, e);
        }
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Role Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session, true);

            Wt::Dbo::ptr<Echoes::Dbo::UserRole> uroPtr = m_session.find<Echoes::Dbo::UserRole>()
                .where(QUOTE(TRIGRAM_USER_ROLE ID) " = ?").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_USER_ROLE SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_USER_ROLE SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = ?").bind(grpId);

            if (uroPtr)
            {
                if (!name.empty())
                {
                    uroPtr.modify()->name = name;
                }

                res = serialize(uroPtr, responseMsg);
            }
            else
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, uroPtr);
            }

            transaction.commit();
        }
        catch (Wt::Dbo::Exception const& e)
        {
            res = EReturnCode::SERVICE_UNAVAILABLE;
            responseMsg = httpCodeToJSON(res, e);
        }
    }

    return res;
}

EReturnCode RoleResource::processPutRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
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
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Role Resource] bad nextElement";
        responseMsg = httpCodeToJSON(res, err);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);

            if (nextElement.empty())
            {
                res = putRole(grpId, responseMsg, pathElements, sRequest);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Role Resource] bad nextElement";
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

EReturnCode RoleResource::deleteRole(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try 
    {  
        Wt::Dbo::Transaction transaction(m_session, true);
           
        Wt::Dbo::ptr<Echoes::Dbo::UserRole> ustPtr = m_session.find<Echoes::Dbo::UserRole>()
                .where(QUOTE(TRIGRAM_USER_ROLE ID) " = ?").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_USER_ROLE SEP "DELETE") " IS NULL");;

        if(ustPtr)
        {

            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasInformationCriteria>> aicPtr = m_session.find<Echoes::Dbo::AlertMessageAliasInformationCriteria>()
                    .where(QUOTE(TRIGRAM_USER_ROLE ID SEP TRIGRAM_USER_ROLE ID)" = ?").bind(pathElements[1])
                    .where(QUOTE(TRIGRAM_ALERT_MESSAGE_ALIAS_INFORMATION_CRITERIA SEP "DELETE") " IS NULL");

            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasInformation>> aaiPtr = m_session.find<Echoes::Dbo::AlertMessageAliasInformation>()
                    .where(QUOTE(TRIGRAM_USER_ROLE ID SEP TRIGRAM_USER_ROLE ID)" = ?").bind(pathElements[1])
                    .where(QUOTE(TRIGRAM_ALERT_MESSAGE_ALIAS_INFORMATION SEP "DELETE") " IS NULL");
            
            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasAsset>> aaaPtr = m_session.find<Echoes::Dbo::AlertMessageAliasAsset>()
                    .where(QUOTE(TRIGRAM_USER_ROLE ID SEP TRIGRAM_USER_ROLE ID)" = ?").bind(pathElements[1])
                    .where(QUOTE(TRIGRAM_ALERT_MESSAGE_ALIAS_ASSET SEP "DELETE") " IS NULL");
            
            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasPlugin>> aapPtr = m_session.find<Echoes::Dbo::AlertMessageAliasPlugin>()
                    .where(QUOTE(TRIGRAM_USER_ROLE ID SEP TRIGRAM_USER_ROLE ID)" = ?").bind(pathElements[1])
                    .where(QUOTE(TRIGRAM_ALERT_MESSAGE_ALIAS_PLUGIN SEP "DELETE") " IS NULL");
            
            if (aicPtr.size() == 0 && aaiPtr.size() == 0 && aaaPtr.size() == 0 && aapPtr.size() == 0)
            {
                ustPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
                res = EReturnCode::NO_CONTENT;
            }
            else
            {
                res = EReturnCode::CONFLICT;
                responseMsg = httpCodeToJSON(res, ustPtr);
            } 
        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = httpCodeToJSON(res, ustPtr);
        }

        transaction.commit();
    }
    catch (boost::bad_lexical_cast const& e)
    {
        res = EReturnCode::BAD_REQUEST;
        responseMsg = httpCodeToJSON(res, e);
    }

    return res;
}

EReturnCode RoleResource::processDeleteRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
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
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Role Resource] bad nextElement";
        responseMsg = httpCodeToJSON(res, err);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);

            if (nextElement.empty())
            {
                res = deleteRole(grpId, responseMsg, pathElements, sRequest);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Role Resource] bad nextElement";
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
