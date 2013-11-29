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

EReturnCode ProbeResource::getProbesList(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    
// FIXME 
//    unsigned long idx = 0;
//    
//    try 
//    {
//        Wt::Dbo::Transaction transaction(m_session);
//        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Asset>> listAssets = this->m_session.find<Echoes::Dbo::Asset> ()
//                .where("\"AST_ORG_ORG_ID\" = ?").bind(this->m_session.user()->organization.id())
//                .where("\"AST_DELETE\" IS NULL");
//
//        responseMsg = "[\n";
//        for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Asset>>::const_iterator i = listAssets.begin(); i != listAssets.end(); ++i)
//        {
//            Wt::Dbo::ptr<Echoes::Dbo::Probe> probe = this->m_session.find<Echoes::Dbo::Probe> ()
//                .where("\"PRB_ID\" = ?").bind(i->get()->probe.id())
//                .where("\"PRB_DELETE\" IS NULL");
//            if (Utils::checkId<Echoes::Dbo::Probe>(probe))
//            {
//                responseMsg += "\t{\n";
//                responseMsg += "\t\t\"id\": " + boost::lexical_cast<string, long long>(probe.id()) + ",\n";
//                responseMsg += "\t\t\"name\": \"" + probe->name.toUTF8() + "\"\n";
//                responseMsg += "\t}";
//                if (idx < listAssets.size())
//                {
//                    responseMsg += ",";
//                }
//                responseMsg += "\n";
//            }
//            ++idx;
//        }
//        responseMsg += "]";
//
//        transaction.commit();
//        res = EReturnCode::OK;
//    } 
//    catch (Wt::Dbo::Exception const& e) 
//    {
//        Wt::log("error") << e.what();
//        res = EReturnCode::SERVICE_UNAVAILABLE;
//        responseMsg = "{\n\t\"message\":\"Service Unavailable\n\"}";
//    }
    
    return res;
}

