/* 
 * API SourceResource
 * @author ECHOES Technologies (FPO)
 * @date 08/12/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "SourceResource.h"

using namespace std;

SourceResource::SourceResource()
{
    m_parameters["plugin_id"] = 0;
}

SourceResource::~SourceResource()
{
}

Wt::Dbo::ptr<Echoes::Dbo::Source> SourceResource::selectSource(const long long &srcId, Echoes::Dbo::Session &session)
{
    return selectSource(boost::lexical_cast<string>(srcId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::Source> SourceResource::selectSource(const string &srcId, Echoes::Dbo::Session &session)
{
    const string queryStr =
" SELECT src"
"   FROM " QUOTE("T_SOURCE" SEP TRIGRAM_SOURCE) " src"
"   WHERE"
"     " QUOTE(TRIGRAM_SOURCE ID) " IN"
"       ("
"         SELECT " QUOTE("T_SOURCE" SEP TRIGRAM_SOURCE SEP TRIGRAM_SOURCE ID)
"           FROM " QUOTE("TJ" SEP TRIGRAM_PLUGIN SEP TRIGRAM_SOURCE)
"           WHERE " QUOTE("T_PLUGIN" SEP TRIGRAM_PLUGIN SEP TRIGRAM_PLUGIN ID) " IN"
"             ("
"               SELECT " QUOTE(TRIGRAM_PLUGIN ID)
"                 FROM " QUOTE("T_PLUGIN" SEP TRIGRAM_PLUGIN)
"                 WHERE"
"                   " QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = " + boost::lexical_cast<string>(session.user()->organization.id()) +
"                   AND " QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL"
"             )"
"             AND " QUOTE("T_SOURCE" SEP TRIGRAM_SOURCE SEP TRIGRAM_SOURCE ID) " = " + srcId +
"       )"
"     AND " QUOTE(TRIGRAM_SOURCE SEP "DELETE") " IS NULL"
"   LIMIT 1";

    Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Source>> queryRes = session.query<Wt::Dbo::ptr<Echoes::Dbo::Source>>(queryStr);

    return queryRes.resultValue();
}

EReturnCode SourceResource::getSourcesList(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session);
        string queryStr =
" SELECT src"
"   FROM " QUOTE("T_SOURCE" SEP TRIGRAM_SOURCE) " src"
"   WHERE"
"     " QUOTE(TRIGRAM_SOURCE ID) " IN"
"       ("
"         SELECT " QUOTE("T_SOURCE" SEP TRIGRAM_SOURCE SEP TRIGRAM_SOURCE ID)
"           FROM " QUOTE("TJ" SEP TRIGRAM_PLUGIN SEP TRIGRAM_SOURCE)
"           WHERE " QUOTE("T_PLUGIN" SEP TRIGRAM_PLUGIN SEP TRIGRAM_PLUGIN ID) " IN"
"             ("
"               SELECT " QUOTE(TRIGRAM_PLUGIN ID)
"                 FROM " QUOTE("T_PLUGIN" SEP TRIGRAM_PLUGIN)
"                 WHERE"
"                   " QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = " + boost::lexical_cast<string>(m_session.user()->organization.id());

        if (m_parameters["plugin_id"] > 0)
        {
            queryStr +=
"                   AND " QUOTE(TRIGRAM_PLUGIN ID) " = " + boost::lexical_cast<string>(m_parameters["plugin_id"]);
        }

        queryStr +=
"                   AND " QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL"
"             )"
"       )"
"     AND " QUOTE(TRIGRAM_SOURCE SEP "DELETE") " IS NULL";

        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Source>> queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::Source>>(queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Source>> srcPtrCol = queryRes.resultList();

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

EReturnCode SourceResource::getSource(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::ptr<Echoes::Dbo::Source> srcPtr = selectSource(m_pathElements[1], m_session);

        res = serialize(srcPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

void SourceResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = getSourcesList(responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();
            if (nextElement.empty())
            {
                m_statusCode = getSource(responseMsg);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                const string err = "[Source Resource] bad nextElement";
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

EReturnCode SourceResource::postSource(string& responseMsg, const string& sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    long long adoId;
    long long plgId;

    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            adoId = result.get("addon_id");
            plgId = result.get("plugin_id");
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
        const string err = "[Source Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }
    
    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session);

            Wt::Dbo::ptr<Echoes::Dbo::Plugin> plgPtr = m_session.find<Echoes::Dbo::Plugin>()
                    .where(QUOTE(TRIGRAM_PLUGIN ID) " = ?").bind(plgId)
                    .where(QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL")
                    .where(QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(m_session.user()->organization.id());
            if (!plgPtr)
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, plgPtr);
                return res;
            }

            Wt::Dbo::ptr<Echoes::Dbo::Addon> adoPtr = m_session.find<Echoes::Dbo::Addon>()
                    .where(QUOTE(TRIGRAM_ADDON ID) " = ?").bind(adoId)
                    .where(QUOTE(TRIGRAM_ADDON SEP "DELETE") " IS NULL");
            if (!adoPtr)
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, adoPtr);
                return res;
            }
            
            Echoes::Dbo::Source *newSrc = new Echoes::Dbo::Source();
            newSrc->addon = adoPtr;

            Wt::Dbo::ptr<Echoes::Dbo::Source> newSrcPtr = m_session.add<Echoes::Dbo::Source>(newSrc);
            newSrcPtr.flush();

            try
            {
                Wt::Json::Object result;
                Wt::Json::parse(sRequest, result);

                for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SourceParameter> >::const_iterator it = adoPtr->sourceParameters.begin(); it != adoPtr->sourceParameters.end(); it++)
                {
                    Echoes::Dbo::SourceParameterValue *newSpv = new Echoes::Dbo::SourceParameterValue;

                    newSpv->value = result.get(it->get()->name.toUTF8());
                    newSpv->name = it->get()->name;
                    newSpv->pk.sourceParameter = *it;
                    newSpv->pk.source = newSrcPtr;
                    m_session.add<Echoes::Dbo::SourceParameterValue>(newSpv);
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
            
            plgPtr.modify()->sources.insert(newSrcPtr);

            res = serialize(newSrcPtr, responseMsg, EReturnCode::CREATED);

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

void SourceResource::processPostRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = postSource(responseMsg, m_requestData);
    }
    else
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
        const string err = "[Source Resource] bad nextElement";
        responseMsg = httpCodeToJSON(m_statusCode, err);
    }

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}

EReturnCode SourceResource::putSource(string &responseMsg, const string &sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    return res;
}

void SourceResource::processPutRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
        const string err = "[Source Resource] bad nextElement";
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
                m_statusCode = putSource(responseMsg, m_requestData);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                const string err = "[Source Resource] bad nextElement";
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

EReturnCode SourceResource::deleteSource(string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try 
    {  
        Wt::Dbo::Transaction transaction(m_session);
           
        Wt::Dbo::ptr<Echoes::Dbo::Source> srcPtr = selectSource(m_pathElements[1], m_session);

        if(srcPtr)
        {
            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Search>> seaPtrCol = m_session.find<Echoes::Dbo::Search>()
                    .where(QUOTE(TRIGRAM_SEARCH SEP TRIGRAM_SOURCE SEP TRIGRAM_SOURCE ID)" = ?").bind(m_pathElements[1])
                    .where(QUOTE(TRIGRAM_SEARCH SEP "DELETE") " IS NULL");

            if (seaPtrCol.size() == 0)
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

void SourceResource::processDeleteRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
        const string err = "[Source Resource] bad nextElement";
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
                m_statusCode = deleteSource(responseMsg);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                const string err = "[Source Resource] bad nextElement";
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

