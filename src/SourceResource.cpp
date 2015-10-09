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

SourceResource::SourceResource(Echoes::Dbo::Session& session) : PublicApiResource::PublicApiResource(session)
{
}

SourceResource::~SourceResource()
{
}

Wt::Dbo::ptr<Echoes::Dbo::Source> SourceResource::selectSource(const long long &srcId, const long long &orgId, Echoes::Dbo::Session &session)
{
    return selectSource(boost::lexical_cast<string>(srcId), boost::lexical_cast<string>(orgId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::Source> SourceResource::selectSource(const string &srcId, const long long &orgId, Echoes::Dbo::Session &session)
{
    return selectSource(srcId, boost::lexical_cast<string>(orgId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::Source> SourceResource::selectSource(const string &srcId, const string &orgId, Echoes::Dbo::Session &session)
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
            "                   " QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = " + orgId +
            "                   AND " QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL"
            "             )"
            "             AND " QUOTE("T_SOURCE" SEP TRIGRAM_SOURCE SEP TRIGRAM_SOURCE ID) " = " + srcId +
            "       )"
            "     AND " QUOTE(TRIGRAM_SOURCE SEP "DELETE") " IS NULL"
            "   LIMIT 1";

    Wt::Dbo::Query<Wt::Dbo::ptr < Echoes::Dbo::Source>> queryRes = session.query<Wt::Dbo::ptr < Echoes::Dbo::Source >> (queryStr);

    return queryRes.resultValue();
}

EReturnCode SourceResource::getSourcesList(map<string, long long> &parameters, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);
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
                "                   " QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = " + boost::lexical_cast<string>(orgId);

        if (parameters["plugin_id"] > 0)
        {
            queryStr +=
                    "                   AND " QUOTE(TRIGRAM_PLUGIN ID) " = " + boost::lexical_cast<string>(parameters["plugin_id"]);
        }

        queryStr +=
                "                   AND " QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL"
                "             )"
                "       )"
                "     AND " QUOTE(TRIGRAM_SOURCE SEP "DELETE") " IS NULL";

        Wt::Dbo::Query<Wt::Dbo::ptr < Echoes::Dbo::Source>> queryRes = m_session.query<Wt::Dbo::ptr < Echoes::Dbo::Source >> (queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr < Echoes::Dbo::Source>> srcPtrCol = queryRes.resultList();

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

EReturnCode SourceResource::getSource(const std::vector<std::string> &pathElements, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::Source> srcPtr = selectSource(pathElements[1], orgId, m_session);

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

EReturnCode SourceResource::getParametersList(map<string, long long> &parameters, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        string queryStr =
                " SELECT srp\n"
                "   FROM " QUOTE("TR_SOURCE_PARAMETER" SEP TRIGRAM_SOURCE_PARAMETER) " srp\n";

        if (parameters["addon_id"] > 0)
        {
            queryStr +=
                    "   WHERE\n"
                    "     " QUOTE(TRIGRAM_SOURCE_PARAMETER ID) " IN\n"
                    "       (\n"
                    "         SELECT " QUOTE("TR_SOURCE_PARAMETER" SEP TRIGRAM_SOURCE_PARAMETER SEP TRIGRAM_SOURCE_PARAMETER ID) "\n"
                    "           FROM " QUOTE("TJ" SEP TRIGRAM_ADDON SEP TRIGRAM_SOURCE_PARAMETER) "\n"
                    "           WHERE\n"
                    "             " QUOTE("TR_ADDON" SEP TRIGRAM_ADDON SEP TRIGRAM_ADDON ID) " IN\n"
                    "               (\n"
                    "                 SELECT " QUOTE(TRIGRAM_ADDON ID)
                    "                   FROM " QUOTE("TR_ADDON" SEP TRIGRAM_ADDON) "\n"
                    "                   WHERE\n"
                    "                     " QUOTE(TRIGRAM_ADDON ID) " = " + boost::lexical_cast<string>(parameters["addon_id"]) + "\n"
                    "                     AND " QUOTE(TRIGRAM_ADDON SEP "DELETE") " IS NULL\n"
                    "               )\n"
                    "       )\n";
        }

        queryStr +=
                "   ORDER BY " QUOTE(TRIGRAM_SOURCE_PARAMETER ID)"\n";

        Wt::Dbo::Query<Wt::Dbo::ptr < Echoes::Dbo::SourceParameter>> queryRes = m_session.query<Wt::Dbo::ptr < Echoes::Dbo::SourceParameter >> (queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr < Echoes::Dbo::SourceParameter>> srpPtrCol = queryRes.resultList();

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

EReturnCode SourceResource::getParametersListForSource(const std::vector<std::string> &pathElements, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::Source> srcPtr = selectSource(pathElements[1], orgId, m_session);

        Wt::Dbo::collection<Wt::Dbo::ptr < Echoes::Dbo::SourceParameterValue>> spvPtrCol = m_session.find<Echoes::Dbo::SourceParameterValue>()
                .where(QUOTE(TRIGRAM_SOURCE_PARAMETER_VALUE SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_SOURCE ID SEP TRIGRAM_SOURCE ID) " = ?").bind(srcPtr.id());

        res = serialize(spvPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode SourceResource::processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    string nextElement = "";
    unsigned short indexPathElement = 1;
    vector<string> pathElements;
    map<string, long long> parameters;

    parameters["addon_id"] = 0;
    parameters["plugin_id"] = 0;

    const string sRequest = processRequestParameters(request, pathElements, parameters);

    nextElement = getNextElementFromPath(indexPathElement, pathElements);
    if (nextElement.empty())
    {
        res = getSourcesList(parameters, orgId, responseMsg);
    }
    else if (nextElement.compare("parameters") == 0)
    {
        res = getParametersList(parameters, orgId, responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.empty())
            {
                res = getSource(pathElements, orgId, responseMsg);
            }
            else if (nextElement.compare("parameters") == 0)
            {
                res = getParametersListForSource(pathElements, orgId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Source Resource] bad nextElement";
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

EReturnCode SourceResource::postSource(const string& sRequest, const long long &orgId, string& responseMsg)
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
            Wt::Dbo::Transaction transaction(m_session, true);

            Wt::Dbo::ptr<Echoes::Dbo::Plugin> plgPtr = m_session.find<Echoes::Dbo::Plugin>()
                    .where(QUOTE(TRIGRAM_PLUGIN ID) " = ?").bind(plgId)
                    .where(QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL")
                    .where(QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(orgId);
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

                for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SourceParameter> >::const_iterator it = adoPtr->sourceParameters.begin(); it != adoPtr->sourceParameters.end(); ++it)
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

EReturnCode SourceResource::processPostRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        res = postSource(sRequest, orgId, responseMsg);
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Source Resource] bad nextElement";
        responseMsg = httpCodeToJSON(res, err);
    }

    return res;
}

EReturnCode SourceResource::putSource(const std::vector<std::string> &pathElements, const string& sRequest, const long long &orgId, string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    if (sRequest.empty())
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Source Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session, true);

            Wt::Dbo::ptr<Echoes::Dbo::Source> srcPtr = m_session.find<Echoes::Dbo::Source>()
                    .where(QUOTE(TRIGRAM_SOURCE SEP "DELETE") " IS NULL")
                    .where(QUOTE(TRIGRAM_SOURCE ID) " = ?").bind(pathElements[1]);

            if (!srcPtr)
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, srcPtr);
                return res;
            }
            
            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Plugin> >::const_iterator itPlugin = srcPtr.get()->plugins.begin();
            if (itPlugin->get()->organization.id() != orgId)
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, srcPtr);
                return res;
            }

            try
            {                
                Wt::Json::Object result;
                Wt::Json::parse(sRequest, result);
                Wt::Dbo::ptr<Echoes::Dbo::Addon> adoPtr = srcPtr.get()->addon;

                for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SourceParameter> >::const_iterator it = adoPtr->sourceParameters.begin(); it != adoPtr->sourceParameters.end(); ++it)
                {
                    Wt::Dbo::ptr<Echoes::Dbo::SourceParameterValue> srcParaValuePtr = m_session.find<Echoes::Dbo::SourceParameterValue>()
                        .where(QUOTE(TRIGRAM_SOURCE_PARAMETER_VALUE SEP "DELETE") " IS NULL")
                        .where(QUOTE("SRC_ID_SRC_ID") " = ?").bind(srcPtr.id())
                        .where(QUOTE("SRP_ID_SRP_ID") " = ?").bind(it->id());
                                            
                    if(srcParaValuePtr && result.contains(it->get()->name.toUTF8()))
                    {
                        srcParaValuePtr.modify()->value = result.get(it->get()->name.toUTF8());
                    }
                }
                
                res = serialize(srcPtr, responseMsg);
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

EReturnCode SourceResource::processPutRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        const string err = "[Source Resource] bad nextElement";
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
                res = putSource(pathElements, sRequest, orgId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Source Resource] bad nextElement";
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

EReturnCode SourceResource::deleteSource(const std::vector<std::string> &pathElements, const long long &orgId, string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::Source> srcPtr = selectSource(pathElements[1], orgId, m_session);

        if (srcPtr)
        {
            Wt::Dbo::collection<Wt::Dbo::ptr < Echoes::Dbo::Search>> seaPtrCol = m_session.find<Echoes::Dbo::Search>()
                    .where(QUOTE(TRIGRAM_SEARCH SEP TRIGRAM_SOURCE SEP TRIGRAM_SOURCE ID)" = ?").bind(pathElements[1])
                    .where(QUOTE(TRIGRAM_SEARCH SEP "DELETE") " IS NULL");

            if (seaPtrCol.size() == 0)
            {
                srcPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
                res = EReturnCode::NO_CONTENT;
            }
            else
            {
                res = EReturnCode::LOCKED;
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

EReturnCode SourceResource::processDeleteRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        const string err = "[Source Resource] bad nextElement";
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
                res = deleteSource(pathElements, orgId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Source Resource] bad nextElement";
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