EReturnCode ProbeResource::getProbe(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    
    try 
    {
        Wt::Dbo::Transaction transaction(m_session);
        Wt::Dbo::ptr<Echoes::Dbo::Probe> probe = this->m_session.find<Echoes::Dbo::Probe>()
                .where("\"PRB_ORG_ORG_ID\" = ?").bind(this->m_session.user()->organization.id())
                .where("\"PRB_ID\" = ?").bind(this->m_pathElements[1])
                .where("\"PRB_DELETE\" IS NULL");

        if (Utils::checkId<Echoes::Dbo::Probe>(probe)) 
        {
            responseMsg += "{\n";
            responseMsg += "\t\"id\": " + boost::lexical_cast<string, long long>(probe.id()) + ",\n";
            responseMsg += "\t\"name\": \"" + probe->name.toUTF8() + "\",\n";

            // Est-ce que les param pkg de cette probe existent ?
            Wt::Dbo::ptr<Echoes::Dbo::ProbePackageParameter> probePackageParameter = this->m_session.find<Echoes::Dbo::ProbePackageParameter>()
                    .where("\"PPP_ID\" = ?").bind(probe->probePackageParameter.id())
                    .where("\"PPP_DELETE\" IS NULL");
            if (Utils::checkId<Echoes::Dbo::ProbePackageParameter>(probePackageParameter))
            {
                responseMsg += "\t\"version\": \"" + probePackageParameter->probeVersion.toUTF8() + "\",\n";
                responseMsg += "\t\"package\": {\n";

                // Est-ce que le pkg de cette probe existent ?
                Wt::Dbo::ptr<Echoes::Dbo::ProbePackage> probePackage = this->m_session.find<Echoes::Dbo::ProbePackage>()
                        .where("\"PPA_ID\" = ?").bind(probePackageParameter->probePackage.id())
                        .where("\"PPA_DELETE\" IS NULL");
                if (Utils::checkId<Echoes::Dbo::ProbePackage>(probePackage))
                {
                    ifstream ifs("/var/www/wt/probe/" + probePackageParameter->probePackage->filename.toUTF8());
                    string content((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
                    responseMsg += "\t\t\"filename\": \"" + probePackageParameter->probePackage->filename.toUTF8() + "\",\n";
                    responseMsg += "\t\t\"content\": \"" + Wt::Utils::base64Encode(content) + "\",\n";
                }
                else
                {
                    responseMsg += "\t\t\"filename\": \"Unknown\",\n";
                    responseMsg += "\t\t\"content\": \"\",\n";
                }
                responseMsg += "\t\t\"version\": \"" + probePackageParameter->packageVersion.toUTF8() + "\"\n";
                responseMsg += "\t}\n";

            }
            else
            {
                responseMsg += "\t\"version\": \"Unknown\",\n";
                responseMsg += "\t\"package\": {\n";
                responseMsg += "\t\t\"filename\": \"Unknown\",\n";
                responseMsg += "\t\t\"content\": \"\",\n";
                responseMsg += "\t\t\"version\": \"Unknown\"\n";
                responseMsg += "\t}\n";
            }
            responseMsg += "}";

            res = EReturnCode::OK;
        } 
        else 
        {
            responseMsg = "{\n\t\"message\":\"Probe not found\"\n}";
            res = EReturnCode::NOT_FOUND;
        }

        transaction.commit();
    } 
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = "{\n\t\"message\":\"Service Unavailable\"\n}";
    }
    
    return res;
}

void ProbeResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";

    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->m_statusCode = getProbesList(responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();

            if(!nextElement.compare(""))
            {
                this->m_statusCode = getProbe(responseMsg);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            m_statusCode = EReturnCode::BAD_REQUEST;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(this->m_statusCode);
    response.out() << responseMsg;
    return;
}

EReturnCode ProbeResource::postProbe(string &responseMsg, const string &sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

// FIXME !
//    try 
//    {
//        Wt::Dbo::Transaction transaction(m_session);
//
//        // Est-ce que l'asset existe ?
//        Wt::Dbo::ptr<Echoes::Dbo::Asset> asset = m_session.find<Echoes::Dbo::Asset>()
//                .where("\"AST_ORG_ORG_ID\" = ?").bind(m_session.user()->organization.id())
//                .where("\"AST_ID\" = ?").bind(m_pathElements[1])
//                .where("\"AST_DELETE\" IS NULL");
//        if (Utils::checkId<Echoes::Dbo::Asset>(asset))
//        {
//            Wt::WString probeName = "Probe_" + m_session.user()->lastName + "_" + asset->name;
//
//            responseMsg += "{\n";
//            // Est-ce que la probe existe pour cet asset ?
//            Wt::Dbo::ptr<Echoes::Dbo::Probe> probe = m_session.find<Echoes::Dbo::Probe>()
//                .where("\"PRB_ID\" = ?").bind(asset->probe.id())
//                .where("\"PRB_DELETE\" IS NULL");
//            if (!Utils::checkId<Echoes::Dbo::Probe>(probe))
//            {
//                Echoes::Dbo::Probe *newProbe = new Echoes::Dbo::Probe();
//                newProbe->name = probeName;
//                asset.modify()->probe = m_session.add<Echoes::Dbo::Probe>(newProbe);
//            }
//            else
//            {
//                asset->probe.modify()->name = probeName;
//            }
//
//            Wt::Dbo::ptr<Echoes::Dbo::ProbePackageParameter> probePackageParameter = m_session.find<Echoes::Dbo::ProbePackageParameter>()
//                    .where("\"PPP_ASA_ASA_ID\" = ?").bind(asset->assetArchitecture.id())
//                    .where("\"PPP_ASD_ASD_ID\" = ?").bind(asset->assetDistribution.id())
//                    .where("\"PPP_ASR_ASR_ID\" = ?").bind(asset->assetRelease.id())
//                    .where("\"PPP_DELETE\" IS NULL")
//                    .orderBy("\"PPP_PROBE_VERSION\" DESC, \"PPP_PACKAGE_VERSION\" DESC")
//                    .limit(1);
//
//            bool releaseChecked = false;
//            Wt::Dbo::ptr<Echoes::Dbo::AssetRelease> ptrAssetRelease;
//            if (!Utils::checkId<Echoes::Dbo::ProbePackageParameter>(probePackageParameter))
//            {
//                string wildcardRelease = asset->assetRelease->name.toUTF8().substr(0,asset->assetRelease->name.toUTF8().find_last_of('.') + 1) + "*";
//                ptrAssetRelease = m_session.find<Echoes::Dbo::AssetRelease>().where("\"ASR_NAME\" = ?").bind(wildcardRelease);
//                probePackageParameter = m_session.find<Echoes::Dbo::ProbePackageParameter>()
//                    .where("\"PPP_ASA_ASA_ID\" = ?").bind(asset->assetArchitecture.id())
//                    .where("\"PPP_ASD_ASD_ID\" = ?").bind(asset->assetDistribution.id())
//                    .where("\"PPP_ASR_ASR_ID\" = ?").bind(ptrAssetRelease.id())
//                    .where("\"PPP_DELETE\" IS NULL")
//                    .orderBy("\"PPP_PROBE_VERSION\" DESC, \"PPP_PACKAGE_VERSION\" DESC")
//                    .limit(1);
//                releaseChecked = true;
//            }
//
//
//            bool architectureChecked = false;
//            Wt::Dbo::ptr<Echoes::Dbo::AssetArchitecture> ptrAssetArchitecture;
//            if (!Utils::checkId<Echoes::Dbo::ProbePackageParameter>(probePackageParameter))
//            {
//                if( (boost::starts_with(asset->assetArchitecture->name.toUTF8(), "i")) && (boost::ends_with(asset->assetArchitecture->name.toUTF8(), "86")) )
//                {
//                    string wildcardArchitecture = "i*86";
//                    ptrAssetArchitecture = m_session.find<Echoes::Dbo::AssetArchitecture>().where("\"ASA_NAME\" = ?").bind(wildcardArchitecture);
//                    probePackageParameter = m_session.find<Echoes::Dbo::ProbePackageParameter>()
//                            .where("\"PPP_ASA_ASA_ID\" = ?").bind(ptrAssetArchitecture.id())
//                            .where("\"PPP_ASD_ASD_ID\" = ?").bind(asset->assetDistribution.id())
//                            .where("\"PPP_ASR_ASR_ID\" = ?").bind(asset->assetRelease.id())
//                            .where("\"PPP_DELETE\" IS NULL")
//                            .orderBy("\"PPP_PROBE_VERSION\" DESC, \"PPP_PACKAGE_VERSION\" DESC")
//                            .limit(1);
//                    architectureChecked = true;
//                }
//            }
//
//            if (releaseChecked
//                && architectureChecked
//                && !Utils::checkId<Echoes::Dbo::ProbePackageParameter>(probePackageParameter)
//               )
//            {
//                probePackageParameter = m_session.find<Echoes::Dbo::ProbePackageParameter>()
//                    .where("\"PPP_ASA_ASA_ID\" = ?").bind(ptrAssetArchitecture.id())
//                    .where("\"PPP_ASD_ASD_ID\" = ?").bind(asset->assetDistribution.id())
//                    .where("\"PPP_ASR_ASR_ID\" = ?").bind(ptrAssetRelease.id())
//                    .where("\"PPP_DELETE\" IS NULL")
//                    .orderBy("\"PPP_PROBE_VERSION\" DESC, \"PPP_PACKAGE_VERSION\" DESC")
//                    .limit(1);
//            }
//
//            asset->probe.modify()->probePackageParameter = probePackageParameter;
//
//            asset->probe.flush();
//
//            responseMsg += "\t\"id\": " + boost::lexical_cast<string, long long>(asset->probe.id()) + ",\n";
//            responseMsg += "\t\"name\": \"" + asset->probe->name.toUTF8() + "\",\n";
//
//            // Est-ce que les param pkg de cette probe existent ?
//            if (Utils::checkId<Echoes::Dbo::ProbePackageParameter>(asset->probe->probePackageParameter))
//            {
//                responseMsg += "\t\"version\": \"" + asset->probe->probePackageParameter->probeVersion.toUTF8() + "\",\n";
//                responseMsg += "\t\"package\": {\n";
//                // Est-ce que le pkg de cette probe existent ?
//                if (Utils::checkId<Echoes::Dbo::ProbePackage>(asset->probe->probePackageParameter->probePackage))
//                {
//                    ifstream ifs("/var/www/wt/probe/" + asset->probe->probePackageParameter->probePackage->filename.toUTF8());
//                    string content((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
//                    responseMsg += "\t\t\"filename\": \"" + asset->probe->probePackageParameter->probePackage->filename.toUTF8() + "\",\n";
//                    responseMsg += "\t\t\"content\": \"" + Wt::Utils::base64Encode(content) + "\",\n";
//                }
//                else
//                {
//                    responseMsg += "\t\t\"filename\": \"Unknown\",\n";
//                    responseMsg += "\t\t\"content\": \"\",\n";
//                }
//                responseMsg += "\t\t\"version\": \"" + asset->probe->probePackageParameter->packageVersion.toUTF8() + "\"\n";
//                responseMsg += "\t}\n";
//
//            }
//            else
//            {
//                responseMsg += "\t\"version\": \"Unknown\",\n";
//                responseMsg += "\t\"package\": {\n";
//                responseMsg += "\t\t\"filename\": \"Unknown\",\n";
//                responseMsg += "\t\t\"content\": \"\",\n";
//                responseMsg += "\t\t\"version\": \"Unknown\"\n";
//                responseMsg += "\t}\n";
//            }
//            responseMsg += "}\n";
//
//            res = EReturnCode::OK;
//        } 
//        else 
//        {
//            res = EReturnCode::NOT_FOUND;
//            responseMsg = "{\n\t\"message\":\"Asset not found\"\n}";
//        }
//        transaction.commit();
//    } 
//    catch (Wt::Dbo::Exception const& e) 
//    {
//        Wt::log("error") << "[Asset Ressource] " << e.what();
//        res = EReturnCode::SERVICE_UNAVAILABLE;
//        responseMsg = "{\n\t\"message\": \"Service Unavailable\"\n}";
//    }
    
    return res;
}

void ProbeResource::processPostRequest(Wt::Http::Response &response)
{
    return;
}

void ProbeResource::processPutRequest(Wt::Http::Response &response)
{
    return;
}

void ProbeResource::processDeleteRequest(Wt::Http::Response &response)
{
    return;
}

void ProbeResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    PublicApiResource::handleRequest(request, response);

    return;
}

