/**
 * @file AssetResource.cpp
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

#include "AssetResource.h"

using namespace std;

AssetResource::AssetResource(Echoes::Dbo::Session& session) : PublicApiResource::PublicApiResource(session)
{
}

AssetResource::~AssetResource()
{
}

Wt::Dbo::ptr<Echoes::Dbo::Asset> AssetResource::selectAsset(const long long &astId, const long long &orgId, Echoes::Dbo::Session &session)
{
    return selectAsset(boost::lexical_cast<string>(astId), boost::lexical_cast<string>(orgId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::Asset> AssetResource::selectAsset(const string &astId, const long long &orgId, Echoes::Dbo::Session &session)
{
    return selectAsset(astId, boost::lexical_cast<string>(orgId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::Asset> AssetResource::selectAsset(const string &astId, const string &orgId, Echoes::Dbo::Session &session)
{
    return session.find<Echoes::Dbo::Asset>()
            .where(QUOTE(TRIGRAM_ASSET ID) " = ?").bind(astId)
            .where(QUOTE(TRIGRAM_ASSET SEP "DELETE") " IS NULL")
            .where(QUOTE(TRIGRAM_ASSET SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(orgId);
}

EReturnCode AssetResource::getAssetsList(const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Asset>> astPtrCol = m_session.find<Echoes::Dbo::Asset>()
                .where(QUOTE(TRIGRAM_ASSET SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_ASSET SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(orgId)
                .orderBy(QUOTE(TRIGRAM_ASSET ID));

        res = serialize(astPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode AssetResource::getAsset(const vector<string> &pathElements, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::Asset> astPtr = selectAsset(pathElements[1], orgId, m_session);

        res = serialize(astPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode AssetResource::getAliasForAsset(const vector<string> &pathElements, map<string, long long> &parameters, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    if (parameters["media_type_id"] <= 0 || parameters["user_role_id"] <= 0)
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Assert Resource] media_types or/and user_role are empty";
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
                Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasAsset> aaaPtr = m_session.find<Echoes::Dbo::AlertMessageAliasAsset>()
                        .where(QUOTE(TRIGRAM_ALERT_MESSAGE_ALIAS_ASSET SEP "DELETE") " IS NULL")
                        .where(QUOTE(TRIGRAM_USER_ROLE ID SEP TRIGRAM_USER_ROLE ID) " = ?").bind(parameters["user_role_id"])
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE ID SEP TRIGRAM_MEDIA_TYPE ID) " = ?").bind(parameters["media_type_id"])
                        .where(QUOTE(TRIGRAM_ASSET ID SEP TRIGRAM_ASSET ID) " = ?").bind(pathElements[1]);

                res = serialize(aaaPtr, responseMsg);
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

EReturnCode AssetResource::getPluginsForAsset(const vector<string> &pathElements, map<string, long long> &parameters, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session, true);

            
            
            const string queryStr =
            " SELECT plg"
            "   FROM " QUOTE("T_PLUGIN_PLG") " plg"
            "   WHERE " QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_ASSET SEP TRIGRAM_ASSET ID) " = " + pathElements[1] +
            "     AND " QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = " + boost::lexical_cast<string>(orgId) +
            "     AND " QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL";

            Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Plugin>> queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::Plugin>>(queryStr);

            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Plugin>> plgColl = queryRes.resultList();
            

            if (plgColl.size() > 0)
            {
                res = serialize(plgColl, responseMsg);
            }
            else
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, plgColl);
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

EReturnCode AssetResource::processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        res = getAssetsList(orgId, responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.empty())
            {
                res = getAsset(pathElements, orgId, responseMsg);
            }
            else if (!nextElement.compare("alias"))
            {
                res = getAliasForAsset(pathElements, parameters, orgId, responseMsg);
            }
            else if (!nextElement.compare("plugins"))
            {
                res = getPluginsForAsset(pathElements, parameters, orgId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Asset Resource] bad nextElement";
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

EReturnCode AssetResource::postAsset(const string &sRequest, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    Wt::WString name;
    Wt::WString architecture;
    Wt::WString distribution;
    Wt::WString release;

    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            name = result.get("name");
            if (result.contains("distribution"))
            {
                distribution = result.get("distribution");
            }
            if (result.contains("architecture"))
            {
                architecture = result.get("architecture");
            }
            if (result.contains("release"))
            {
                release = result.get("release");
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
        const string err = "[Assert Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session, true);

            Wt::Dbo::ptr<Echoes::Dbo::Organization> orgPtr = m_session.find<Echoes::Dbo::Organization>()
                    .where(QUOTE(TRIGRAM_ORGANIZATION SEP "DELETE") " IS NULL")
                    .where(QUOTE(TRIGRAM_ORGANIZATION ID) " = ?").bind(orgId);
            if (!orgPtr)
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, orgPtr);
                return res;
            }

            Echoes::Dbo::Asset *newAst = new Echoes::Dbo::Asset();
            newAst->name = name;
            newAst->assetIsHost = false;
            newAst->organization = orgPtr;

            if (!architecture.empty())
            {
                Wt::Dbo::ptr<Echoes::Dbo::AssetArchitecture> asaPtr = m_session.find<Echoes::Dbo::AssetArchitecture>()
                        .where(QUOTE(TRIGRAM_ASSET_ARCHITECTURE SEP "NAME") " = ?").bind(architecture)
                        .limit(1);
                if (!asaPtr)
                {
                    Echoes::Dbo::AssetArchitecture *newAsa = new Echoes::Dbo::AssetArchitecture();
                    newAsa->name = architecture;
                    asaPtr = m_session.add<Echoes::Dbo::AssetArchitecture>(newAsa);
                    asaPtr.flush();
                }
                newAst->assetArchitecture = asaPtr;
            }

            if (!distribution.empty())
            {
                Wt::Dbo::ptr<Echoes::Dbo::AssetDistribution> asdPtr = m_session.find<Echoes::Dbo::AssetDistribution>()
                        .where(QUOTE(TRIGRAM_ASSET_DISTRIBUTION SEP "NAME") " = ?").bind(distribution)
                        .limit(1);
                if (!asdPtr)
                {
                    Echoes::Dbo::AssetDistribution *newAsd = new Echoes::Dbo::AssetDistribution();
                    newAsd->name = distribution;
                    asdPtr = m_session.add<Echoes::Dbo::AssetDistribution>(newAsd);
                    asdPtr.flush();
                }
                newAst->assetDistribution = asdPtr;
            }

            if (!distribution.empty())
            {
                Wt::Dbo::ptr<Echoes::Dbo::AssetRelease> asrPtr = m_session.find<Echoes::Dbo::AssetRelease>()
                        .where(QUOTE(TRIGRAM_ASSET_RELEASE SEP "NAME") " = ?").bind(release)
                        .limit(1);
                if (!asrPtr)
                {
                    Echoes::Dbo::AssetRelease *newAsr = new Echoes::Dbo::AssetRelease();
                    newAsr->name = release;
                    asrPtr = m_session.add<Echoes::Dbo::AssetRelease>(newAsr);
                    asrPtr.flush();
                }
                newAst->assetRelease = asrPtr;
            }

            Wt::Dbo::ptr<Echoes::Dbo::Asset> newAstPtr = m_session.add<Echoes::Dbo::Asset>(newAst);
            newAstPtr.flush();

            res = serialize(newAstPtr, responseMsg, EReturnCode::CREATED);

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

EReturnCode AssetResource::postPluginForAsset(const vector<string> &pathElements, const string &sRequest, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    // IDA attributs
    struct IdaStruct
    {
        long long astId;
        long long infId;
        long long inuId;
        long long filId;
        int filterFieldIndex;
    };
    vector<IdaStruct> idaStructs;

    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Value result;
            Wt::Json::parse(sRequest, result);

            Wt::Json::Array idaAttributs = result;
            for (Wt::Json::Array::const_iterator it = idaAttributs.begin(); it < idaAttributs.end(); ++it)
            {
                Wt::Json::Object tmp = *it;
                idaStructs.push_back(
                {
                    tmp.get("asset_id"),
                    tmp.get("information_id"),
                    tmp.get("information_unit_id"),
                    tmp.get("filter_id"),
                    tmp.get("filter_field_index_id")
                }
                );
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
        const string err = "[Asset Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session, true);

            Wt::Dbo::ptr<Echoes::Dbo::Asset> astPtr = selectAsset(pathElements[1], orgId, m_session);
            if (astPtr)
            {
                vector<Wt::Dbo::ptr<Echoes::Dbo::Asset>> astPtrVector;
                vector<Wt::Dbo::ptr<Echoes::Dbo::Information>> infPtrVector;
                vector<Wt::Dbo::ptr<Echoes::Dbo::InformationUnit>> inuPtrVector;
                vector<Wt::Dbo::ptr<Echoes::Dbo::Filter>> filPtrVector;
                for (vector<IdaStruct>::const_iterator it = idaStructs.begin(); it < idaStructs.end(); ++it)
                {
                    Wt::Dbo::ptr<Echoes::Dbo::Asset> astPtr = selectAsset(it->astId, orgId, m_session);
                    if (!astPtr)
                    {
                        res = EReturnCode::NOT_FOUND;
                        responseMsg = httpCodeToJSON(res, astPtr);
                        return res;
                    }
                    astPtrVector.push_back(astPtr);

                    Wt::Dbo::ptr<Echoes::Dbo::Information> infPtr = m_session.find<Echoes::Dbo::Information>()
                            .where(QUOTE(TRIGRAM_INFORMATION ID) " = ?").bind(it->infId)
                            .where(QUOTE(TRIGRAM_INFORMATION SEP "DELETE") " IS NULL");
                    if (!infPtr)
                    {
                        res = EReturnCode::NOT_FOUND;
                        responseMsg = httpCodeToJSON(res, infPtr);
                        return res;
                    }
                    infPtrVector.push_back(infPtr);

                    Wt::Dbo::ptr<Echoes::Dbo::InformationUnit> inuPtr = m_session.find<Echoes::Dbo::InformationUnit>()
                            .where(QUOTE(TRIGRAM_INFORMATION_UNIT ID) " = ?").bind(it->inuId)
                            .where(QUOTE(TRIGRAM_INFORMATION_UNIT SEP "DELETE") " IS NULL");
                    if (!inuPtr)
                    {
                        res = EReturnCode::NOT_FOUND;
                        responseMsg = httpCodeToJSON(res, inuPtr);
                        return res;
                    }
                    inuPtrVector.push_back(inuPtr);

                    Wt::Dbo::ptr<Echoes::Dbo::Filter> filPtr = FilterResource::selectFilter(it->filId, orgId, m_session);
                    if (!filPtr)
                    {
                        res = EReturnCode::NOT_FOUND;
                        responseMsg = httpCodeToJSON(res, filPtr);
                        return res;
                    }
                    filPtrVector.push_back(filPtr);

                    if (it->filterFieldIndex > filPtr->nbValue)
                    {
                        res = EReturnCode::BAD_REQUEST;
                        const string err = "[Asset Resource] filterFieldIndex > filPtr->nbValue";
                        responseMsg = httpCodeToJSON(res, err);
                        return res;
                    }
                }

                for (vector<IdaStruct>::iterator it = idaStructs.begin(); it < idaStructs.end(); ++it)
                {
                    Echoes::Dbo::InformationData *newIda = new Echoes::Dbo::InformationData();
                    newIda->asset = astPtrVector[distance(idaStructs.begin(), it)];
                    newIda->information = infPtrVector[distance(idaStructs.begin(), it)];
                    newIda->informationUnit = inuPtrVector[distance(idaStructs.begin(), it)];
                    newIda->filter = filPtrVector[distance(idaStructs.begin(), it)];
                    newIda->filterFieldIndex = it->filterFieldIndex;
                    
                    Wt::Dbo::ptr<Echoes::Dbo::InformationData> newIdaPtr = m_session.add<Echoes::Dbo::InformationData>(newIda);
                    newIdaPtr.flush();
                }

                res = serialize(astPtr, responseMsg, EReturnCode::CREATED);
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

EReturnCode AssetResource::processPostRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        res = postAsset(sRequest, orgId, responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.compare("plugins") == 0)
            {
                res = postPluginForAsset(pathElements, sRequest, orgId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Asset Resource] bad nextElement";
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

EReturnCode AssetResource::putAsset(const vector<string> &pathElements, const string &sRequest, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    Wt::WString name;
    Wt::WString architecture;
    Wt::WString distribution;
    Wt::WString release;

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
            if (result.contains("distribution"))
            {
                distribution = result.get("distribution");
            }
            if (result.contains("architecture"))
            {
                architecture = result.get("architecture");
            }
            if (result.contains("release"))
            {
                release = result.get("release");
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
        const string err = "[Assert Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session, true);

            Wt::Dbo::ptr<Echoes::Dbo::Asset> astPtr = selectAsset(pathElements[1], orgId, m_session);
            if (astPtr)
            {
                if (!name.empty())
                {
                    astPtr.modify()->name = name;
                }

                if (!architecture.empty())
                {
                    Wt::Dbo::ptr<Echoes::Dbo::AssetArchitecture> asaPtr = m_session.find<Echoes::Dbo::AssetArchitecture>()
                            .where(QUOTE(TRIGRAM_ASSET_ARCHITECTURE SEP "NAME") " = ?").bind(architecture)
                            .limit(1);
                    if (!asaPtr)
                    {
                        Echoes::Dbo::AssetArchitecture *newAsa = new Echoes::Dbo::AssetArchitecture();
                        newAsa->name = architecture;
                        asaPtr = m_session.add<Echoes::Dbo::AssetArchitecture>(newAsa);
                        asaPtr.flush();
                    }
                    astPtr.modify()->assetArchitecture = asaPtr;
                }

                if (!distribution.empty())
                {
                    Wt::Dbo::ptr<Echoes::Dbo::AssetDistribution> asdPtr = m_session.find<Echoes::Dbo::AssetDistribution>()
                            .where(QUOTE(TRIGRAM_ASSET_DISTRIBUTION SEP "NAME") " = ?").bind(distribution)
                            .limit(1);
                    if (!asdPtr)
                    {
                        Echoes::Dbo::AssetDistribution *newAsd = new Echoes::Dbo::AssetDistribution();
                        newAsd->name = distribution;
                        asdPtr = m_session.add<Echoes::Dbo::AssetDistribution>(newAsd);
                        asdPtr.flush();
                    }
                    astPtr.modify()->assetDistribution = asdPtr;
                }

                if (!distribution.empty())
                {
                    Wt::Dbo::ptr<Echoes::Dbo::AssetRelease> asrPtr = m_session.find<Echoes::Dbo::AssetRelease>()
                            .where(QUOTE(TRIGRAM_ASSET_RELEASE SEP "NAME") " = ?").bind(release)
                            .limit(1);
                    if (!asrPtr)
                    {
                        Echoes::Dbo::AssetRelease *newAsr = new Echoes::Dbo::AssetRelease();
                        newAsr->name = release;
                        asrPtr = m_session.add<Echoes::Dbo::AssetRelease>(newAsr);
                        asrPtr.flush();
                    }
                    astPtr.modify()->assetRelease = asrPtr;
                }
                
                if (!architecture.empty() || !distribution.empty() || !distribution.empty())
                {
                    Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Probe>> prbPtrCol = astPtr->probes.find()
                            .where(QUOTE(TRIGRAM_PROBE SEP "DELETE") " IS NULL");

                    for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Probe> >::const_iterator it = prbPtrCol.begin(); it != prbPtrCol.end(); ++it)
                    {
                        it->modify()->probePackageParameter = ProbeResource::selectProbePackageParameter(astPtr, m_session);
                    }
                }

                res = serialize(astPtr, responseMsg);
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

EReturnCode AssetResource::putAliasForAsset(const vector<string> &pathElements, const string &sRequest, const long long &orgId, string &responseMsg)
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
        const string err = "[Assert Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session, true);

            Wt::Dbo::ptr<Echoes::Dbo::Asset> astPtr = m_session.find<Echoes::Dbo::Asset>()
                    .where(QUOTE(TRIGRAM_ASSET ID) " = ?").bind(pathElements[1])
                    .where(QUOTE(TRIGRAM_ASSET SEP "DELETE") " IS NULL")
                    .where(QUOTE(TRIGRAM_ASSET SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(orgId);

            if (astPtr)
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

                Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasAsset> aaaPtr = m_session.find<Echoes::Dbo::AlertMessageAliasAsset>()
                        .where(QUOTE(TRIGRAM_USER_ROLE ID SEP TRIGRAM_USER_ROLE ID) " = ?").bind(uroId)
                        .where(QUOTE(TRIGRAM_ASSET ID SEP TRIGRAM_ASSET ID) " = ?").bind(pathElements[1])
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE ID SEP TRIGRAM_MEDIA_TYPE ID) " = ?").bind(mtyId);
                if (aaaPtr)
                {
                    aaaPtr.modify()->alias = value;
                }
                else
                {
                    Echoes::Dbo::AlertMessageAliasAsset *newAaa = new Echoes::Dbo::AlertMessageAliasAsset();
                    newAaa->pk.asset = astPtr;
                    newAaa->pk.userRole = uroPtr;
                    newAaa->pk.mediaType = mtyPtr;
                    newAaa->alias = value;
                    aaaPtr = m_session.add<Echoes::Dbo::AlertMessageAliasAsset>(newAaa);
                }
                res = EReturnCode::OK;
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

EReturnCode AssetResource::processPutRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        const string err = "[Asset Resource] bad nextElement";
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
                res = putAsset(pathElements, sRequest, orgId, responseMsg);
            }
            else if (!nextElement.compare("alias"))
            {
                res = putAliasForAsset(pathElements, sRequest, orgId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Asset Resource] bad nextElement";
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

EReturnCode AssetResource::deleteAsset(const vector<string> &pathElements, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::Asset> astPtr = selectAsset(pathElements[1], orgId, m_session);
        if (astPtr)
        {
            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationData>> idaPtrCol = m_session.find<Echoes::Dbo::InformationData>()
                    .where(QUOTE(TRIGRAM_INFORMATION_DATA SEP TRIGRAM_ASSET SEP TRIGRAM_ASSET ID)" = ?").bind(pathElements[1])
                    .where(QUOTE(TRIGRAM_INFORMATION_DATA SEP "DELETE") " IS NULL");

            if (idaPtrCol.size() == 0)
            {
                astPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
                res = EReturnCode::NO_CONTENT;
            }
            else
            {
                res = EReturnCode::CONFLICT;
                responseMsg = httpCodeToJSON(res, astPtr);
            } 
        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = httpCodeToJSON(res, astPtr);
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

EReturnCode AssetResource::processDeleteRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        const string err = "[Asset Resource] bad nextElement";
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
                res = deleteAsset(pathElements, orgId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Asset Resource] bad nextElement";
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
