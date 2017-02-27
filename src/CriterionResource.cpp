/**
 * @file CriterionResource.cpp
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

#include "CriterionResource.h"

using namespace std;

CriterionResource::CriterionResource(Echoes::Dbo::Session& session) : PublicApiResource::PublicApiResource(session)
{
}

CriterionResource::~CriterionResource()
{
}

EReturnCode CriterionResource::getCriteriaList(const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertCriteria>> acrPtrCol = m_session.find<Echoes::Dbo::AlertCriteria>()
                .where(QUOTE(TRIGRAM_ALERT_CRITERIA SEP "DELETE") " IS NULL")
                .orderBy(QUOTE(TRIGRAM_ALERT_CRITERIA ID));

        res = serialize(acrPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode CriterionResource::getCriterion(const vector<string> &pathElements, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::AlertCriteria> acrPtr = m_session.find<Echoes::Dbo::AlertCriteria>()
                .where(QUOTE(TRIGRAM_ALERT_CRITERIA ID) " = ?").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_ALERT_CRITERIA SEP "DELETE") " IS NULL");

        res = serialize(acrPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode CriterionResource::getAliasForCriterion(const std::vector<std::string> &pathElements, map<string, long long> &parameters, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    if (parameters["media_type_id"] <= 0 || parameters["user_role_id"] <= 0 || parameters["information_id"] <= 0)
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Criterion Resource] media_types or/and user_role or/and information are empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session, true);

            Wt::Dbo::ptr<Echoes::Dbo::UserRole> uroPtr = m_session.find<Echoes::Dbo::UserRole>()
                    .where(QUOTE(TRIGRAM_USER_ROLE ID) " = ?").bind(parameters["user_role_id"])
                    .where(QUOTE(TRIGRAM_USER_ROLE SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(orgId)
                    .where(QUOTE(TRIGRAM_USER_ROLE SEP "DELETE") " IS NULL");
            if (uroPtr)
            {
                Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasInformationCriteria> aicPtr = m_session.find<Echoes::Dbo::AlertMessageAliasInformationCriteria>()
                        .where(QUOTE(TRIGRAM_ALERT_MESSAGE_ALIAS_INFORMATION_CRITERIA SEP "DELETE") " IS NULL")
                        .where(QUOTE(TRIGRAM_USER_ROLE ID SEP TRIGRAM_USER_ROLE ID) " = ?").bind(parameters["user_role_id"])
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE ID SEP TRIGRAM_MEDIA_TYPE ID) " = ?").bind(parameters["media_type_id"])
                        .where(QUOTE(TRIGRAM_INFORMATION ID SEP TRIGRAM_INFORMATION ID) " = ?").bind(parameters["information_id"])
                        .where(QUOTE(TRIGRAM_ALERT_CRITERIA ID SEP TRIGRAM_ALERT_CRITERIA ID) " = ?").bind(pathElements[1]);

                res = serialize(aicPtr, responseMsg);
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

EReturnCode CriterionResource::processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    string nextElement = "";
    unsigned short indexPathElement = 1;
    vector<string> pathElements;
    map<string, long long> parameters;

    parameters["media_type_id"] = 0;
    parameters["user_role_id"] = 0;
    parameters["information_id"] = 0;
    
    const string sRequest = processRequestParameters(request, pathElements, parameters);

    nextElement = getNextElementFromPath(indexPathElement, pathElements);
    if (nextElement.empty())
    {
        res = getCriteriaList(orgId, responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.empty())
            {
                res = getCriterion(pathElements, orgId, responseMsg);
            }
            else if (!nextElement.compare("alias"))
            {
                res = getAliasForCriterion(pathElements, parameters, orgId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Criterion Resource] bad nextElement";
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

EReturnCode CriterionResource::putAliasForCriterion(const std::vector<std::string> &pathElements, const string &sRequest, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    long long uroId;
    long long mtyId;
    long long infId;
    Wt::WString value;

    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            uroId = result.get("user_role_id");
            mtyId = result.get("media_type_id");
            infId = result.get("information_id");
            value = result.get("value");
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
        const string err = "[Criterion Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }
    

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session, true);

            Wt::Dbo::ptr<Echoes::Dbo::AlertCriteria> acrPtr =  m_session.find<Echoes::Dbo::AlertCriteria>()
                .where(QUOTE(TRIGRAM_ALERT_CRITERIA ID) " = ?").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_ALERT_CRITERIA SEP "DELETE") " IS NULL");;
            if (acrPtr)
            {
                Wt::Dbo::ptr<Echoes::Dbo::Information> infPtr = m_session.find<Echoes::Dbo::Information>()
                         .where(QUOTE(TRIGRAM_INFORMATION ID) " = ?").bind(infId)
                         .where(QUOTE(TRIGRAM_INFORMATION SEP "DELETE") " IS NULL");
                if (!infPtr)
                {
                    res = EReturnCode::NOT_FOUND;
                    responseMsg = httpCodeToJSON(res, infPtr);
                    return res;
                }

                Wt::Dbo::ptr<Echoes::Dbo::UserRole> uroPtr = m_session.find<Echoes::Dbo::UserRole>()
                        .where(QUOTE(TRIGRAM_USER_ROLE ID) " = ?").bind(uroId)
                        .where(QUOTE(TRIGRAM_USER_ROLE SEP "DELETE") " IS NULL");
                if (!uroPtr)
                {
                    res = EReturnCode::NOT_FOUND;
                    responseMsg = httpCodeToJSON(res, uroPtr);
                    return res;
                }

                Wt::Dbo::ptr<Echoes::Dbo::MediaType> mtyPtr = m_session.find<Echoes::Dbo::MediaType>()
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE ID) " = ?").bind(mtyId)
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE SEP "DELETE") " IS NULL");
                if (!mtyPtr)
                {
                    res = EReturnCode::NOT_FOUND;
                    responseMsg = httpCodeToJSON(res, mtyPtr);
                    return res;
                }

                Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasInformationCriteria> aicPtr = m_session.find<Echoes::Dbo::AlertMessageAliasInformationCriteria>()
                        .where(QUOTE(TRIGRAM_USER_ROLE ID SEP TRIGRAM_USER_ROLE ID) " = ?").bind(uroId)
                        .where(QUOTE(TRIGRAM_INFORMATION ID SEP TRIGRAM_INFORMATION ID) " = ?").bind(infId)
                        .where(QUOTE(TRIGRAM_ALERT_CRITERIA ID SEP TRIGRAM_ALERT_CRITERIA ID) " = ?").bind(pathElements[1])
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE ID SEP TRIGRAM_MEDIA_TYPE ID) " = ?").bind(mtyId);
                if (aicPtr)
                {
                    aicPtr.modify()->alias = value;
                }
                else
                {
                    Echoes::Dbo::AlertMessageAliasInformationCriteria *newAic = new Echoes::Dbo::AlertMessageAliasInformationCriteria();
                    newAic->pk.alertCriteria = acrPtr;
                    newAic->pk.information = infPtr;
                    newAic->pk.userRole = uroPtr;
                    newAic->pk.mediaType = mtyPtr;
                    newAic->alias = value;
                    aicPtr = m_session.add<Echoes::Dbo::AlertMessageAliasInformationCriteria>(newAic);
                }
                res = EReturnCode::OK;
            }
            else
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, acrPtr);
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

EReturnCode CriterionResource::processPutRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        const string err = "[Criterion Resource] bad nextElement";
        responseMsg = httpCodeToJSON(res, err);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);

            if (!nextElement.compare("alias"))
            {
                res = putAliasForCriterion(pathElements, sRequest, orgId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Criterion Resource] bad nextElement";
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
