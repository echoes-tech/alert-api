/* 
 * API PluginResource
 * @author ECHOES Technologies (GDR)
 * @date 10/02/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "PluginResource.h"

using namespace std;

PluginResource::PluginResource(Echoes::Dbo::Session& session) : PublicApiResource::PublicApiResource(session)
{
    resourceClassName = "PluginResource";

    functionMap["getPluginsList"] = boost::bind(&PluginResource::getPluginsList, this, _1, _2, _3, _4, _5);
    functionMap["getPlugin"] = boost::bind(&PluginResource::getPlugin, this, _1, _2, _3, _4, _5);
    functionMap["getAliasForPlugin"] = boost::bind(&PluginResource::getAliasForPlugin, this, _1, _2, _3, _4, _5);
    functionMap["getInformationsListForPlugin"] = boost::bind(&PluginResource::getInformationsListForPlugin, this, _1, _2, _3, _4, _5);
    functionMap["getAssetForPlugin"] = boost::bind(&PluginResource::getAssetForPlugin, this, _1, _2, _3, _4, _5);
    functionMap["postPlugin"] = boost::bind(&PluginResource::postPlugin, this, _1, _2, _3, _4, _5);
    functionMap["putPlugin"] = boost::bind(&PluginResource::putPlugin, this, _1, _2, _3, _4, _5);
    functionMap["putAliasForPlugin"] = boost::bind(&PluginResource::putAliasForPlugin, this, _1, _2, _3, _4, _5);
    functionMap["deletePlugin"] = boost::bind(&PluginResource::deletePlugin, this, _1, _2, _3, _4, _5);
    
    calls = FillCallsVector();
    
    /*Call structFillTmp;
    
    structFillTmp.method = "GET";
    structFillTmp.path = "";
    structFillTmp.function = boost::bind(&PluginResource::getPluginsList, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "GET";
    structFillTmp.path = "/[0-9]+";
    structFillTmp.function = boost::bind(&PluginResource::getPlugin, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "GET";
    structFillTmp.path = "/[0-9]+/alias";
    structFillTmp.parameters.push_back("media_type_id");
    structFillTmp.parameters.push_back("user_role_id");
    structFillTmp.function = boost::bind(&PluginResource::getAliasForPlugin, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "GET";
    structFillTmp.path = "/[0-9]+/informations";
    structFillTmp.function = boost::bind(&PluginResource::getInformationsListForPlugin, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "GET";
    structFillTmp.path = "/[0-9]+/assets";
    structFillTmp.function = boost::bind(&PluginResource::getAssetForPlugin, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "GET";
    structFillTmp.path = "/(\\D)*";
    structFillTmp.function = boost::bind(&PluginResource::Error, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "POST";
    structFillTmp.path = "";
    structFillTmp.function = boost::bind(&PluginResource::postPlugin, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "POST";
    structFillTmp.path = ".+";
    structFillTmp.function = boost::bind(&PluginResource::Error, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "PUT";
    structFillTmp.path = "";
    structFillTmp.function = boost::bind(&PluginResource::Error, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "PUT";
    structFillTmp.path = "/[0-9]+";
    structFillTmp.function = boost::bind(&PluginResource::putPlugin, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "PUT";
    structFillTmp.path = "/[0-9]+/alias";
    structFillTmp.function = boost::bind(&PluginResource::putAliasForPlugin, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "PUT";
    structFillTmp.path = "/(\\D)*";
    structFillTmp.function = boost::bind(&PluginResource::Error, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "DELETE";
    structFillTmp.path = "";
    structFillTmp.function = boost::bind(&PluginResource::Error, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "DELETE";
    structFillTmp.path = "/[0-9]+";
    structFillTmp.function = boost::bind(&PluginResource::deletePlugin, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "DELETE";
    structFillTmp.path = "/(\\D)*";
    structFillTmp.function = boost::bind(&PluginResource::Error, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);*/
}

PluginResource::~PluginResource()
{
}

