/* 
 * API SearchResource
 * @author ECHOES Technologies (FPO)
 * @date 09/12/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "SearchResource.h"

using namespace std;

SearchResource::SearchResource()
{
    m_parameters["type_id"] = 0;
    m_parameters["plugin_id"] = 0;
    m_parameters["source_id"] = 0;
}

SearchResource::~SearchResource()
{
}

Wt::Dbo::ptr<Echoes::Dbo::Search> SearchResource::selectSearch(const long long &seaId, const long long &orgId, Echoes::Dbo::Session &session)
{
    return selectSearch(boost::lexical_cast<string>(seaId), boost::lexical_cast<string>(orgId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::Search> SearchResource::selectSearch(const string &seaId, const long long &orgId, Echoes::Dbo::Session &session)
{
    return selectSearch(seaId, boost::lexical_cast<string>(orgId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::Search> SearchResource::selectSearch(const string &seaId, const string &orgId, Echoes::Dbo::Session &session)
{
    const string queryStr =
" SELECT sea"
"   FROM " QUOTE("T_SEARCH" SEP TRIGRAM_SEARCH) " sea"
"   WHERE"
"     " QUOTE(TRIGRAM_SEARCH SEP TRIGRAM_SOURCE SEP TRIGRAM_SOURCE ID) " IN"
"       ("
"         SELECT " QUOTE(TRIGRAM_SOURCE ID)
"           FROM " QUOTE("T_SOURCE" SEP TRIGRAM_SOURCE)
"             WHERE"
"               " QUOTE(TRIGRAM_SOURCE ID) " IN"
"                 ("
"                   SELECT " QUOTE("T_SOURCE" SEP TRIGRAM_SOURCE SEP TRIGRAM_SOURCE ID)
"                     FROM " QUOTE("TJ" SEP TRIGRAM_PLUGIN SEP TRIGRAM_SOURCE)
"                       WHERE " QUOTE("T_PLUGIN" SEP TRIGRAM_PLUGIN SEP TRIGRAM_PLUGIN ID) " IN"
"                         ("
"                           SELECT " QUOTE(TRIGRAM_PLUGIN ID)
"                             FROM " QUOTE("T_PLUGIN" SEP TRIGRAM_PLUGIN)
"                               WHERE"
"                                 " QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = " + orgId +
"                                 AND " QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL"
"                         )"
"                 )"
"               AND " QUOTE(TRIGRAM_SOURCE SEP "DELETE") " IS NULL"
"       )"
"     AND " QUOTE(TRIGRAM_SEARCH ID) " = " + seaId +
"     AND " QUOTE(TRIGRAM_SEARCH SEP "DELETE") " IS NULL"
"   LIMIT 1";

    Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Search>> queryRes = session.query<Wt::Dbo::ptr<Echoes::Dbo::Search>>(queryStr);

    return queryRes.resultValue();
}

EReturnCode SearchResource::getSearchsList(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session);
        string queryStr =
" SELECT sea"
"   FROM " QUOTE("T_SEARCH" SEP TRIGRAM_SEARCH) " sea"
"   WHERE"
"     " QUOTE(TRIGRAM_SEARCH SEP TRIGRAM_SOURCE SEP TRIGRAM_SOURCE ID) " IN"
"       ("
"         SELECT " QUOTE(TRIGRAM_SOURCE ID)
"           FROM " QUOTE("T_SOURCE" SEP TRIGRAM_SOURCE)
"             WHERE"
"               " QUOTE(TRIGRAM_SOURCE ID) " IN"
"                 ("
"                   SELECT " QUOTE("T_SOURCE" SEP TRIGRAM_SOURCE SEP TRIGRAM_SOURCE ID)
"                     FROM " QUOTE("TJ" SEP TRIGRAM_PLUGIN SEP TRIGRAM_SOURCE)
"                       WHERE " QUOTE("T_PLUGIN" SEP TRIGRAM_PLUGIN SEP TRIGRAM_PLUGIN ID) " IN"
"                         ("
"                           SELECT " QUOTE(TRIGRAM_PLUGIN ID)
"                             FROM " QUOTE("T_PLUGIN" SEP TRIGRAM_PLUGIN)
"                               WHERE"
"                                 " QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = " + boost::lexical_cast<string>(m_organization);

        if (m_parameters["plugin_id"] > 0)
        {
            queryStr +=
"                                 AND " QUOTE(TRIGRAM_PLUGIN ID) " = " + boost::lexical_cast<string>(m_parameters["plugin_id"]);
        }

        queryStr +=
"                                 AND " QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL"
"                         )";

        if (m_parameters["source_id"] > 0)
        {
            queryStr +=
"                         AND " QUOTE("T_SOURCE" SEP TRIGRAM_SOURCE SEP TRIGRAM_SOURCE ID) " = " + boost::lexical_cast<string>(m_parameters["source_id"]);
        }

        queryStr +=
"                 )"
"               AND " QUOTE(TRIGRAM_SOURCE SEP "DELETE") " IS NULL"
"       )"
"     AND " QUOTE(TRIGRAM_SEARCH SEP "DELETE") " IS NULL";

        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Search>> queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::Search>>(queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Search>> seaPtrCol = queryRes.resultList();

        res = serialize(seaPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode SearchResource::getSearch(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::ptr<Echoes::Dbo::Search> seaPtr = selectSearch(m_pathElements[1], m_organization, m_session);

        res = serialize(seaPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode SearchResource::getParametersList(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        string queryStr =
" SELECT sep\n"
"   FROM " QUOTE("TR_SEARCH_PARAMETER" SEP TRIGRAM_SEARCH_PARAMETER) " sep\n";

        if (m_parameters["type_id"] > 0)
        {
            queryStr +=
"   WHERE\n"
"     " QUOTE(TRIGRAM_SEARCH_PARAMETER ID) " IN\n"
"       (\n"
"         SELECT " QUOTE ("TR_SEARCH_PARAMETER" SEP TRIGRAM_SEARCH_PARAMETER SEP TRIGRAM_SEARCH_PARAMETER ID) "\n"
"           FROM " QUOTE("TJ" SEP TRIGRAM_SEARCH_TYPE SEP TRIGRAM_SEARCH_PARAMETER) "\n"
"           WHERE\n"
"             " QUOTE("TR_SEARCH_TYPE" SEP TRIGRAM_SEARCH_TYPE SEP TRIGRAM_SEARCH_TYPE ID) " IN\n"
"               (\n"
"                 SELECT " QUOTE(TRIGRAM_SEARCH_TYPE ID)
"                   FROM " QUOTE ("TR_SEARCH_TYPE" SEP TRIGRAM_SEARCH_TYPE) "\n"
"                   WHERE\n"
"                     " QUOTE(TRIGRAM_SEARCH_TYPE ID) " = " + boost::lexical_cast<string>(m_parameters["type_id"]) + "\n" 
"                     AND " QUOTE(TRIGRAM_SEARCH_TYPE SEP "DELETE") " IS NULL\n"
"               )\n"
"       )\n";
        }

        queryStr +=
"   ORDER BY " QUOTE(TRIGRAM_SEARCH_PARAMETER ID)"\n";

        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::SearchParameter>> queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::SearchParameter>>(queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SearchParameter>> srpPtrCol = queryRes.resultList();

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

EReturnCode SearchResource::getParametersListForSearch(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::ptr<Echoes::Dbo::Search> seaPtr = selectSearch(m_pathElements[1], m_organization, m_session);
        
        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SearchParameterValue>> sevPtrCol = m_session.find<Echoes::Dbo::SearchParameterValue>()
                .where(QUOTE(TRIGRAM_SEARCH_PARAMETER_VALUE SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_SEARCH ID SEP TRIGRAM_SEARCH ID) " = ?").bind(seaPtr.id());

        res = serialize(sevPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

void SearchResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = getSearchsList(responseMsg);
    }
    else if (nextElement.compare("parameters") == 0)
    {
        m_statusCode = getParametersList(responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();
            if (nextElement.empty())
            {
                m_statusCode = getSearch(responseMsg);
            }
            else if (nextElement.compare("parameters") == 0)
            {
                m_statusCode = getParametersListForSearch(responseMsg);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                const string err = "[Search Resource] bad nextElement";
                responseMsg = httpCodeToJSON(m_statusCode, err);
            }
        }
        catch (boost::bad_lexical_cast const& e)
        {
            m_statusCode = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(m_statusCode, e);
        }
    }

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}

EReturnCode SearchResource::postSearch(string& responseMsg, const string& sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    long long srcId;
    long long styId;
    int period;

    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            srcId = result.get("source_id");
            styId = result.get("type_id");
            period = result.get("period");
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
        const string err = "[Search Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session);

            Wt::Dbo::ptr<Echoes::Dbo::SearchType> styPtr = m_session.find<Echoes::Dbo::SearchType>()
                    .where(QUOTE(TRIGRAM_SEARCH_TYPE ID) " = ?").bind(styId)
                    .where(QUOTE(TRIGRAM_SEARCH_TYPE SEP "DELETE") " IS NULL");
            if (!styPtr)
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, styPtr);
                return res;
            }
            
            Wt::Dbo::ptr<Echoes::Dbo::Source> srcPtr = SourceResource::selectSource(srcId, m_organization, m_session);
            if (!srcPtr)
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, srcPtr);
                return res;
            }
            
            Echoes::Dbo::Search *newSea = new Echoes::Dbo::Search();
            newSea->searchType = styPtr;
            newSea->source = srcPtr;
            newSea->period = period;

            Wt::Dbo::ptr<Echoes::Dbo::Search> newSeaPtr = m_session.add<Echoes::Dbo::Search>(newSea);
            newSeaPtr.flush();

            try
            {
                Wt::Json::Object result;
                Wt::Json::parse(sRequest, result);

                for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SearchParameter> >::const_iterator it = styPtr->searchParameters.begin(); it != styPtr->searchParameters.end(); it++)
                {
                    Echoes::Dbo::SearchParameterValue *newSev = new Echoes::Dbo::SearchParameterValue;

                    newSev->value = result.get(it->get()->name.toUTF8());
                    newSev->name = it->get()->name;
                    newSev->searchParameterValueId.searchParameter= *it;
                    newSev->searchParameterValueId.search = newSeaPtr;

                    m_session.add<Echoes::Dbo::SearchParameterValue>(newSev);
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

            res = serialize(newSeaPtr, responseMsg, EReturnCode::CREATED);

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

void SearchResource::processPostRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = postSearch(responseMsg, m_requestData);
    }
    else
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
        const string err = "[Search Resource] bad nextElement";
        responseMsg = httpCodeToJSON(m_statusCode, err);
    }

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}

EReturnCode SearchResource::putSearch(string &responseMsg, const string &sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    return res;
}

void SearchResource::processPutRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
        const string err = "[Search Resource] bad nextElement";
        responseMsg = httpCodeToJSON(m_statusCode, err);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();

            if (nextElement.empty())
            {
                m_statusCode = putSearch(responseMsg, m_requestData);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                const string err = "[Search Resource] bad nextElement";
                responseMsg = httpCodeToJSON(m_statusCode, err);
            }
        }
        catch (boost::bad_lexical_cast const& e)
        {
            m_statusCode = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(m_statusCode, e);
        }
    }

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}

EReturnCode SearchResource::deleteSearch(string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try 
    {  
        Wt::Dbo::Transaction transaction(m_session);
           
        Wt::Dbo::ptr<Echoes::Dbo::Search> seaPtr = selectSearch(m_pathElements[1], m_organization, m_session);

        if(seaPtr)
        {
            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Filter>> filPtrCol = m_session.find<Echoes::Dbo::Filter>()
                    .where(QUOTE(TRIGRAM_FILTER SEP TRIGRAM_SEARCH SEP TRIGRAM_SEARCH ID)" = ?").bind(m_pathElements[1])
                    .where(QUOTE(TRIGRAM_FILTER SEP "DELETE") " IS NULL");

            if (filPtrCol.size() == 0)
            {
                seaPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
                res = EReturnCode::NO_CONTENT;
            }
            else
            {
                res = EReturnCode::CONFLICT;
                responseMsg = httpCodeToJSON(res, seaPtr);
            } 
        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = httpCodeToJSON(res, seaPtr);
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

void SearchResource::processDeleteRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
        const string err = "[Search Resource] bad nextElement";
        responseMsg = httpCodeToJSON(m_statusCode, err);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();

            if (nextElement.empty())
            {
                m_statusCode = deleteSearch(responseMsg);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                const string err = "[Search Resource] bad nextElement";
                responseMsg = httpCodeToJSON(m_statusCode, err);
            }
        }
        catch (boost::bad_lexical_cast const& e)
        {
            m_statusCode = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(m_statusCode, e);
        }
    }

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}

