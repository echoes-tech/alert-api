/* 
 * API Probe Resource
 * @author ECHOES Technologies (FPO)
 * @date 02/11/2012
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2012 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "ProbeResource.h"

using namespace std;

ProbeResource::ProbeResource() : PublicApiResource::PublicApiResource()
{
}

ProbeResource::~ProbeResource()
{
}

Wt::Dbo::ptr<Echoes::Dbo::Probe> ProbeResource::selectProbe(const long long &prbId, Echoes::Dbo::Session &session)
{
    return selectProbe(boost::lexical_cast<string>(prbId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::Probe> ProbeResource::selectProbe(const string &prbId, Echoes::Dbo::Session &session)
{
    const string queryStr =
" SELECT prb"
"   FROM " QUOTE("T_PROBE_PRB") " prb"
"   WHERE"
"     " QUOTE(TRIGRAM_PROBE ID) " = " + prbId +
"     AND " QUOTE(TRIGRAM_PROBE SEP TRIGRAM_ASSET SEP TRIGRAM_ASSET ID) " IN"
"       ("
"         SELECT " QUOTE(TRIGRAM_ASSET ID)
"           FROM " QUOTE("T_ASSET_AST")
"           WHERE " QUOTE(TRIGRAM_ASSET SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = " + boost::lexical_cast<string>(session.user()->organization.id()) +
"             AND " QUOTE(TRIGRAM_ASSET SEP "DELETE") " IS NULL"
"       )"
"     AND " QUOTE(TRIGRAM_PROBE SEP "DELETE") " IS NULL"
"   LIMIT 1";

    Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Probe>> queryRes = session.query<Wt::Dbo::ptr<Echoes::Dbo::Probe>> (queryStr);

    return queryRes.resultValue();
}

Wt::Dbo::ptr<Echoes::Dbo::ProbePackageParameter> ProbeResource::selectProbePackageParameter(const Wt::Dbo::ptr<Echoes::Dbo::Asset> &astPtr, Echoes::Dbo::Session &session)
{
    Wt::Dbo::ptr<Echoes::Dbo::ProbePackageParameter> pppPtr = session.find<Echoes::Dbo::ProbePackageParameter>()
            .where(QUOTE(TRIGRAM_PROBE_PACKAGE_PARAMETER SEP TRIGRAM_ASSET_ARCHITECTURE SEP TRIGRAM_ASSET_ARCHITECTURE ID) " = ?").bind(astPtr->assetArchitecture.id())
            .where(QUOTE(TRIGRAM_PROBE_PACKAGE_PARAMETER SEP TRIGRAM_ASSET_DISTRIBUTION SEP TRIGRAM_ASSET_DISTRIBUTION ID) " = ?").bind(astPtr->assetDistribution.id())
            .where(QUOTE(TRIGRAM_PROBE_PACKAGE_PARAMETER SEP TRIGRAM_ASSET_RELEASE SEP TRIGRAM_ASSET_RELEASE ID) " = ?").bind(astPtr->assetRelease.id())
            .where(QUOTE(TRIGRAM_PROBE_PACKAGE_PARAMETER SEP "DELETE") " IS NULL")
            .orderBy(QUOTE(TRIGRAM_PROBE_PACKAGE_PARAMETER SEP "PROBE_VERSION") " DESC, " QUOTE(TRIGRAM_PROBE_PACKAGE_PARAMETER SEP "PACKAGE_VERSION") " DESC")
            .limit(1);

    bool releaseChecked = false;
    Wt::Dbo::ptr<Echoes::Dbo::AssetRelease> asrPtr;
    if (!pppPtr)
    {
        string wildcardRelease = astPtr->assetRelease->name.toUTF8().substr(0, astPtr->assetRelease->name.toUTF8().find_last_of('.') + 1) + "*";
        asrPtr = session.find<Echoes::Dbo::AssetRelease>().where(QUOTE(TRIGRAM_ASSET_RELEASE SEP "NAME") " = ?").bind(wildcardRelease);
        pppPtr = session.find<Echoes::Dbo::ProbePackageParameter>()
                .where(QUOTE(TRIGRAM_PROBE_PACKAGE_PARAMETER SEP TRIGRAM_ASSET_ARCHITECTURE SEP TRIGRAM_ASSET_ARCHITECTURE ID) " = ?").bind(astPtr->assetArchitecture.id())
                .where(QUOTE(TRIGRAM_PROBE_PACKAGE_PARAMETER SEP TRIGRAM_ASSET_DISTRIBUTION SEP TRIGRAM_ASSET_DISTRIBUTION ID) " = ?").bind(astPtr->assetDistribution.id())
                .where(QUOTE(TRIGRAM_PROBE_PACKAGE_PARAMETER SEP TRIGRAM_ASSET_RELEASE SEP TRIGRAM_ASSET_RELEASE ID) " = ?").bind(asrPtr.id())
                .where(QUOTE(TRIGRAM_PROBE_PACKAGE_PARAMETER SEP "DELETE") " IS NULL")
                .orderBy(QUOTE(TRIGRAM_PROBE_PACKAGE_PARAMETER SEP "PROBE_VERSION") " DESC, " QUOTE(TRIGRAM_PROBE_PACKAGE_PARAMETER SEP "PACKAGE_VERSION") " DESC")
                .limit(1);
        releaseChecked = true;
    }

    bool architectureChecked = false;
    Wt::Dbo::ptr<Echoes::Dbo::AssetArchitecture> asaPtr;
    if (!pppPtr)
    {
        // Only if arch is an i*86
        if ((boost::starts_with(astPtr->assetArchitecture->name.toUTF8(), "i")) && (boost::ends_with(astPtr->assetArchitecture->name.toUTF8(), "86")))
        {
            string wildcardArchitecture = "i*86";
            asaPtr = session.find<Echoes::Dbo::AssetArchitecture>().where(QUOTE(TRIGRAM_ASSET_ARCHITECTURE SEP "NAME") " = ?").bind(wildcardArchitecture);
            pppPtr = session.find<Echoes::Dbo::ProbePackageParameter>()
                    .where(QUOTE(TRIGRAM_PROBE_PACKAGE_PARAMETER SEP TRIGRAM_ASSET_ARCHITECTURE SEP TRIGRAM_ASSET_ARCHITECTURE ID) " = ?").bind(asaPtr.id())
                    .where(QUOTE(TRIGRAM_PROBE_PACKAGE_PARAMETER SEP TRIGRAM_ASSET_DISTRIBUTION SEP TRIGRAM_ASSET_DISTRIBUTION ID) " = ?").bind(astPtr->assetDistribution.id())
                    .where(QUOTE(TRIGRAM_PROBE_PACKAGE_PARAMETER SEP TRIGRAM_ASSET_RELEASE SEP TRIGRAM_ASSET_RELEASE ID) " = ?").bind(astPtr->assetRelease.id())
                    .where(QUOTE(TRIGRAM_PROBE_PACKAGE_PARAMETER SEP "DELETE") " IS NULL")
                    .orderBy(QUOTE(TRIGRAM_PROBE_PACKAGE_PARAMETER SEP "PROBE_VERSION") " DESC, " QUOTE(TRIGRAM_PROBE_PACKAGE_PARAMETER SEP "PACKAGE_VERSION") " DESC")
                    .limit(1);
            architectureChecked = true;
        }
    }

    if (releaseChecked && architectureChecked && !pppPtr)
    {
        pppPtr = session.find<Echoes::Dbo::ProbePackageParameter>()
                .where(QUOTE(TRIGRAM_PROBE_PACKAGE_PARAMETER SEP TRIGRAM_ASSET_ARCHITECTURE SEP TRIGRAM_ASSET_ARCHITECTURE ID) " = ?").bind(asaPtr.id())
                .where(QUOTE(TRIGRAM_PROBE_PACKAGE_PARAMETER SEP TRIGRAM_ASSET_DISTRIBUTION SEP TRIGRAM_ASSET_DISTRIBUTION ID) " = ?").bind(astPtr->assetDistribution.id())
                .where(QUOTE(TRIGRAM_PROBE_PACKAGE_PARAMETER SEP TRIGRAM_ASSET_RELEASE SEP TRIGRAM_ASSET_RELEASE ID) " = ?").bind(asrPtr.id())
                .where(QUOTE(TRIGRAM_PROBE_PACKAGE_PARAMETER SEP "DELETE") " IS NULL")
                .orderBy(QUOTE(TRIGRAM_PROBE_PACKAGE_PARAMETER SEP "PROBE_VERSION") " DESC, " QUOTE(TRIGRAM_PROBE_PACKAGE_PARAMETER SEP "PACKAGE_VERSION") " DESC")
                .limit(1);
    }

    return pppPtr;
}

EReturnCode ProbeResource::getProbesList(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session);
        string queryStr =
" SELECT prb"
"   FROM " QUOTE("T_PROBE_PRB") " prb"
"   WHERE"
"     " QUOTE(TRIGRAM_PROBE SEP TRIGRAM_ASSET SEP TRIGRAM_ASSET ID) " IN"
"       ("
"         SELECT " QUOTE(TRIGRAM_ASSET ID)
"           FROM " QUOTE("T_ASSET_AST")
"           WHERE " QUOTE(TRIGRAM_ASSET SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = " + boost::lexical_cast<string>(m_session.user()->organization.id()) +
"             AND " QUOTE(TRIGRAM_ASSET SEP "DELETE") " IS NULL"
"       )"
"     AND " QUOTE(TRIGRAM_PROBE SEP "DELETE") " IS NULL"
"   ORDER BY " QUOTE(TRIGRAM_PROBE ID);

        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Probe>> queryRes = m_session.query <Wt::Dbo::ptr<Echoes::Dbo::Probe>> (queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Probe>> prbPtrCol = queryRes.resultList();

        res = serialize(prbPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode ProbeResource::getProbe(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::ptr<Echoes::Dbo::Probe> prbPtr = selectProbe(m_pathElements[1], m_session);

        res = serialize(prbPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode ProbeResource::getJsonForProbe(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::ptr<Echoes::Dbo::Probe> prbPtr = selectProbe(m_pathElements[1], m_session);
        if (prbPtr)
        {
            map<Wt::Dbo::ptr<Echoes::Dbo::Addon>, vector<Wt::Dbo::ptr<Echoes::Dbo::Source>>> srcListByAddon;

            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Plugin>> plgPtrCol = prbPtr->asset->plugins;

            for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Plugin>>::iterator plgPtrIt = plgPtrCol.begin(); plgPtrIt != plgPtrCol.end(); plgPtrIt++)
            {
                Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Source>> srcPtrCol = plgPtrIt->get()->sources;
                for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Source>>::iterator srcPtrIt = srcPtrCol.begin(); srcPtrIt != srcPtrCol.end(); srcPtrIt++)
                {
                    srcListByAddon[srcPtrIt->get()->addon].push_back(*srcPtrIt);
                }
            }

            stringstream ss;
            ss << "[\n";
            for (map<Wt::Dbo::ptr<Echoes::Dbo::Addon>, vector<Wt::Dbo::ptr<Echoes::Dbo::Source>>>::iterator srcPtrByAdoIt = srcListByAddon.begin(); srcPtrByAdoIt != srcListByAddon.end(); ++srcPtrByAdoIt)
            {
                boost::property_tree::ptree addonElem;
                addonElem.put("id", srcPtrByAdoIt->first.id());

                boost::property_tree::ptree srcArr;
                for (vector<Wt::Dbo::ptr<Echoes::Dbo::Source>>::iterator srcPtrIt = srcPtrByAdoIt->second.begin(); srcPtrIt != srcPtrByAdoIt->second.end(); ++srcPtrIt)
                {
                    Wt::Dbo::ptr<Echoes::Dbo::Source> srcPtr = *srcPtrIt;

                    boost::property_tree::ptree srcElem;
                    srcElem.put("id", srcPtr.id());

                    Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SourceParameter>> srpPtrCol = srcPtrByAdoIt->first->sourceParameters;
                    boost::property_tree::ptree srpElem;
                    for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SourceParameter>>::iterator srpPtrIt = srpPtrCol.begin(); srpPtrIt != srpPtrCol.end(); srpPtrIt++)                    
                    {
                        Wt::Dbo::ptr<Echoes::Dbo::SourceParameterValue> spvPtr =  m_session.find<Echoes::Dbo::SourceParameterValue>()
                                .where(QUOTE(TRIGRAM_SOURCE_PARAMETER ID SEP TRIGRAM_SOURCE_PARAMETER ID) " = ?").bind(srpPtrIt->id())
                                .where(QUOTE(TRIGRAM_SOURCE ID SEP TRIGRAM_SOURCE ID) " = ?").bind(srcPtr.id())
                                .where(QUOTE(TRIGRAM_SOURCE_PARAMETER_VALUE SEP "DELETE") " IS NULL");
                        srpElem.put(srpPtrIt->get()->name.toUTF8(), spvPtr->value.toUTF8());
                    }
                    srcElem.put_child("params", srpElem);
                    
                    Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Search>> seaPtrCol = srcPtr->searches;
                    boost::property_tree::ptree seaArr;
                    for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Search>>::iterator seaPtrIt = seaPtrCol.begin(); seaPtrIt != seaPtrCol.end(); seaPtrIt++)
                    {
                        boost::property_tree::ptree seaElem;
                        seaElem.put("id", seaPtrIt->id());
                        seaElem.put("idType", seaPtrIt->get()->searchType.id());

                        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SearchParameter>> sepPtrCol = seaPtrIt->get()->searchType->searchParameters;
                        boost::property_tree::ptree sepElem;
                        for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SearchParameter>>::iterator sepPtrIt = sepPtrCol.begin(); sepPtrIt != sepPtrCol.end(); sepPtrIt++)                    
                        {
                            Wt::Dbo::ptr<Echoes::Dbo::SearchParameterValue> sevPtr =  m_session.find<Echoes::Dbo::SearchParameterValue>()
                                    .where(QUOTE(TRIGRAM_SEARCH_PARAMETER ID SEP TRIGRAM_SEARCH_PARAMETER ID) " = ?").bind(sepPtrIt->id())
                                    .where(QUOTE(TRIGRAM_SEARCH ID SEP TRIGRAM_SEARCH ID) " = ?").bind(seaPtrIt->id())
                                    .where(QUOTE(TRIGRAM_SEARCH_PARAMETER_VALUE SEP "DELETE") " IS NULL");
                            sepElem.put(sepPtrIt->get()->name.toUTF8(), sevPtr->value.toUTF8());
                        }
                        seaElem.put_child("params", sepElem);
                        
                        seaElem.put("period", seaPtrIt->get()->period);
                        
                        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Filter>> filPtrCol = seaPtrIt->get()->filters;
                        boost::property_tree::ptree filArr;
                        for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Filter>>::iterator filPtrIt = filPtrCol.begin(); filPtrIt != filPtrCol.end(); filPtrIt++)
                        {
                            boost::property_tree::ptree filElem;
                            filElem.put("id", filPtrIt->id());
                            filElem.put("idType", filPtrIt->get()->filterType.id());

                            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::FilterParameter>> fpaPtrCol = filPtrIt->get()->filterType->filterParameters;
                            boost::property_tree::ptree fpaElem;
                            for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::FilterParameter>>::iterator fpaPtrIt = fpaPtrCol.begin(); fpaPtrIt != fpaPtrCol.end(); fpaPtrIt++)                    
                            {
                                Wt::Dbo::ptr<Echoes::Dbo::FilterParameterValue> fpvPtr =  m_session.find<Echoes::Dbo::FilterParameterValue>()
                                        .where(QUOTE(TRIGRAM_FILTER_PARAMETER ID SEP TRIGRAM_SEARCH_PARAMETER ID) " = ?").bind(fpaPtrIt->id())
                                        .where(QUOTE(TRIGRAM_FILTER ID SEP TRIGRAM_FILTER ID) " = ?").bind(filPtrIt->id())
                                        .where(QUOTE(TRIGRAM_FILTER_PARAMETER_VALUE SEP "DELETE") " IS NULL");
                                fpaElem.put(fpaPtrIt->get()->name.toUTF8(), fpvPtr->value.toUTF8());
                            }
                            filElem.put_child("params", fpaElem);

                            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationData>> idaPtrCol = filPtrIt->get()->informationDatas;
                            boost::property_tree::ptree idaArr;
                            for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationData>>::iterator idaPtrIt = idaPtrCol.begin(); idaPtrIt != idaPtrCol.end(); idaPtrIt++)
                            {
                                boost::property_tree::ptree idaElem;
                                idaElem.put_value(idaPtrIt->id());
                                idaArr.push_back(std::make_pair("", idaElem));
                            }
                            filElem.put_child("idsIDA", idaArr);
                            
                            filArr.push_back(std::make_pair("", filElem));
                        }
                        seaElem.put_child("filters", filArr);

                        seaArr.push_back(std::make_pair("", seaElem));
                    }
                    srcElem.put_child("searches", seaArr);

                    srcArr.push_back(std::make_pair("", srcElem));
                }
                addonElem.put_child("sources", srcArr);

                boost::property_tree::json_parser::write_json(ss, addonElem);

                if (distance(srcListByAddon.begin(), srcPtrByAdoIt) > 1)
                {
                    string tmp = ss.str().erase(ss.str().size() - 1);
                    ss.str("");
                    ss.clear();
                    ss << tmp;
                    ss << ",\n";
                }
            }
            ss << "]\n";

            responseMsg = ss.str();
            
            res = EReturnCode::OK;
        }        
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = httpCodeToJSON(res, prbPtr);
        }

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

void ProbeResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = getProbesList(responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();
            if (nextElement.empty())
            {
                m_statusCode = getProbe(responseMsg);
            }
            else if (nextElement.compare("json") == 0)
            {
                m_statusCode = getJsonForProbe(responseMsg);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                const string err = "[Probe Resource] bad nextElement";
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

EReturnCode ProbeResource::postProbe(string &responseMsg, const string &sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    long long astId;
    Wt::WString name;

    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            astId = result.get("asset_id");
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
        const string err = "[Probe Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session);

            Wt::Dbo::ptr<Echoes::Dbo::Asset> astPtr = AssetResource::selectAsset(astId, m_session);
            if (astPtr)
            {
                Echoes::Dbo::Probe *newPrb = new Echoes::Dbo::Probe();
                newPrb->asset = astPtr;
                newPrb->name = name;
                newPrb->probePackageParameter = selectProbePackageParameter(astPtr, m_session);

                Wt::Dbo::ptr<Echoes::Dbo::Probe> newPrbPtr = m_session.add<Echoes::Dbo::Probe>(newPrb);
                newPrbPtr.flush();

                res = serialize(newPrbPtr, responseMsg, EReturnCode::CREATED);

                transaction.commit();
            }
            else
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, astPtr);
            }
        }
        catch (Wt::Dbo::Exception const& e)
        {
            res = EReturnCode::SERVICE_UNAVAILABLE;
            responseMsg = httpCodeToJSON(res, e);
        }
    }

    return res;
}

void ProbeResource::processPostRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = postProbe(responseMsg, m_requestData);
    }
    else
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
        const string err = "[Probe Resource] bad nextElement";
        responseMsg = httpCodeToJSON(m_statusCode, err);
    }

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}

EReturnCode ProbeResource::putProbe(string &responseMsg, const string &sRequest)
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
        const string err = "[Probe Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session);

            Wt::Dbo::ptr<Echoes::Dbo::Probe> prbPtr = selectProbe(m_pathElements[1], m_session);

            if (prbPtr)
            {
                if (!name.empty())
                {
                    prbPtr.modify()->name = name;
                }

                res = serialize(prbPtr, responseMsg);
            }
            else
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, prbPtr);
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

void ProbeResource::processPutRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
        const string err = "[Probe Resource] bad nextElement";
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
                m_statusCode = putProbe(responseMsg, m_requestData);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                const string err = "[Probe Resource] bad nextElement";
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

EReturnCode ProbeResource::deleteProbe(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::ptr<Echoes::Dbo::Probe> prbPtr = selectProbe(m_pathElements[1], m_session);

        if (prbPtr)
        {
            prbPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
            res = EReturnCode::NO_CONTENT;
        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = httpCodeToJSON(res, prbPtr);
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

void ProbeResource::processDeleteRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
        const string err = "[Probe Resource] bad nextElement";
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
                m_statusCode = deleteProbe(responseMsg);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                const string err = "[Probe Resource] bad nextElement";
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

