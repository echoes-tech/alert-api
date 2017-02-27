/**
 * @file FilterResource.cpp
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

#include "FilterResource.h"

using namespace std;

FilterResource::FilterResource(Echoes::Dbo::Session& session) : PublicApiResource::PublicApiResource(session)
{
    resourceClassName = "FilterResource";
    
    functionMap["getFiltersList"] = boost::bind(&FilterResource::getFiltersList, this, _1, _2, _3, _4, _5);
    functionMap["getFilter"] = boost::bind(&FilterResource::getFilter, this, _1, _2, _3, _4, _5);
    functionMap["getParametersListForFilters"] = boost::bind(&FilterResource::getParametersListForFilters, this, _1, _2, _3, _4, _5);
    functionMap["getParametersForFilter"] = boost::bind(&FilterResource::getParametersForFilter, this, _1, _2, _3, _4, _5);
    functionMap["postFilters"] = boost::bind(&FilterResource::postFilters, this, _1, _2, _3, _4, _5);
    functionMap["putFilters"] = boost::bind(&FilterResource::putFilters, this, _1, _2, _3, _4, _5);
    functionMap["deleteFilters"] = boost::bind(&FilterResource::deleteFilters, this, _1, _2, _3, _4, _5);
    
    calls = FillCallsVector();
}

FilterResource::~FilterResource()
{
}

Wt::Dbo::ptr<Echoes::Dbo::Filter> FilterResource::selectFilter(const long long &filId, const long long &grpId, Echoes::Dbo::Session &session)
{
    return selectFilter(boost::lexical_cast<string>(filId), boost::lexical_cast<string>(grpId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::Filter> FilterResource::selectFilter(const string &filId, const long long &grpId, Echoes::Dbo::Session &session)
{
    return selectFilter(filId, boost::lexical_cast<string>(grpId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::Filter> FilterResource::selectFilter(const string &filId, const std::string &grpId, Echoes::Dbo::Session &session)
{
    const string queryStr =
" SELECT fil"
"   FROM " QUOTE("T_FILTER" SEP TRIGRAM_FILTER) " fil"
"   WHERE"
"     " QUOTE(TRIGRAM_FILTER SEP TRIGRAM_SEARCH SEP TRIGRAM_SEARCH ID) " IN"
"       ("
"         SELECT " QUOTE(TRIGRAM_SEARCH ID)
"           FROM " QUOTE("T_SEARCH" SEP TRIGRAM_SEARCH)
"           WHERE"
"             " QUOTE(TRIGRAM_SEARCH SEP TRIGRAM_SOURCE SEP TRIGRAM_SOURCE ID) " IN"
"               ("
"                 SELECT " QUOTE(TRIGRAM_SOURCE ID)
"                   FROM " QUOTE("T_SOURCE" SEP TRIGRAM_SOURCE)
"                     WHERE"
"                       " QUOTE(TRIGRAM_SOURCE ID) " IN"
"                         ("
"                           SELECT " QUOTE("T_SOURCE" SEP TRIGRAM_SOURCE SEP TRIGRAM_SOURCE ID)
"                             FROM " QUOTE("TJ" SEP TRIGRAM_PLUGIN SEP TRIGRAM_SOURCE)
"                               WHERE " QUOTE("T_PLUGIN" SEP TRIGRAM_PLUGIN SEP TRIGRAM_PLUGIN ID) " IN"
"                                 ("
"                                   SELECT " QUOTE(TRIGRAM_PLUGIN ID)
"                                     FROM " QUOTE("T_PLUGIN" SEP TRIGRAM_PLUGIN)
"                                       WHERE"
"                                         " QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = " + grpId +
"                                         AND " QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL"
"                                 )"
"                         )"
"                       AND " QUOTE(TRIGRAM_SOURCE SEP "DELETE") " IS NULL"
"               )"
"             AND " QUOTE(TRIGRAM_SEARCH SEP "DELETE") " IS NULL"
"       )"
"     AND " QUOTE(TRIGRAM_FILTER ID) " = " + filId +
"     AND " QUOTE(TRIGRAM_FILTER SEP "DELETE") " IS NULL"
"   LIMIT 1";

    Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Filter>> queryRes = session.query<Wt::Dbo::ptr<Echoes::Dbo::Filter>>(queryStr);

    return queryRes.resultValue();
}

EReturnCode FilterResource::getFiltersList(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);
        string queryStr =
" SELECT fil\n"
"   FROM " QUOTE("T_FILTER" SEP TRIGRAM_FILTER) " fil\n"
"   WHERE\n"
"     " QUOTE(TRIGRAM_FILTER SEP TRIGRAM_SEARCH SEP TRIGRAM_SEARCH ID) " IN\n"
"       (\n"
"         SELECT " QUOTE(TRIGRAM_SEARCH ID) "\n"
"           FROM " QUOTE("T_SEARCH" SEP TRIGRAM_SEARCH) "\n"
"           WHERE\n"
"             " QUOTE(TRIGRAM_SEARCH SEP TRIGRAM_SOURCE SEP TRIGRAM_SOURCE ID) " IN\n"
"               (\n"
"                 SELECT " QUOTE(TRIGRAM_SOURCE ID) "\n"
"                   FROM " QUOTE("T_SOURCE" SEP TRIGRAM_SOURCE) "\n"
"                     WHERE\n"
"                       " QUOTE(TRIGRAM_SOURCE ID) " IN\n"
"                         (\n"
"                           SELECT " QUOTE("T_SOURCE" SEP TRIGRAM_SOURCE SEP TRIGRAM_SOURCE ID) "\n"
"                             FROM " QUOTE("TJ" SEP TRIGRAM_PLUGIN SEP TRIGRAM_SOURCE) "\n"
"                               WHERE " QUOTE("T_PLUGIN" SEP TRIGRAM_PLUGIN SEP TRIGRAM_PLUGIN ID) " IN\n"
"                                 (\n"
"                                   SELECT " QUOTE(TRIGRAM_PLUGIN ID) "\n"
"                                     FROM " QUOTE("T_PLUGIN" SEP TRIGRAM_PLUGIN) "\n"
"                                       WHERE\n"
"                                         " QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = " + boost::lexical_cast<string>(grpId) +  "\n";

        if (parameters["plugin_id"] > 0)
        {
            queryStr +=
"                                         AND " QUOTE(TRIGRAM_PLUGIN ID) " = " + boost::lexical_cast<string>(parameters["plugin_id"]) + "\n";
        }

        queryStr +=
"                                         AND " QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL\n"
"                                 )\n";

        if (parameters["source_id"] > 0)
        {
            queryStr +=
"                                 AND " QUOTE("T_SOURCE" SEP TRIGRAM_SOURCE SEP TRIGRAM_SOURCE ID) " = " + boost::lexical_cast<string>(parameters["source_id"]) + "\n";
        }

        queryStr +=
"                         )\n"
"                       AND " QUOTE(TRIGRAM_SOURCE SEP "DELETE") " IS NULL\n"
"               )\n";

        if (parameters["search_id"] > 0)
        {
            queryStr +=
"             AND " QUOTE(TRIGRAM_SEARCH ID) " = " + boost::lexical_cast<string>(parameters["search_id"]) + "\n";
        }

        queryStr +=
"             AND " QUOTE(TRIGRAM_SEARCH SEP "DELETE") " IS NULL\n"
"       )\n"
"     AND " QUOTE(TRIGRAM_FILTER SEP "DELETE") " IS NULL;";

        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Filter>> queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::Filter>>(queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Filter>> filPtrCol = queryRes.resultList();

        res = serialize(filPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode FilterResource::getFilter(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::Filter> filPtr = selectFilter(pathElements[1], grpId, m_session);

        res = serialize(filPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode FilterResource::getParametersListForFilters(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        string queryStr =
" SELECT fpa\n"
"   FROM " QUOTE("TR_FILTER_PARAMETER" SEP TRIGRAM_FILTER_PARAMETER) " fpa\n";

        if (parameters["type_id"] > 0)
        {
            queryStr +=
"   WHERE\n"
"     " QUOTE(TRIGRAM_FILTER_PARAMETER ID) " IN\n"
"       (\n"
"         SELECT " QUOTE ("TR_FILTER_PARAMETER" SEP TRIGRAM_FILTER_PARAMETER SEP TRIGRAM_FILTER_PARAMETER ID) "\n"
"           FROM " QUOTE("TJ" SEP TRIGRAM_FILTER_TYPE SEP TRIGRAM_FILTER_PARAMETER) "\n"
"           WHERE\n"
"             " QUOTE("TR_FILTER_TYPE" SEP TRIGRAM_FILTER_TYPE SEP TRIGRAM_FILTER_TYPE ID) " IN\n"
"               (\n"
"                 SELECT " QUOTE(TRIGRAM_FILTER_TYPE ID)
"                   FROM " QUOTE ("TR_FILTER_TYPE" SEP TRIGRAM_FILTER_TYPE) "\n"
"                   WHERE\n"
"                     " QUOTE(TRIGRAM_FILTER_TYPE ID) " = " + boost::lexical_cast<string>(parameters["type_id"]) + "\n" 
"                     AND " QUOTE(TRIGRAM_FILTER_TYPE SEP "DELETE") " IS NULL\n"
"               )\n"
"       )\n";
        }

        queryStr +=
"   ORDER BY " QUOTE(TRIGRAM_FILTER_PARAMETER ID)"\n";

        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::FilterParameter>> queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::FilterParameter>>(queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::FilterParameter>> srpPtrCol = queryRes.resultList();

        res = serialize(srpPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode FilterResource::getParametersForFilter(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::Filter> filPtr = selectFilter(pathElements[1], grpId, m_session);
        
        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::FilterParameterValue>> fpvPtrCol = m_session.find<Echoes::Dbo::FilterParameterValue>()
                .where(QUOTE(TRIGRAM_FILTER_PARAMETER_VALUE SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_FILTER ID SEP TRIGRAM_FILTER ID) " = ?").bind(filPtr.id());

        res = serialize(fpvPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode FilterResource::processGetRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    string nextElement = "";
    unsigned short indexPathElement = 1;
    vector<string> pathElements;
    map<string, long long> parameters;

    parameters["type_id"] = 0;
    parameters["plugin_id"] = 0;
    parameters["source_id"] = 0;
    parameters["search_id"] = 0;
    
    const string sRequest = processRequestParameters(request, pathElements, parameters);

    nextElement = getNextElementFromPath(indexPathElement, pathElements);
    if (nextElement.empty())
    {
        res = getFiltersList(grpId, responseMsg, pathElements, sRequest, parameters);
    }
    else if (nextElement.compare("parameters") == 0)
    {
        res = getParametersListForFilters(grpId, responseMsg, pathElements, sRequest, parameters);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.empty())
            {
                res = getFilter(grpId, responseMsg, pathElements);
            }
            else if (nextElement.compare("parameters") == 0)
            {
                res = getParametersForFilter(grpId, responseMsg, pathElements);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Filter Resource] bad nextElement";
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

EReturnCode FilterResource::postFilters(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    long long seaId;
    long long ftyId;
    int nbValue;
    int posKeyValue;

    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            seaId = result.get("search_id");
            ftyId = result.get("type_id");
            nbValue = result.get("nb_value").toNumber();
            posKeyValue = result.get("position_key_value").toNumber();
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
        const string err = "[Filter Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session, true);

            Wt::Dbo::ptr<Echoes::Dbo::FilterType> ftyPtr = m_session.find<Echoes::Dbo::FilterType>()
                    .where(QUOTE(TRIGRAM_FILTER_TYPE ID) " = ?").bind(ftyId)
                    .where(QUOTE(TRIGRAM_FILTER_TYPE SEP "DELETE") " IS NULL");
            if (!ftyPtr)
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, ftyPtr);
                return res;
            }
            
            Wt::Dbo::ptr<Echoes::Dbo::Search> seaPtr = SearchResource::selectSearch(seaId, grpId, m_session);
            if (!seaPtr)
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, seaPtr);
                return res;
            }
            
            Echoes::Dbo::Filter *newFil = new Echoes::Dbo::Filter();
            newFil->filterType = ftyPtr;
            newFil->search = seaPtr;
            newFil->nbValue = nbValue;
            newFil->posKeyValue = posKeyValue;

            Wt::Dbo::ptr<Echoes::Dbo::Filter> newFilPtr = m_session.add<Echoes::Dbo::Filter>(newFil);
            newFilPtr.flush();

            try
            {
                Wt::Json::Object result;
                Wt::Json::parse(sRequest, result);

                for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::FilterParameter> >::const_iterator it = ftyPtr->filterParameters.begin(); it != ftyPtr->filterParameters.end(); ++it)
                {
                    Echoes::Dbo::FilterParameterValue *newFpv = new Echoes::Dbo::FilterParameterValue;

                    newFpv->value = result.get(it->get()->name.toUTF8());
                    newFpv->name = it->get()->name;
//                    newFpv->filterParameterValueId.filterParameter= *it;
//                    newFpv->filterParameterValueId.filter = newFilPtr;
                    //TODO: Fix this name on DBO
                    newFpv->filteParameterValueId.filterParameter= *it;
                    newFpv->filteParameterValueId.filter = newFilPtr;

                    m_session.add<Echoes::Dbo::FilterParameterValue>(newFpv);
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

            res = serialize(newFilPtr, responseMsg, EReturnCode::CREATED);

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

EReturnCode FilterResource::processPostRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
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
        res = postFilters(grpId, responseMsg, pathElements, sRequest);
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Filter Resource] bad nextElement";
        responseMsg = httpCodeToJSON(res, err);
    }
    return res;
}

EReturnCode FilterResource::putFilters(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    if (sRequest.empty())
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Filter Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session, true);

            Wt::Dbo::ptr<Echoes::Dbo::Filter> fltPtr = m_session.find<Echoes::Dbo::Filter>()
                    .where(QUOTE(TRIGRAM_FILTER SEP "DELETE") " IS NULL")
                    .where(QUOTE(TRIGRAM_FILTER ID) " = ?").bind(pathElements[1]);

            if (!fltPtr)
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, fltPtr);
                return res;
            }
            
            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Plugin> >::const_iterator itPlugin = fltPtr.get()->search.get()->source.get()->plugins.begin();
            if (itPlugin->get()->group.id() != grpId)
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, fltPtr);
                return res;
            }

            try
            {                
                Wt::Json::Object result;
                Wt::Json::parse(sRequest, result);
                Wt::Dbo::ptr<Echoes::Dbo::FilterType> fltTypePtr = fltPtr.get()->filterType;
            
                if (result.contains("nb_value"))
                {
                    fltPtr.modify()->nbValue = result.get("nb_value").toNumber();
                }
                
                if (result.contains("position_key_value"))
                {
                    fltPtr.modify()->posKeyValue = result.get("position_key_value").toNumber();
                }

                for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::FilterParameter> >::const_iterator it = fltTypePtr->filterParameters.begin(); it != fltTypePtr->filterParameters.end(); ++it)
                {
                    Wt::Dbo::ptr<Echoes::Dbo::FilterParameterValue> fltParaValuePtr = m_session.find<Echoes::Dbo::FilterParameterValue>()
                        .where(QUOTE(TRIGRAM_FILTER_PARAMETER_VALUE SEP "DELETE") " IS NULL")
                        .where(QUOTE("FIL_ID_FIL_ID") " = ?").bind(fltPtr.id())
                        .where(QUOTE("FPA_ID_FPA_ID") " = ?").bind(it->id());
                                            
                    if(fltParaValuePtr && result.contains(it->get()->name.toUTF8()))
                    {
                        fltParaValuePtr.modify()->value = result.get(it->get()->name.toUTF8());
                    }
                }
                
                res = serialize(fltPtr, responseMsg);
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

EReturnCode FilterResource::processPutRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
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
        const string err = "[Filter Resource] bad nextElement";
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
                res = putFilters(grpId, responseMsg, pathElements, sRequest);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Filter Resource] bad nextElement";
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

EReturnCode FilterResource::deleteFilters(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try 
    {  
        Wt::Dbo::Transaction transaction(m_session, true);
           
        Wt::Dbo::ptr<Echoes::Dbo::Filter> filPtr = selectFilter(pathElements[1], grpId, m_session);

        if(filPtr)
        {
            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationData>> idaPtrCol = m_session.find<Echoes::Dbo::InformationData>()
                    .where(QUOTE(TRIGRAM_INFORMATION_DATA SEP TRIGRAM_FILTER SEP TRIGRAM_FILTER ID)" = ?").bind(pathElements[1])
                    .where(QUOTE(TRIGRAM_INFORMATION_DATA SEP "DELETE") " IS NULL");

            if (idaPtrCol.size() == 0)
            {
                filPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
                res = EReturnCode::NO_CONTENT;
            }
            else
            {
                res = EReturnCode::LOCKED;
                responseMsg = httpCodeToJSON(res, filPtr);
            }
        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = httpCodeToJSON(res, filPtr);
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

EReturnCode FilterResource::processDeleteRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
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
        const string err = "[Filter Resource] bad nextElement";
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
                res = deleteFilters(grpId, responseMsg, pathElements);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Filter Resource] bad nextElement";
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