EReturnCode PluginResource::getPluginsList(const long long &grpId, string &responseMsg)
EReturnCode PluginResource::getPluginsList(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Plugin>> plgPtrCol = m_session.find<Echoes::Dbo::Plugin>()
                .where(QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = ?").bind(grpId)
                .orderBy(QUOTE(TRIGRAM_PLUGIN ID));

        res = serialize(plgPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode PluginResource::getPlugin(const std::vector<std::string> &pathElements, const long long &grpId, std::string &responseMsg)
EReturnCode PluginResource::getPlugin(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::Plugin> plgPtr = m_session.find<Echoes::Dbo::Plugin>()
                .where(QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_PLUGIN ID) " = ?").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = ?").bind(grpId);

        res = serialize(plgPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode PluginResource::getAliasForPlugin(const std::vector<std::string> &pathElements, map<string, long long> &parameters, const long long &grpId, string &responseMsg)
EReturnCode PluginResource::getAliasForPlugin(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    if (parameters["media_type_id"] <= 0 || parameters["user_role_id"] <= 0)
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Criterion Resource] media_types or/and user_role are empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session, true);

            Wt::Dbo::ptr<Echoes::Dbo::UserRole> uroPtr = m_session.find<Echoes::Dbo::UserRole>()
                    .where(QUOTE(TRIGRAM_USER_ROLE ID) " = ?").bind(parameters["user_role_id"])
                    .where(QUOTE(TRIGRAM_USER_ROLE SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = ?").bind(grpId)
                    .where(QUOTE(TRIGRAM_USER_ROLE SEP "DELETE") " IS NULL");
            if (uroPtr)
            {
                Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasPlugin> aapPtr = m_session.find<Echoes::Dbo::AlertMessageAliasPlugin>()
                        .where(QUOTE(TRIGRAM_ALERT_MESSAGE_ALIAS_PLUGIN SEP "DELETE") " IS NULL")
                        .where(QUOTE(TRIGRAM_USER_ROLE ID SEP TRIGRAM_USER_ROLE ID) " = ?").bind(parameters["user_role_id"])
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE ID SEP TRIGRAM_MEDIA_TYPE ID) " = ?").bind(parameters["media_type_id"])
                        .where(QUOTE(TRIGRAM_PLUGIN ID SEP TRIGRAM_PLUGIN ID) " = ?").bind(pathElements[1]);

                res = serialize(aapPtr, responseMsg);
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

EReturnCode PluginResource::getInformationsListForPlugin(const std::vector<std::string> &pathElements, const long long &grpId, string &responseMsg)
EReturnCode PluginResource::getInformationsListForPlugin(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);
        const string queryStr =
" SELECT inf\n"
"   FROM " QUOTE("T_INFORMATION" SEP TRIGRAM_INFORMATION) " inf\n"
"   WHERE\n"
"     " QUOTE(TRIGRAM_INFORMATION ID) " IN\n"
"       (\n"
"         SELECT " QUOTE(TRIGRAM_INFORMATION_DATA SEP TRIGRAM_INFORMATION SEP TRIGRAM_INFORMATION ID) "\n"
"           FROM " QUOTE("T_INFORMATION_DATA" SEP TRIGRAM_INFORMATION_DATA) "\n"
"           WHERE\n"
"             " QUOTE(TRIGRAM_INFORMATION_DATA SEP TRIGRAM_FILTER SEP TRIGRAM_FILTER ID) " IN\n"
"               (\n"
"                 SELECT " QUOTE(TRIGRAM_FILTER ID) "\n"
"                   FROM " QUOTE("T_FILTER" SEP TRIGRAM_FILTER) "\n"
"                   WHERE\n"
"                     " QUOTE(TRIGRAM_FILTER SEP TRIGRAM_SEARCH SEP TRIGRAM_SEARCH ID) " IN\n"
"                       (\n"
"                         SELECT " QUOTE(TRIGRAM_SEARCH ID) "\n"
"                           FROM " QUOTE("T_SEARCH" SEP TRIGRAM_SEARCH) "\n"
"                           WHERE\n"
"                             " QUOTE(TRIGRAM_SEARCH SEP TRIGRAM_SOURCE SEP TRIGRAM_SOURCE ID) " IN\n"
"                               (\n"
"                                 SELECT " QUOTE(TRIGRAM_SOURCE ID) "\n"
"                                   FROM " QUOTE("T_SOURCE" SEP TRIGRAM_SOURCE) "\n"
"                                   WHERE\n"
"                                     " QUOTE(TRIGRAM_SOURCE ID) " IN\n"
"                                       (\n"
"                                         SELECT " QUOTE("T_SOURCE" SEP TRIGRAM_SOURCE SEP TRIGRAM_SOURCE ID) "\n"
"                                           FROM " QUOTE("TJ" SEP TRIGRAM_PLUGIN SEP TRIGRAM_SOURCE) "\n"
"                                           WHERE\n"
"                                             " QUOTE("T_PLUGIN" SEP TRIGRAM_PLUGIN SEP TRIGRAM_PLUGIN ID) " IN\n"
"                                               (\n"
"                                                 SELECT " QUOTE(TRIGRAM_PLUGIN ID) "\n"
"                                                   FROM " QUOTE("T_PLUGIN" SEP TRIGRAM_PLUGIN) "\n"
"                                                   WHERE\n"
"                                                     " QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = " + boost::lexical_cast<string>(grpId) + "\n"
"                                                     AND " QUOTE(TRIGRAM_PLUGIN ID) " = " + pathElements[1] + "\n"
"                                                     AND " QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL\n"
"                                               )\n"
"                                       )\n"
"                                     AND " QUOTE(TRIGRAM_SOURCE SEP "DELETE") " IS NULL\n"
"                               )\n"
"                             AND " QUOTE(TRIGRAM_SEARCH SEP "DELETE") " IS NULL\n"
"                       )\n"
"                     AND " QUOTE(TRIGRAM_FILTER SEP "DELETE") " IS NULL\n"
"               )\n"
"             AND " QUOTE(TRIGRAM_INFORMATION_DATA SEP "DELETE") " IS NULL\n"
"       )\n"
"     AND " QUOTE(TRIGRAM_INFORMATION SEP "DELETE") " IS NULL\n";

        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Information>> queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::Information>>(queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Information>> infPtrCol = queryRes.resultList();

        res = serialize(infPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

//EReturnCode PluginResource::getAssetsListForPlugin(const std::vector<std::string> &pathElements, const long long &grpId, string &responseMsg)
//{
//    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//
//    try
//    {
//        Wt::Dbo::Transaction transaction(m_session, true);
//        const string queryStr =
//            " SELECT ast"
//            "   FROM " QUOTE("T_ASSET_AST") " ast"
//            "   WHERE"
//            "     " QUOTE(TRIGRAM_ASSET ID) " IN"
//            "       ("
//            "         SELECT " QUOTE("T_ASSET_AST" SEP TRIGRAM_ASSET ID)
//            "           FROM " QUOTE("TJ_PLG_AST")
//            "           WHERE " QUOTE("T_PLUGIN_PLG" SEP TRIGRAM_PLUGIN ID) " = " + pathElements[1] +
//            "       )"
//            "     AND " QUOTE(TRIGRAM_ASSET SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = " + boost::lexical_cast<string>(grpId) +
//            "     AND " QUOTE(TRIGRAM_ASSET SEP "DELETE") " IS NULL";
//
//            Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Asset>> queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::Asset>>(queryStr);
//
//            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Asset>> plgColl = queryRes.resultList();
//            
//
//            if (plgColl.size() > 0)
//            {
//                res = serialize(plgColl, responseMsg);
//            }
//            else
//            {
//                res = EReturnCode::NOT_FOUND;
//                responseMsg = httpCodeToJSON(res, plgColl);
//            }
//
//        transaction.commit();
//    }
//    catch (Wt::Dbo::Exception const& e)
//    {
//        res = EReturnCode::SERVICE_UNAVAILABLE;
//        responseMsg = httpCodeToJSON(res, e);
//    }
//    return res;
//}

EReturnCode PluginResource::getAssetForPlugin(const std::vector<std::string> &pathElements, const long long &grpId, string &responseMsg)
EReturnCode PluginResource::getAssetForPlugin(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);
        const string queryStr =
            " SELECT ast "
            " FROM " QUOTE("T_ASSET_AST") " ast "
            " WHERE " QUOTE(TRIGRAM_ASSET ID) " IN "
            "   ( "
            "      SELECT " QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_ASSET SEP TRIGRAM_ASSET ID)
            "        FROM " QUOTE("T_PLUGIN_PLG")
            "       WHERE " QUOTE(TRIGRAM_PLUGIN ID) " = " + pathElements[1] +
            "         AND " QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = " + boost::lexical_cast<string>(grpId) +
            "         AND " QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL"
            "   )"
            "   AND " QUOTE(TRIGRAM_ASSET SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = " + boost::lexical_cast<string>(grpId) +
            "   AND " QUOTE(TRIGRAM_ASSET SEP "DELETE") " IS NULL";

        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Asset>> queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::Asset>>(queryStr);
        Wt::Dbo::ptr<Echoes::Dbo::Asset> ptrAsset = queryRes.resultValue();

        res = serialize(ptrAsset, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode PluginResource::processGetRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    string nextElement = "";
    unsigned short indexPathElement = 1;
    vector<string> pathElements;
    map<string, long long> parameters;
    
    parameters["media_type_id"] = 0;
    parameters["user_role_id"] = 0;

    const string sRequest = processRequestParameters(request, pathElements, parameters);

    nextElement = getNextElementFromPath(indexPathElement, pathElements);
    if (nextElement.empty())
    {
        res = getPluginsList(grpId, responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.empty())
            {
                res = getPlugin(pathElements, grpId, responseMsg);
                res = getPlugin(orgId, responseMsg, pathElements);
            }
            else if (nextElement.compare("alias") == 0)
            {
                res = getAliasForPlugin(pathElements, parameters, grpId, responseMsg);
                res = getAliasForPlugin(orgId, responseMsg, pathElements, sRequest, parameters);
            }
            else if (nextElement.compare("informations") == 0)
            {
                res = getInformationsListForPlugin(pathElements, grpId, responseMsg);
                res = getInformationsListForPlugin(orgId, responseMsg, pathElements);
            }
            else if (nextElement.compare("assets") == 0)
            {
                res = getAssetForPlugin(pathElements, grpId, responseMsg);
                res = getAssetForPlugin(orgId, responseMsg, pathElements);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Plugin Resource] bad nextElement";
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

EReturnCode PluginResource::postPlugin(const string& sRequest, const long long &grpId, string& responseMsg)
EReturnCode PluginResource::postPlugin(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    Wt::WString name;
    Wt::WString desc;
    Wt::WString assetId;

    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            name = result.get("name");
            desc = result.get("desc");
            assetId = result.get("asset_id");
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
        const string err = "[Plugin Resource] sRequest is not empty";
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

            Wt::Dbo::ptr<Echoes::Dbo::Asset> astPtr = m_session.find<Echoes::Dbo::Asset>()
                    .where(QUOTE(TRIGRAM_ASSET ID) " = ?").bind(assetId)
                    .where(QUOTE(TRIGRAM_ASSET SEP "DELETE") " IS NULL");
            if (astPtr)
            {
                Echoes::Dbo::Plugin *newPlg = new Echoes::Dbo::Plugin();
                newPlg->group = grpPtr;
//                newPlg->pluginReference = prePtr;
//                newPlg->versionRef = prePtr->version;
                newPlg->versionRef = "1.0";
                newPlg->versionClient = "1.0";
                newPlg->name = name;
                newPlg->desc = desc;
                newPlg->asset = astPtr;
                Wt::Dbo::ptr<Echoes::Dbo::Plugin> newPlgPtr = m_session.add<Echoes::Dbo::Plugin>(newPlg);
                newPlgPtr.flush();

                res = serialize(newPlgPtr, responseMsg, EReturnCode::CREATED);
            }
            else
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, astPtr);
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

EReturnCode PluginResource::processPostRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
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
        res = postPlugin(sRequest, grpId, responseMsg);
        res = postPlugin(orgId, responseMsg, pathElements, sRequest);
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Plugin Resource] bad nextElement";
        responseMsg = httpCodeToJSON(res, err);
    }

    return res;
}

EReturnCode PluginResource::putAliasForPlugin(const std::vector<std::string> &pathElements, const string &sRequest, const long long &grpId, string &responseMsg)
EReturnCode PluginResource::putAliasForPlugin(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    long long uroId;
    long long mtyId;
    Wt::WString value;
    
    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            uroId = result.get("user_role_id");
            mtyId = result.get("media_type_id");
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
        const string err = "[Information Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session, true);

            Wt::Dbo::ptr<Echoes::Dbo::Plugin> plgPtr =  m_session.find<Echoes::Dbo::Plugin>()
                .where(QUOTE(TRIGRAM_PLUGIN ID) " = ?").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = ?").bind(grpId)
                .where(QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL");
            if (plgPtr)
            {
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

                Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasPlugin> aapPtr = m_session.find<Echoes::Dbo::AlertMessageAliasPlugin>()
                        .where(QUOTE(TRIGRAM_USER_ROLE ID SEP TRIGRAM_USER_ROLE ID) " = ?").bind(uroId)
                        .where(QUOTE(TRIGRAM_PLUGIN ID SEP TRIGRAM_PLUGIN ID) " = ?").bind(pathElements[1])
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE ID SEP TRIGRAM_MEDIA_TYPE ID) " = ?").bind(mtyId);
                if (aapPtr)
                {
                    aapPtr.modify()->alias = value;
                }
                else
                {
                    Echoes::Dbo::AlertMessageAliasPlugin *newAap = new Echoes::Dbo::AlertMessageAliasPlugin();
                    newAap->pk.plugin = plgPtr;
                    newAap->pk.userRole = uroPtr;
                    newAap->pk.mediaType = mtyPtr;
                    newAap->alias = value;
                    aapPtr = m_session.add<Echoes::Dbo::AlertMessageAliasPlugin>(newAap);
                }
                res = EReturnCode::OK;
            }
            else
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, plgPtr);
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

EReturnCode PluginResource::putPlugin(const std::vector<std::string> &pathElements, const string &sRequest, const long long &grpId, string &responseMsg)
EReturnCode PluginResource::putPlugin(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    Wt::WString name;
    Wt::WString desc;
    Wt::WString assetId;

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
            if (result.contains("desc"))
            {
                desc = result.get("desc");
            }
            if (result.contains("asset_id"))
            {
                assetId = result.get("asset_id");
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
        const string err = "[Plugin Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session, true);
            
            Wt::Dbo::ptr<Echoes::Dbo::Plugin> plgPtr = m_session.find<Echoes::Dbo::Plugin>()
                .where(QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_PLUGIN ID) " = ?").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = ?").bind(grpId);

            if (plgPtr)
            {
                if (!name.empty())
                {
                    plgPtr.modify()->name = name;
                }

                if (!desc.empty())
                {
                    plgPtr.modify()->desc = desc;
                }
                if (!assetId.empty())
                {
                    Wt::Dbo::ptr<Echoes::Dbo::Asset> astPtr = m_session.find<Echoes::Dbo::Asset>()
                        .where(QUOTE(TRIGRAM_ASSET ID) " = ?").bind(assetId)
                        .where(QUOTE(TRIGRAM_ASSET SEP "DELETE") " IS NULL");
                    if (astPtr)
                    {
                        plgPtr.modify()->asset = astPtr;
                    }
                }

                res = serialize(plgPtr, responseMsg);
            }
            else
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, plgPtr);
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

EReturnCode PluginResource::processPutRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
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
        const string err = "[Plugin Resource] bad nextElement";
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
                res = putPlugin(pathElements, sRequest, grpId, responseMsg);
                res = putPlugin(orgId, responseMsg, pathElements, sRequest);
            }
            else if (nextElement.compare("alias") == 0)
            {
                res = putAliasForPlugin(pathElements, sRequest, grpId, responseMsg);
                res = putAliasForPlugin(orgId, responseMsg, pathElements, sRequest);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Plugin Resource] bad nextElement";
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

EReturnCode PluginResource::deletePlugin(const std::vector<std::string> &pathElements, const long long &grpId, string& responseMsg)
EReturnCode PluginResource::deletePlugin(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try 
    {  
        Wt::Dbo::Transaction transaction(m_session, true);
           
        Wt::Dbo::ptr<Echoes::Dbo::Plugin> plgPtr = m_session.find<Echoes::Dbo::Plugin>()
                .where(QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_PLUGIN ID) " = ?").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = ?").bind(grpId);

        if(plgPtr)
        {
            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Source>> srcPtrCol = plgPtr->sources.find().where(QUOTE(TRIGRAM_SOURCE SEP "DELETE") " IS NULL");

            if (srcPtrCol.size() == 0)
            {
                plgPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
                res = EReturnCode::NO_CONTENT;
            }
            else
            {
                res = EReturnCode::LOCKED;
                responseMsg = httpCodeToJSON(res, plgPtr);
            } 
        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = httpCodeToJSON(res, plgPtr);
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

EReturnCode PluginResource::processDeleteRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
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
        const string err = "[Plugin Resource] bad nextElement";
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
                res = deletePlugin(pathElements, grpId, responseMsg);
                res = deletePlugin(orgId, responseMsg, pathElements);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Plugin Resource] bad nextElement";
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

