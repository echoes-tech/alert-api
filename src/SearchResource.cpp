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
#include "SourceResource.h"

using namespace std;

SearchResource::SearchResource()
{
    m_parameters["plugin"] = 0;
}

SearchResource::~SearchResource()
{
}

Wt::Dbo::ptr<Echoes::Dbo::Search> SearchResource::selectSearch(const long long &seaId, Echoes::Dbo::Session &session)
{
    return selectSearch(boost::lexical_cast<string>(seaId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::Search> SearchResource::selectSearch(const string &seaId, Echoes::Dbo::Session &session)
{
    const string queryStr =
" SELECT sea"
"   FROM " QUOTE("T_SEARCH" SEP TRIGRAM_SEARCH) " sea"
"   WHERE"
"     " QUOTE(TRIGRAM_SEARCH SEP TRIGRAM_SEARCH SEP TRIGRAM_SOURCE ID) " IN"
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
"                               WHERE " QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = " + boost::lexical_cast<string>(session.user()->organization.id()) +
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
"     " QUOTE(TRIGRAM_SEARCH SEP TRIGRAM_SEARCH SEP TRIGRAM_SOURCE ID) " IN"
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
"                               WHERE " QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = " + boost::lexical_cast<string>(m_session.user()->organization.id());

        if (m_parameters["type"] > 0)
        {
            queryStr +=
"                                 AND " QUOTE(TRIGRAM_PLUGIN ID) " = " + boost::lexical_cast<string>(m_parameters["plugin"]);
        }

        queryStr +=
"                                 AND " QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL"
"                         )"
"                 )"
"               AND " QUOTE(TRIGRAM_SOURCE SEP "DELETE") " IS NULL"
"       )"
"     AND " QUOTE(TRIGRAM_SEARCH SEP "DELETE") " IS NULL";

        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Search>> queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::Search>>(queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Search>> srcPtrCol = queryRes.resultList();

        res = serialize(srcPtrCol, responseMsg);

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

        Wt::Dbo::ptr<Echoes::Dbo::Search> seaPtr = selectSearch(m_pathElements[1], m_session);

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

void SearchResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = getSearchsList(responseMsg);
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
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                responseMsg = httpCodeToJSON(m_statusCode, "");
            }
        }
        catch (boost::bad_lexical_cast const& e)
        {
            m_statusCode = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(m_statusCode, e);
        }
    }

    response.setStatus(this->m_statusCode);
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
        responseMsg = httpCodeToJSON(res, "");
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
            
            Wt::Dbo::ptr<Echoes::Dbo::Source> srcPtr = SourceResource::selectSource(srcId, m_session);
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
        responseMsg = httpCodeToJSON(m_statusCode, "");
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
        responseMsg = httpCodeToJSON(m_statusCode, "");
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
                responseMsg = httpCodeToJSON(m_statusCode, "");
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
           
        Wt::Dbo::ptr<Echoes::Dbo::Search> srcPtr = selectSearch(m_pathElements[1], m_session);

        if(srcPtr)
        {
            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Filter>> filPtrCol = m_session.find<Echoes::Dbo::Filter>()
                    .where(QUOTE(TRIGRAM_FILTER SEP TRIGRAM_SEARCH SEP TRIGRAM_SEARCH ID)" = ?").bind(m_pathElements[1])
                    .where(QUOTE(TRIGRAM_FILTER SEP "DELETE") " IS NULL");

            if (filPtrCol.size() == 0)
            {
                srcPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
                res = EReturnCode::NO_CONTENT;
            }
            else
            {
                res = EReturnCode::CONFLICT;
                responseMsg = httpCodeToJSON(res, srcPtr);
            } 
        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = httpCodeToJSON(res, srcPtr);
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
        responseMsg = httpCodeToJSON(m_statusCode, "");
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
                responseMsg = httpCodeToJSON(m_statusCode, "");
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

