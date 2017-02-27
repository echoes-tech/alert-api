/**
 * @file ProbeResource.cpp
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

#include <string>
#include <Wt/WTimer>
#include "ProbeResource.h"

using namespace std;

int ProbeResource::m_defaultTimer = 61;

ProbeResource::ProbeResource(Echoes::Dbo::Session& session) : PublicApiResource::PublicApiResource(session)
{
}

ProbeResource::~ProbeResource()
{
}

Wt::Dbo::ptr<Echoes::Dbo::Probe> ProbeResource::selectProbe(const long long &prbId, const long long &orgId, Echoes::Dbo::Session &session)
{
    return selectProbe(boost::lexical_cast<string>(prbId), boost::lexical_cast<string>(orgId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::Probe> ProbeResource::selectProbe(const string &prbId, const long long &orgId, Echoes::Dbo::Session &session)
{
    return selectProbe(prbId, boost::lexical_cast<string>(orgId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::Probe> ProbeResource::selectProbe(const string &prbId, const string &orgId, Echoes::Dbo::Session &session)
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
"           WHERE " QUOTE(TRIGRAM_ASSET SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = " + orgId +
"             AND " QUOTE(TRIGRAM_ASSET SEP "DELETE") " IS NULL"
"       )"
"     AND " QUOTE(TRIGRAM_PROBE SEP "DELETE") " IS NULL"
"   LIMIT 1";

    Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Probe>> queryRes = session.query<Wt::Dbo::ptr<Echoes::Dbo::Probe>> (queryStr);

    return queryRes.resultValue();
}

Wt::Dbo::ptr<Echoes::Dbo::ProbePackageParameter> ProbeResource::selectProbePackageParameter(const Wt::Dbo::ptr<Echoes::Dbo::Asset> &astPtr, Echoes::Dbo::Session &session)
{
    Wt::Dbo::ptr<Echoes::Dbo::ProbePackageParameter> pppPtr;

    if (astPtr->assetArchitecture && astPtr->assetDistribution && astPtr->assetRelease)
    {
        pppPtr = session.find<Echoes::Dbo::ProbePackageParameter>()
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
    }

    return pppPtr;
}

EReturnCode ProbeResource::getProbesList(const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);
        string queryStr =
" SELECT prb"
"   FROM " QUOTE("T_PROBE_PRB") " prb"
"   WHERE"
"     " QUOTE(TRIGRAM_PROBE SEP TRIGRAM_ASSET SEP TRIGRAM_ASSET ID) " IN"
"       ("
"         SELECT " QUOTE(TRIGRAM_ASSET ID)
"           FROM " QUOTE("T_ASSET_AST")
"           WHERE " QUOTE(TRIGRAM_ASSET SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = " + boost::lexical_cast<string>(orgId) +
"             AND " QUOTE(TRIGRAM_ASSET SEP "DELETE") " IS NULL"
"       )"
"     AND " QUOTE(TRIGRAM_PROBE SEP "DELETE") " IS NULL"
"   ORDER BY " QUOTE(TRIGRAM_PROBE ID);

        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Probe>> queryRes = m_session.query <Wt::Dbo::ptr<Echoes::Dbo::Probe>> (queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Probe>> prbPtrCol = queryRes.resultList();

        res = serialize<Echoes::Dbo::Probe>(prbPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode ProbeResource::getProbe(const std::vector<std::string> &pathElements, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::Probe> prbPtr = selectProbe(pathElements[1], orgId, m_session);

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

EReturnCode ProbeResource::getAliveProbe(const std::vector<std::string> &pathElements, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        int probeId = boost::lexical_cast<int>(pathElements[1]);

        if (m_mapTimer.find(probeId) == m_mapTimer.end())
        {
            m_mapTimer[probeId] = 0;
        }
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::Probe> prbPtr = selectProbe(pathElements[1], orgId, m_session);
        if (m_mapTimer[probeId] == 0)
        {
            if (prbPtr)
            {
                m_mapTimer[probeId] = prbPtr->timer + 1;
            }
            else
            {
                m_mapTimer[probeId] = m_defaultTimer;
            }
        }
        
        std::string rep = "false";
        if (prbPtr)
        {
            long long   lastHb = prbPtr->lastlog.secsTo(Wt::WDateTime::currentDateTime());

            if (m_mapTimer[probeId] > lastHb)
            {
                rep = "true";
            }
        }
        
        responseMsg = "{\n    \"probe_heartbeat\": "
                + rep
                + ",\n    \"id\": "
                + pathElements[1]
                + "\n}";
        res = EReturnCode::OK;
        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode ProbeResource::getJsonForProbe(const std::vector<std::string> &pathElements, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::Probe> prbPtr = selectProbe(pathElements[1], orgId, m_session);
        if (prbPtr)
        {
            map<Wt::Dbo::ptr<Echoes::Dbo::Addon>, vector<Wt::Dbo::ptr<Echoes::Dbo::Source>>> srcListByAddon;

            for (const Wt::Dbo::ptr<Echoes::Dbo::Plugin> &plgPtr : prbPtr->asset->plugins)
            {
                if (plgPtr->deleteTag.isNull())
                {
                    for (const Wt::Dbo::ptr<Echoes::Dbo::Source> &srcPtr : plgPtr->sources)
                    {
                        if (srcPtr->deleteTag.isNull())
                        {
                            srcListByAddon[srcPtr->addon].push_back(srcPtr);
                        }
                    }
                }
            }

            stringstream ss;
            ss << "[\n";
            for (map<Wt::Dbo::ptr<Echoes::Dbo::Addon>, vector<Wt::Dbo::ptr<Echoes::Dbo::Source>>>::iterator srcPtrByAdoIt = srcListByAddon.begin(); srcPtrByAdoIt != srcListByAddon.end(); ++srcPtrByAdoIt)
            {
                boost::property_tree::ptree addonElem;
                addonElem.put("id", srcPtrByAdoIt->first.id());

                boost::property_tree::ptree srcArr;
                for (const Wt::Dbo::ptr<Echoes::Dbo::Source> &srcPtr : srcPtrByAdoIt->second)
                {
                    boost::property_tree::ptree srcElem;
                    srcElem.put("id", srcPtr.id());

                    boost::property_tree::ptree srpElem;
                    for (const Wt::Dbo::ptr<Echoes::Dbo::SourceParameter> &srpPtr : srcPtrByAdoIt->first->sourceParameters)
                    {
                        if (srpPtr->deleteTag.isNull())
                        {
                            Wt::Dbo::ptr<Echoes::Dbo::SourceParameterValue> spvPtr =  m_session.find<Echoes::Dbo::SourceParameterValue>()
                                    .where(QUOTE(TRIGRAM_SOURCE_PARAMETER ID SEP TRIGRAM_SOURCE_PARAMETER ID) " = ?").bind(srpPtr.id())
                                    .where(QUOTE(TRIGRAM_SOURCE ID SEP TRIGRAM_SOURCE ID) " = ?").bind(srcPtr.id())
                                    .where(QUOTE(TRIGRAM_SOURCE_PARAMETER_VALUE SEP "DELETE") " IS NULL");
                            if (spvPtr)
                            {
                                if (srpPtr->format.toUTF8().compare("integer") == 0)
                                {
                                    srpElem.put(srpPtr->name.toUTF8(), boost::lexical_cast<int>(spvPtr->value.toUTF8()));
                                }
                                else
                                {
                                    srpElem.put(srpPtr->name.toUTF8(), spvPtr->value.toUTF8());
                                }
                            }
                            else
                            {
                                srpElem.put(srpPtr->name.toUTF8(), "");
                            }
                        }
                    }
                    if (srpElem.size() > 0)
                    {
                        srcElem.put_child("params", srpElem);
                    }

                    boost::property_tree::ptree seaArr;
                    for (const Wt::Dbo::ptr<Echoes::Dbo::Search> &seaPtr : srcPtr->searches)
                    {
                        if (seaPtr->deleteTag.isNull())
                        {
                            boost::property_tree::ptree seaElem;
                            seaElem.put("id", seaPtr.id());
                            seaElem.put("idType", seaPtr->searchType.id());

                            boost::property_tree::ptree sepElem;
                            for (const Wt::Dbo::ptr<Echoes::Dbo::SearchParameter> sepPtr : seaPtr->searchType->searchParameters)
                            {
                                if (sepPtr->deleteTag.isNull())
                                {
                                    Wt::Dbo::ptr<Echoes::Dbo::SearchParameterValue> sevPtr =  m_session.find<Echoes::Dbo::SearchParameterValue>()
                                            .where(QUOTE(TRIGRAM_SEARCH_PARAMETER ID SEP TRIGRAM_SEARCH_PARAMETER ID) " = ?").bind(sepPtr.id())
                                            .where(QUOTE(TRIGRAM_SEARCH ID SEP TRIGRAM_SEARCH ID) " = ?").bind(seaPtr.id())
                                            .where(QUOTE(TRIGRAM_SEARCH_PARAMETER_VALUE SEP "DELETE") " IS NULL");
                                    if (sevPtr)
                                    {
                                        if (sepPtr->format.toUTF8().compare("integer") == 0)
                                        {
                                            sepElem.put(sepPtr->name.toUTF8(), boost::lexical_cast<int>(sevPtr->value.toUTF8()));
                                        }
                                        else
                                        {
                                            sepElem.put(sepPtr->name.toUTF8(), sevPtr->value.toUTF8());
                                        }
                                    }
                                    else
                                    {
                                        sepElem.put(sepPtr->name.toUTF8(), "");
                                    }
                                }
                            }
                            if (sepElem.size() > 0)
                            {
                                seaElem.put_child("params", sepElem);
                            }

                            seaElem.put("period", seaPtr->period);

                            boost::property_tree::ptree filArr;
                            for (const Wt::Dbo::ptr<Echoes::Dbo::Filter> &filPtr : seaPtr->filters)
                            {
                                if (filPtr->deleteTag.isNull())
                                {
                                    boost::property_tree::ptree filElem;
                                    filElem.put("id", filPtr.id());
                                    filElem.put("idType", filPtr->filterType.id());

                                    boost::property_tree::ptree fpaElem;
                                    for (const Wt::Dbo::ptr<Echoes::Dbo::FilterParameter> &fpaPtr : filPtr->filterType->filterParameters)
                                    {
                                        if (fpaPtr->deleteTag.isNull())
                                        {
                                            Wt::Dbo::ptr<Echoes::Dbo::FilterParameterValue> fpvPtr =  m_session.find<Echoes::Dbo::FilterParameterValue>()
                                                    .where(QUOTE(TRIGRAM_FILTER_PARAMETER ID SEP TRIGRAM_FILTER_PARAMETER ID) " = ?").bind(fpaPtr.id())
                                                    .where(QUOTE(TRIGRAM_FILTER ID SEP TRIGRAM_FILTER ID) " = ?").bind(filPtr.id())
                                                    .where(QUOTE(TRIGRAM_FILTER_PARAMETER_VALUE SEP "DELETE") " IS NULL");
                                            if (fpvPtr)
                                            {
                                                fpaElem.put(fpaPtr->name.toUTF8(), fpvPtr->value.toUTF8());
                                            }
                                            else
                                            {
                                                fpaElem.put(fpaPtr->name.toUTF8(), "");
                                            }
                                        }
                                    }
                                    if (fpaElem.size() > 0)
                                    {
                                        filElem.put_child("params", fpaElem);
                                    }

                                    boost::property_tree::ptree idaArr;
                                    for (const Wt::Dbo::ptr<Echoes::Dbo::InformationData> &idaPtr : filPtr->informationDatas)
                                    {
                                        if (idaPtr->deleteTag.isNull() && idaPtr->filterFieldIndex > 0)
                                        {
                                            boost::property_tree::ptree idaElem;
                                            idaElem.put_value(idaPtr.id());
                                            idaArr.push_back(std::make_pair("", idaElem));
                                        }
                                    }
                                    if (idaArr.size() > 0)
                                    {
                                        filElem.put_child("idsIDA", idaArr);
                                    }
                                    else
                                    {
                                        filElem.put("idsIDA", "[]");
                                    }

                                    filArr.push_back(std::make_pair("", filElem));
                                }
                            }
                            if (filArr.size() > 0)
                            {
                                seaElem.put_child("filters", filArr);
                            }
                            else
                            {
                                seaElem.put("filters", "[]");
                            }

                            seaArr.push_back(std::make_pair("", seaElem));
                        }
                    }
                    if (seaArr.size() > 0)
                    {
                        srcElem.put_child("searches", seaArr);
                    }
                    else
                    {
                        srcElem.put("searches", "[]");
                    }

                    srcArr.push_back(std::make_pair("", srcElem));
                }
                if (srcArr.size() > 0)
                {
                    addonElem.put_child("sources", srcArr);
                }
                else
                {
                    addonElem.put("sources", "[]");
                }

                boost::property_tree::json_parser::write_json(ss, addonElem);

                if (distance(srcPtrByAdoIt, srcListByAddon.end()) > 1)
                {
                    const string tmp = ss.str().erase(ss.str().size() - 1);
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
    catch (boost::bad_lexical_cast const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }

    return res;
}

EReturnCode ProbeResource::getPackagesForProbe(const std::vector<std::string> &pathElements, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::Probe> prbPtr = selectProbe(pathElements[1], orgId, m_session);
        if (prbPtr)
        {
            stringstream ss;
            ss << "[\n";
            
            Wt::Dbo::ptr<Echoes::Dbo::ProbePackageParameter> pppPtr = prbPtr->probePackageParameter;
            if(pppPtr)
            {            
                Wt::Dbo::ptr<Echoes::Dbo::AddonCommonPackageParameter> cppPtr = m_session.find<Echoes::Dbo::AddonCommonPackageParameter>()
                        .where("\"CPP_ASA_ASA_ID\" = ?").bind(pppPtr->assetArchitecture.id())
                        .where("\"CPP_ASD_ASD_ID\" = ?").bind(pppPtr->assetDistribution.id())
                        .where("\"CPP_ASR_ASR_ID\" = ?").bind(pppPtr->assetRelease.id())
                        .where("\"CPP_ADDON_COMMON_VERSION\" >= ?").bind(pppPtr->minimumVersion)
                        .where("\"CPP_PPP_MINIMUM_VERSION\" <= ?").bind(pppPtr->probeVersion)
                        // Compares major version number: substring('2.1.1', '^(\\d*)\\.') return '2'
                        .where("substring(\"CPP_ADDON_COMMON_VERSION\", '^(\\d*)\\.') = substring(?, '^(\\d*)\\.')").bind(pppPtr->probeVersion)
                        .where("\"CPP_DELETE\" IS NULL")
                        .orderBy("\"CPP_ADDON_COMMON_VERSION\" DESC, \"CPP_PACKAGE_VERSION\" DESC")
                        .limit(1);
                boost::property_tree::ptree cppElem;
                if(cppPtr)
                {
                    cppElem.put<std::string>("name", "common");
                    cppElem.put("version", cppPtr->addonCommonVersion.toUTF8());
                    
                    boost::property_tree::ptree cpaElem;
                    if (cppPtr->addonCommonPackage)
                    {
                        const string filename = cppPtr->addonCommonPackage->filename.toUTF8();
                        cpaElem.put("filename", filename);
                        const string content = PublicApiResource::file2base64(Wt::WApplication::instance()->appRoot() + "probe/addons/common/" + filename);
                        cpaElem.put("content", content);
                        cpaElem.put("version", cppPtr->packageVersion.toUTF8());
                    }
                    cppElem.put_child("package", cpaElem);
                }
                boost::property_tree::json_parser::write_json(ss, cppElem);
                string tmp = ss.str().erase(ss.str().size() - 1);
                ss.str("");
                ss.clear();
                ss << tmp;
                ss << ",\n";

                boost::property_tree::ptree pppElem;
                pppElem.put<std::string>("name", "core");
                pppElem.put("version", pppPtr->probeVersion.toUTF8());

                boost::property_tree::ptree ppaElem;
                if (pppPtr->probePackage)
                {
                    const string filename = pppPtr->probePackage->filename.toUTF8();
                    ppaElem.put("filename", filename);
                    const string content = PublicApiResource::file2base64(Wt::WApplication::instance()->appRoot() + "probe/core/" + filename);
                    ppaElem.put("content", content);
                    ppaElem.put("version", pppPtr->packageVersion.toUTF8());
                }
                pppElem.put_child("package", ppaElem);

                boost::property_tree::json_parser::write_json(ss, pppElem);
                tmp = ss.str().erase(ss.str().size() - 1);
                ss.str("");
                ss.clear();
                ss << tmp;
                ss << ",\n";

                set<long long> adoIdList;
                for (const Wt::Dbo::ptr<Echoes::Dbo::Plugin> &plgPtr : prbPtr->asset->plugins)
                {
                    if (plgPtr->deleteTag.isNull())
                    {
                        for (const Wt::Dbo::ptr<Echoes::Dbo::Source> &srcPtr : plgPtr->sources)
                        {
                            if (srcPtr->deleteTag.isNull())
                            {
                                adoIdList.insert(srcPtr->addon.id());
                            }
                        }
                    }
                }

                for (set<long long>::iterator adoIdIt = adoIdList.begin(); adoIdIt != adoIdList.end(); ++adoIdIt)
                {
                    Wt::Dbo::ptr<Echoes::Dbo::AddonPackageParameter> appPtr = m_session.find<Echoes::Dbo::AddonPackageParameter>()
                            .where("\"APP_ASA_ASA_ID\" = ?").bind(cppPtr->assetArchitecture.id())
                            .where("\"APP_ASD_ASD_ID\" = ?").bind(cppPtr->assetDistribution.id())
                            .where("\"APP_ASR_ASR_ID\" = ?").bind(cppPtr->assetRelease.id())
                            .where("\"APP_CPP_MINIMUM_VERSION\" <= ?").bind(cppPtr->addonCommonVersion)
                            // Compares major version number: substring('2.1.1', '^(\\d*)\\.') return '2'
                            .where("substring(\"APP_ADDON_VERSION\", '^(\\d*)\\.') = substring(?, '^(\\d*)\\.')").bind(cppPtr->addonCommonVersion)
                            .where("\"APP_ADO_ADO_ID\" = ?").bind(*adoIdIt)
                            .where("\"APP_DELETE\" IS NULL")
                            .orderBy("\"APP_ADDON_VERSION\" DESC, \"APP_PACKAGE_VERSION\" DESC")
                            .limit(1);
                    boost::property_tree::ptree appElem;
                    if(appPtr)
                    {
                        const string &name = appPtr->addon->name.toUTF8();
                        appElem.put("name", name);
                        appElem.put("version", appPtr->addonVersion.toUTF8());

                        boost::property_tree::ptree apaElem;
                        if (appPtr->addonPackage)
                        {
                            const string &filename = appPtr->addonPackage->filename.toUTF8();
                            apaElem.put("filename", filename);
                            const string content = PublicApiResource::file2base64(Wt::WApplication::instance()->appRoot() + "probe/addons/" + name + "/" + filename);
                            apaElem.put("content", content);
                            apaElem.put("version", appPtr->packageVersion.toUTF8());
                        }
                        appElem.put_child("package", apaElem);
                    }
                    boost::property_tree::json_parser::write_json(ss, appElem);

                    if (distance(adoIdList.begin(), adoIdIt) > 1)
                    {
                        tmp = ss.str().erase(ss.str().size() - 1);
                        ss.str("");
                        ss.clear();
                        ss << tmp;
                        ss << ",\n";
                    }
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

EReturnCode ProbeResource::processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        res = getProbesList(orgId, responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.empty())
            {
                res = getProbe(pathElements, orgId, responseMsg);
            }
            else if (nextElement.compare("json") == 0)
            {
                res = getJsonForProbe(pathElements, orgId, responseMsg);
            }
            else if (nextElement.compare("packages") == 0)
            {
                res = getPackagesForProbe(pathElements, orgId, responseMsg);
            }
            else if (nextElement.compare("alive") == 0)
            {
                res = getAliveProbe(pathElements, orgId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Probe Resource] bad nextElement";
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

EReturnCode ProbeResource::postProbe(const string& sRequest, const long long &orgId, string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    long long astId;
    Wt::WString name;
    int timer;

    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            astId = result.get("asset_id");
            name = result.get("name");
            timer = 60;
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
            Wt::Dbo::Transaction transaction(m_session, true);

            Wt::Dbo::ptr<Echoes::Dbo::Asset> astPtr = AssetResource::selectAsset(astId, orgId, m_session);
            if (astPtr)
            {
                Echoes::Dbo::Probe *newPrb = new Echoes::Dbo::Probe();
                newPrb->asset = astPtr;
                newPrb->name = name;
                newPrb->timer = timer;
                newPrb->lastlog = Wt::WDateTime::currentDateTime().addSecs(-61);

                Wt::Dbo::ptr<Echoes::Dbo::ProbePackageParameter> pppPtr = selectProbePackageParameter(astPtr, m_session);
                if (pppPtr)
                {
                    newPrb->probePackageParameter = pppPtr;
                } 

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

EReturnCode ProbeResource::processPostRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        res = postProbe(sRequest, orgId, responseMsg);
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Probe Resource] bad nextElement";
        responseMsg = httpCodeToJSON(res, err);
    }

    return res;
}

EReturnCode ProbeResource::putProbe(const std::vector<std::string> &pathElements, const string &sRequest, const long long &orgId, string &responseMsg)
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
            Wt::Dbo::Transaction transaction(m_session, true);

            Wt::Dbo::ptr<Echoes::Dbo::Probe> prbPtr = selectProbe(pathElements[1], orgId, m_session);

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

EReturnCode ProbeResource::processPutRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        const string err = "[Probe Resource] bad nextElement";
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
                res = putProbe(pathElements, sRequest, orgId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Probe Resource] bad nextElement";
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

EReturnCode ProbeResource::deleteProbe(const std::vector<std::string> &pathElements, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::Probe> prbPtr = selectProbe(pathElements[1], orgId, m_session);

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

EReturnCode ProbeResource::processDeleteRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        const string err = "[Probe Resource] bad nextElement";
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
                res = deleteProbe(pathElements, orgId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Probe Resource] bad nextElement";
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
