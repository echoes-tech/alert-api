/* 
 * API FilterResource
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

#include "FilterResource.h"

using namespace std;

FilterResource::FilterResource()
{
    m_parameters["type_id"] = 0;
    m_parameters["plugin_id"] = 0;
    m_parameters["source_id"] = 0;
    m_parameters["search_id"] = 0;
}

FilterResource::~FilterResource()
{
}

Wt::Dbo::ptr<Echoes::Dbo::Filter> FilterResource::selectFilter(const long long &filId, Echoes::Dbo::Session &session)
{
    return selectFilter(boost::lexical_cast<string>(filId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::Filter> FilterResource::selectFilter(const string &filId, Echoes::Dbo::Session &session)
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
"                                         " QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = " + boost::lexical_cast<string>(session.user()->organization.id()) +
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

EReturnCode FilterResource::getFiltersList(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session);
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
"                                         " QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = " + boost::lexical_cast<string>(m_session.user()->organization.id()) +  "\n";

        if (m_parameters["plugin_id"] > 0)
        {
            queryStr +=
"                                         AND " QUOTE(TRIGRAM_PLUGIN ID) " = " + boost::lexical_cast<string>(m_parameters["plugin_id"]) + "\n";
        }

        queryStr +=
"                                         AND " QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL\n"
"                                 )\n";

        if (m_parameters["source_id"] > 0)
        {
            queryStr +=
"                                 AND " QUOTE("T_SOURCE" SEP TRIGRAM_SOURCE SEP TRIGRAM_SOURCE ID) " = " + boost::lexical_cast<string>(m_parameters["source_id"]) + "\n";
        }

        queryStr +=
"                         )\n"
"                       AND " QUOTE(TRIGRAM_SOURCE SEP "DELETE") " IS NULL\n"
"               )\n";

        if (m_parameters["search_id"] > 0)
        {
            queryStr +=
"             AND " QUOTE(TRIGRAM_SEARCH ID) " = " + boost::lexical_cast<string>(m_parameters["search_id"]) + "\n";
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

EReturnCode FilterResource::getFilter(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::ptr<Echoes::Dbo::Filter> filPtr = selectFilter(m_pathElements[1], m_session);

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

EReturnCode FilterResource::getParametersList(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        string queryStr =
" SELECT fpa\n"
"   FROM " QUOTE("TR_FILTER_PARAMETER" SEP TRIGRAM_FILTER_PARAMETER) " fpa\n";

        if (m_parameters["type_id"] > 0)
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
"                     " QUOTE(TRIGRAM_FILTER_TYPE ID) " = " + boost::lexical_cast<string>(m_parameters["type_id"]) + "\n" 
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

EReturnCode FilterResource::getParametersListForFilter(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::ptr<Echoes::Dbo::Filter> filPtr = selectFilter(m_pathElements[1], m_session);
        
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

void FilterResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = getFiltersList(responseMsg);
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
                m_statusCode = getFilter(responseMsg);
            }
            else if (nextElement.compare("parameters") == 0)
            {
                m_statusCode = getParametersListForFilter(responseMsg);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                const string err = "[Filter Resource] bad nextElement";
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

EReturnCode FilterResource::postFilter(string& responseMsg, const string& sRequest)
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
            nbValue = result.get("nb_value");
            posKeyValue = result.get("position_key_value");
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
            Wt::Dbo::Transaction transaction(m_session);

            Wt::Dbo::ptr<Echoes::Dbo::FilterType> ftyPtr = m_session.find<Echoes::Dbo::FilterType>()
                    .where(QUOTE(TRIGRAM_FILTER_TYPE ID) " = ?").bind(ftyId)
                    .where(QUOTE(TRIGRAM_FILTER_TYPE SEP "DELETE") " IS NULL");
            if (!ftyPtr)
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, ftyPtr);
                return res;
            }
            
            Wt::Dbo::ptr<Echoes::Dbo::Search> seaPtr = SearchResource::selectSearch(seaId, m_session);
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

                for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::FilterParameter> >::const_iterator it = ftyPtr->filterParameters.begin(); it != ftyPtr->filterParameters.end(); it++)
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

void FilterResource::processPostRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = postFilter(responseMsg, m_requestData);
    }
    else
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
        const string err = "[Filter Resource] bad nextElement";
        responseMsg = httpCodeToJSON(m_statusCode, err);
    }

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}

EReturnCode FilterResource::putFilter(string &responseMsg, const string &sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    return res;
}

void FilterResource::processPutRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
        const string err = "[Filter Resource] bad nextElement";
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
                m_statusCode = putFilter(responseMsg, m_requestData);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                const string err = "[Filter Resource] bad nextElement";
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

EReturnCode FilterResource::deleteFilter(string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try 
    {  
        Wt::Dbo::Transaction transaction(m_session);
           
        Wt::Dbo::ptr<Echoes::Dbo::Filter> filPtr = selectFilter(m_pathElements[1], m_session);

        if(filPtr)
        {
            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationData>> idaPtrCol = m_session.find<Echoes::Dbo::InformationData>()
                    .where(QUOTE(TRIGRAM_INFORMATION_DATA SEP TRIGRAM_FILTER SEP TRIGRAM_FILTER ID)" = ?").bind(m_pathElements[1])
                    .where(QUOTE(TRIGRAM_INFORMATION_DATA SEP "DELETE") " IS NULL");

            if (idaPtrCol.size() == 0)
            {
                filPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
                res = EReturnCode::NO_CONTENT;
            }
            else
            {
                res = EReturnCode::CONFLICT;
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

void FilterResource::processDeleteRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
        const string err = "[Filter Resource] bad nextElement";
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
                m_statusCode = deleteFilter(responseMsg);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                const string err = "[Filter Resource] bad nextElement";
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

