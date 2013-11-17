 /* 
 * API Asset Resource
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

#include "AssetResource.h"

using namespace std;

AssetResource::AssetResource() : PublicApiResource::PublicApiResource()
{
}

AssetResource::~AssetResource()
{
}

unsigned short AssetResource::getAssetsList(string &responseMsg)
{
    unsigned short res = 500;
    unsigned long idx = 0;
    
    try 
    {
        Wt::Dbo::Transaction transaction(m_session);
        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Asset>> listAssets = this->m_session.find<Echoes::Dbo::Asset> ()
                .where("\"AST_ORG_ORG_ID\" = ? AND \"AST_DELETE\" IS NULL")
                .bind(this->m_session.user()->organization.id());

        responseMsg = "[\n";
        for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Asset> >::const_iterator i = listAssets.begin(); i != listAssets.end(); ++i)
        {
            responseMsg += "\t{\n";
            responseMsg += "\t\t\"id\": " + boost::lexical_cast<string, long long>(i->id()) + ",\n";
            responseMsg += "\t\t\"name\": \"" + i->get()->name.toUTF8() + "\"\n";
            responseMsg += "\t}";
            if (++idx < listAssets.size())
            {
                responseMsg += ",";
            }
            responseMsg += "\n";
        }
        responseMsg += "]";

        transaction.commit();
        res = 200;
    } 
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\n\t\"message\":\"Service Unavailable\n\"}";
    }
    
    return res;
}

unsigned short AssetResource::getAsset(string &responseMsg)
{
    unsigned short res = 500;
    
    try 
    {
        Wt::Dbo::Transaction transaction(m_session);
        Wt::Dbo::ptr<Echoes::Dbo::Asset> asset = this->m_session.find<Echoes::Dbo::Asset>()
                .where("\"AST_ORG_ORG_ID\" = ? AND \"AST_ID\" = ? AND \"AST_DELETE\" IS NULL")
                .bind(this->m_session.user()->organization.id())
                .bind(this->m_pathElements[1]);
        

        if (Utils::checkId<Echoes::Dbo::Asset>(asset)) 
        {
            responseMsg += "{\n";
            responseMsg += "\t\"id\": " + boost::lexical_cast<string, long long>(asset.id()) + ",\n";
            responseMsg += "\t\"name\": \"" + asset->name.toUTF8() + "\",\n";
            Wt::Dbo::ptr<Echoes::Dbo::AssetArchitecture> assetArchitecture = this->m_session.find<Echoes::Dbo::AssetArchitecture> ()
                .where("\"ASA_ID\" = ? AND \"ASA_DELETE\" IS NULL")
                .bind(asset->assetArchitecture.id());
            if (Utils::checkId<Echoes::Dbo::AssetArchitecture>(assetArchitecture)) 
            {
                responseMsg += "\t\"architecture\": \"" + assetArchitecture->name.toUTF8() + "\",\n";
            }
            else 
            {
                responseMsg += "\t\"architecture\": \"Unknown\",\n";
            }
            responseMsg += "\t\"distribution\": {\n";
            Wt::Dbo::ptr<Echoes::Dbo::AssetDistribution> assetDistribution = this->m_session.find<Echoes::Dbo::AssetDistribution> ()
                .where("\"ASD_ID\" = ? AND \"ASD_DELETE\" IS NULL")
                .bind(asset->assetDistribution.id());
            if (Utils::checkId<Echoes::Dbo::AssetDistribution>(assetDistribution)) 
            {
                responseMsg += "\t\t\"name\": \"" + assetDistribution->name.toUTF8() + "\",\n";
            }
            else 
            {
                responseMsg += "\t\t\"name\": \"Unknown\",\n";
            }
            Wt::Dbo::ptr<Echoes::Dbo::AssetRelease> assetRelease = this->m_session.find<Echoes::Dbo::AssetRelease> ()
                .where("\"ASR_ID\" = ? AND \"ASR_DELETE\" IS NULL")
                .bind(asset->assetRelease.id());
            if (Utils::checkId<Echoes::Dbo::AssetRelease>(assetRelease)) 
            {
                responseMsg += "\t\t\"release\": \"" + assetRelease->name.toUTF8() + "\"\n";
            }
            else 
            {
                responseMsg += "\t\t\"release\": \"Unknown\"\n";
            }
            responseMsg += "\t}\n";
            responseMsg += "}";

            res = 200;
        } 
        else 
        {
            responseMsg = "{\n\t\"message\":\"Asset not found\"\n}";
            res = 404;
        }

        transaction.commit();
    } 
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\n\t\"message\":\"Service Unavailable\"\n}";
    }
    
    return res;
}

// TSA FPO : still looking where it is used ????
//unsigned short AssetResource::getKeyValueForInformation(string &responseMsg)
//{
//    unsigned short res = 500;
//    unsigned idx = 0;
//    try
//    {
//        Wt::Dbo::Transaction transaction(m_session);
//
//        Wt::Dbo::ptr<Echoes::Dbo::Information> ptrInfoKey = m_session.find<Echoes::Dbo::Information>()
//                .where("\"SRC_ID\" = ?").bind(this->m_pathElements[5])
//                .where("\"SEA_ID\" = ?").bind(this->m_pathElements[7])
//                .where("\"PLG_ID_PLG_ID\" = ?").bind(this->m_pathElements[3])
//                .where("\"INF_VALUE_NUM\" = ?").bind(this->m_pathElements[9])
//                .limit(1);
//
//        if (!ptrInfoKey)
//        {
//            res = 404;
//            responseMsg = "{\"message\":\"Information not found\"}";
//            return res;
//        }
//
//        string queryString = 
//        "SELECT iva FROM \"T_INFORMATION_VALUE_IVA\" iva WHERE \"IVA_ID\" IN ( SELECT \"IVA_ID\" FROM"
//        "("
//        "SELECT DISTINCT ON (\"IVA_VALUE\") \"IVA_VALUE\", \"IVA_ID\" FROM"
//        "(" "SELECT iva.\"IVA_VALUE\", iva.\"IVA_ID\" FROM \"T_INFORMATION_VALUE_IVA\" iva"
//        " WHERE \"SEA_ID\" = " + boost::lexical_cast<string>(this->m_pathElements[7]) + 
//        " AND \"SRC_ID\" = " + boost::lexical_cast<string>(this->m_pathElements[5]) + 
//        " AND \"PLG_ID_PLG_ID\" = " + boost::lexical_cast<string>(this->m_pathElements[3]) + 
//        " AND \"INF_VALUE_NUM\" = " + boost::lexical_cast<string>(this->m_pathElements[9]) + 
//        " AND \"IVA_AST_AST_ID\" = " + boost::lexical_cast<string>(this->m_pathElements[1]) +
//        " ORDER BY \"IVA_ID\" DESC LIMIT 50) sr"
//        " ) sr_sr"
//        ")";
//
//        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> collPtrIva = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>>(queryString);
//
//        if(collPtrIva.size() > 0)
//        {
//            responseMsg = "[\n";
//            for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationValue> >::const_iterator i = collPtrIva.begin(); i != collPtrIva.end(); i++)
//            { 
//                i->modify()->setId(i->id());
//                responseMsg += i->get()->toJSON();
//                 ++idx;
//                if(collPtrIva.size()-idx > 0)
//                {
//                    responseMsg.replace(responseMsg.size()-1, 1, "");
//                    responseMsg += ",\n";
//                }
//            }
//            responseMsg += "]\n";
//            res = 200;
//        }
//        else
//        {
//            res = 404;
//            responseMsg = "{\"message\":\"Information value not found\"}";
//        }
//    }
//    catch (Wt::Dbo::Exception const &e)
//    {
//        Wt::log("error") << e.what();
//        res = 503;
//        responseMsg = "{\"message\":\"Service Unavailable\"}";
//    }
//    return res;
//}


unsigned short AssetResource::getPluginsListForAsset(string &responseMsg)
{
    unsigned short res = 500;
    
    try 
    {
        Wt::Dbo::Transaction transaction(m_session);
        Wt::Dbo::ptr<Echoes::Dbo::Asset> asset = this->m_session.find<Echoes::Dbo::Asset>()
                .where("\"AST_ORG_ORG_ID\" = ?").bind(this->m_session.user()->organization.id())
                .where("\"AST_ID\" = ?").bind(this->m_pathElements[1])
                .where("\"AST_DELETE\" IS NULL");

        if (Utils::checkId<Echoes::Dbo::Asset>(asset)) 
        {
            responseMsg = "[\n";
            responseMsg += "\t{\n";
            responseMsg += "\t\t\"id\": 1,\n";
            responseMsg += "\t\t\"name\": \"Linux-System.json\"\n";
            responseMsg += "\t}\n";
            responseMsg += "]";
            res = 200;
        } 
        else 
        {
            responseMsg = "{\n\t\"message\":\"Asset not found\"\n}";
            res = 404;
        }

        transaction.commit();
    } 
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\n\t\"message\":\"Service Unavailable\"\n}";
    }
    
    return res;
}

unsigned short AssetResource::getProbesListForAsset(string &responseMsg)
{
    unsigned short res = 500;

    try 
    {
        Wt::Dbo::Transaction transaction(m_session);
        Wt::Dbo::ptr<Echoes::Dbo::Asset> asset = this->m_session.find<Echoes::Dbo::Asset>()
                .where("\"AST_ORG_ORG_ID\" = ?").bind(this->m_session.user()->organization.id())
                .where("\"AST_ID\" = ?").bind(this->m_pathElements[1])
                .where("\"AST_DELETE\" IS NULL");

        if (Utils::checkId<Echoes::Dbo::Asset>(asset)) 
        {
            Wt::Dbo::ptr<Echoes::Dbo::Probe> probe = this->m_session.find<Echoes::Dbo::Probe> ()
                .where("\"PRB_ID\" = ?").bind(asset->probe.id())
                .where("\"PRB_DELETE\" IS NULL");
            if (Utils::checkId<Echoes::Dbo::Probe>(probe)) 
            {
                responseMsg += "[\n";
                responseMsg += "\t{\n";
                responseMsg += "\t\t\"id\": " + boost::lexical_cast<string, long long>(probe.id()) + ",\n";
                responseMsg += "\t\t\"name\": \"" + probe->name.toUTF8() + "\"\n";
                responseMsg += "\t}\n";
                responseMsg += "]";

                res = 200;
            }
            else 
            {
                responseMsg = "{\n\t\"message\":\"Probe not found\"\n}";
                res = 404;
            }
        } 
        else 
        {
            responseMsg = "{\n\t\"message\":\"Asset not found\"\n}";
            res = 404;
        }

        transaction.commit();
    } 
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\n\t\"message\":\"Service Unavailable\"\n}";
    }
    
    return res;
}

void AssetResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";

    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->m_statusCode = getAssetsList(responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();
            if(!nextElement.compare(""))
            {
                this->m_statusCode = getAsset(responseMsg);
            }
            else if(!nextElement.compare("plugins"))
            {
                nextElement = getNextElementFromPath();
                if(!nextElement.compare(""))
                {
                    this->m_statusCode = getPluginsListForAsset(responseMsg);
                }
                else
                {
//                    boost::lexical_cast<unsigned long long>(nextElement);
//
//                    nextElement = getNextElementFromPath();
//                    if (!nextElement.compare("sources"))
//                    {
//                        nextElement = getNextElementFromPath();
//                      
//                        boost::lexical_cast<unsigned long long>(nextElement);
//                        nextElement = getNextElementFromPath();
//                        if (!nextElement.compare("searches"))
//                        {
//                            nextElement = getNextElementFromPath();
//                           
//                            boost::lexical_cast<unsigned long long>(nextElement);
//                            nextElement = getNextElementFromPath();
//                            if (!nextElement.compare("inf_values"))
//                            {
//                                nextElement = getNextElementFromPath();
//                                boost::lexical_cast<unsigned long long>(nextElement);
//                                nextElement = getNextElementFromPath();
//
//                                if (!nextElement.compare("informations"))
//                                {
//                                    this->m_statusCode = getKeyValueForInformation(responseMsg);
//                                }
//                                else
//                                {
//                                    this->m_statusCode = 400;
//                                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                                }
//                            }
//                            else
//                            {
                                this->m_statusCode = 400;
                                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                            }
//                        }   
//                    }
                }
            }
            else if(!nextElement.compare("probes"))
            {
                this->m_statusCode = getProbesListForAsset(responseMsg);
            }
            else if(!nextElement.compare("alias"))
            {
                this->m_statusCode = getAliasForAsset(responseMsg);
            }
            else
            {
                this->m_statusCode = 400;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            this->m_statusCode = 400;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(this->m_statusCode);
    response.out() << responseMsg;
    return;
}

unsigned short AssetResource::postProbeForAsset(string &responseMsg, const string &sRequest)
{
    unsigned short res = 500;

    try 
    {
        Wt::Dbo::Transaction transaction(m_session);

        // Est-ce que l'asset existe ?
        Wt::Dbo::ptr<Echoes::Dbo::Asset> asset = this->m_session.find<Echoes::Dbo::Asset>()
                .where("\"AST_ORG_ORG_ID\" = ?").bind(this->m_session.user()->organization.id())
                .where("\"AST_ID\" = ?").bind(this->m_pathElements[1])
                .where("\"AST_DELETE\" IS NULL");
        if (Utils::checkId<Echoes::Dbo::Asset>(asset))
        {
            Wt::WString probeName = "Probe_" + this->m_session.user()->lastName + "_" + asset->name;

            responseMsg += "{\n";
            // Est-ce que la probe existe pour cet asset ?
            Wt::Dbo::ptr<Echoes::Dbo::Probe> probe = this->m_session.find<Echoes::Dbo::Probe>()
                .where("\"PRB_ID\" = ?").bind(asset->probe.id())
                .where("\"PRB_DELETE\" IS NULL");
            if (!Utils::checkId<Echoes::Dbo::Probe>(probe))
            {
                Echoes::Dbo::Probe *newProbe = new Echoes::Dbo::Probe();
                newProbe->name = probeName;
                asset.modify()->probe = this->m_session.add<Echoes::Dbo::Probe>(newProbe);
            }
            else
            {
                asset->probe.modify()->name = probeName;
            }

            Wt::Dbo::ptr<Echoes::Dbo::ProbePackageParameter> probePackageParameter = this->m_session.find<Echoes::Dbo::ProbePackageParameter>()
                    .where("\"PPP_ASA_ASA_ID\" = ?").bind(asset->assetArchitecture.id())
                    .where("\"PPP_ASD_ASD_ID\" = ?").bind(asset->assetDistribution.id())
                    .where("\"PPP_ASR_ASR_ID\" = ?").bind(asset->assetRelease.id())
                    .where("\"PPP_DELETE\" IS NULL")
                    .orderBy("\"PPP_PROBE_VERSION\" DESC, \"PPP_PACKAGE_VERSION\" DESC")
                    .limit(1);

            bool releaseChecked = false;
            Wt::Dbo::ptr<Echoes::Dbo::AssetRelease> ptrAssetRelease;
            if (!Utils::checkId<Echoes::Dbo::ProbePackageParameter>(probePackageParameter))
            {
                string wildcardRelease = asset->assetRelease->name.toUTF8().substr(0,asset->assetRelease->name.toUTF8().find_last_of('.') + 1) + "*";
                ptrAssetRelease = this->m_session.find<Echoes::Dbo::AssetRelease>().where("\"ASR_NAME\" = ?").bind(wildcardRelease);
                probePackageParameter = this->m_session.find<Echoes::Dbo::ProbePackageParameter>()
                    .where("\"PPP_ASA_ASA_ID\" = ?").bind(asset->assetArchitecture.id())
                    .where("\"PPP_ASD_ASD_ID\" = ?").bind(asset->assetDistribution.id())
                    .where("\"PPP_ASR_ASR_ID\" = ?").bind(ptrAssetRelease.id())
                    .where("\"PPP_DELETE\" IS NULL")
                    .orderBy("\"PPP_PROBE_VERSION\" DESC, \"PPP_PACKAGE_VERSION\" DESC")
                    .limit(1);
                releaseChecked = true;
            }


            bool architectureChecked = false;
            Wt::Dbo::ptr<Echoes::Dbo::AssetArchitecture> ptrAssetArchitecture;
            if (!Utils::checkId<Echoes::Dbo::ProbePackageParameter>(probePackageParameter))
            {
                if( (boost::starts_with(asset->assetArchitecture->name.toUTF8(), "i")) && (boost::ends_with(asset->assetArchitecture->name.toUTF8(), "86")) )
                {
                    string wildcardArchitecture = "i*86";
                    ptrAssetArchitecture = this->m_session.find<Echoes::Dbo::AssetArchitecture>().where("\"ASA_NAME\" = ?").bind(wildcardArchitecture);
                    probePackageParameter = this->m_session.find<Echoes::Dbo::ProbePackageParameter>()
                            .where("\"PPP_ASA_ASA_ID\" = ?").bind(ptrAssetArchitecture.id())
                            .where("\"PPP_ASD_ASD_ID\" = ?").bind(asset->assetDistribution.id())
                            .where("\"PPP_ASR_ASR_ID\" = ?").bind(asset->assetRelease.id())
                            .where("\"PPP_DELETE\" IS NULL")
                            .orderBy("\"PPP_PROBE_VERSION\" DESC, \"PPP_PACKAGE_VERSION\" DESC")
                            .limit(1);
                    architectureChecked = true;
                }
            }

            if (releaseChecked
                && architectureChecked
                && !Utils::checkId<Echoes::Dbo::ProbePackageParameter>(probePackageParameter)
               )
            {
                probePackageParameter = this->m_session.find<Echoes::Dbo::ProbePackageParameter>()
                    .where("\"PPP_ASA_ASA_ID\" = ?").bind(ptrAssetArchitecture.id())
                    .where("\"PPP_ASD_ASD_ID\" = ?").bind(asset->assetDistribution.id())
                    .where("\"PPP_ASR_ASR_ID\" = ?").bind(ptrAssetRelease.id())
                    .where("\"PPP_DELETE\" IS NULL")
                    .orderBy("\"PPP_PROBE_VERSION\" DESC, \"PPP_PACKAGE_VERSION\" DESC")
                    .limit(1);
            }

            asset->probe.modify()->probePackageParameter = probePackageParameter;

            asset->probe.flush();

            responseMsg += "\t\"id\": " + boost::lexical_cast<string, long long>(asset->probe.id()) + ",\n";
            responseMsg += "\t\"name\": \"" + asset->probe->name.toUTF8() + "\",\n";

            // Est-ce que les param pkg de cette probe existent ?
            if (Utils::checkId<Echoes::Dbo::ProbePackageParameter>(asset->probe->probePackageParameter))
            {
                responseMsg += "\t\"version\": \"" + asset->probe->probePackageParameter->probeVersion.toUTF8() + "\",\n";
                responseMsg += "\t\"package\": {\n";
                // Est-ce que le pkg de cette probe existent ?
                if (Utils::checkId<Echoes::Dbo::ProbePackage>(asset->probe->probePackageParameter->probePackage))
                {
                    ifstream ifs("/var/www/wt/probe/" + asset->probe->probePackageParameter->probePackage->filename.toUTF8());
                    string content((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
                    responseMsg += "\t\t\"filename\": \"" + asset->probe->probePackageParameter->probePackage->filename.toUTF8() + "\",\n";
                    responseMsg += "\t\t\"content\": \"" + Wt::Utils::base64Encode(content) + "\",\n";
                }
                else
                {
                    responseMsg += "\t\t\"filename\": \"Unknown\",\n";
                    responseMsg += "\t\t\"content\": \"\",\n";
                }
                responseMsg += "\t\t\"version\": \"" + asset->probe->probePackageParameter->packageVersion.toUTF8() + "\"\n";
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
            responseMsg += "}\n";

            res = 200;
        } 
        else 
        {
            res = 404;
            responseMsg = "{\n\t\"message\":\"Asset not found\"\n}";
        }
        transaction.commit();
    } 
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << "[Asset Ressource] " << e.what();
        res = 503;
        responseMsg = "{\n\t\"message\": \"Service Unavailable\"\n}";
    }
    
    return res;
}

unsigned short AssetResource::getAliasForAsset(std::string  &responseMsg)
{
    unsigned short res = 500;
    if (this->m_role.empty())
    {
        res = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        return res;
    }

    if (this->m_media.empty())
    {
        res = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        return res;
    }
    
    try 
    {
        Wt::Dbo::Transaction transaction(m_session);
        Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasAsset> aliasAsset = this->m_session.find<Echoes::Dbo::AlertMessageAliasAsset>()
                .where("\"AAA_DELETE\" IS NULL")
                .where("\"URO_ID_URO_ID\" = ?").bind(this->m_role)
                .where("\"MED_ID_MED_ID\" = ?").bind(this->m_media)
                .where("\"AST_ID_AST_ID\" = ?").bind(this->m_pathElements[1]);
        if (aliasAsset)
        {
            responseMsg = aliasAsset->toJSON();
            res = 200;
            transaction.commit();
        }
        else
        {
            res = 404;
            responseMsg = "{\n\t\"message\":\"Alias not found\"\n}";
        }
        
        
    } 
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << "[Asset Ressource] " << e.what();
        res = 503;
        responseMsg = "{\n\t\"message\": \"Service Unavailable\"\n}";
    }
    
    
    return res;
}

void AssetResource::processPostRequest(Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";

    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->m_statusCode = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }
    else
    {   
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();

            if(!nextElement.compare("probes"))
            {
                this->m_statusCode = postProbeForAsset(responseMsg, m_requestData);
            }
            else
            {
                this->m_statusCode = 400;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            this->m_statusCode = 400;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(this->m_statusCode);
    response.out() << responseMsg;
    return;
}

unsigned short AssetResource::putAsset(string &responseMsg, const string &sRequest)
{
    unsigned short res = 500;
    Wt::WString arch = "Unknown", distribName = "Unknown", distribRelease = "Unknown";

    if(sRequest.compare(""))
    {    
        try
        {
            Wt::Json::Object result;

            Wt::Json::parse(sRequest, result);

            Wt::Json::Object distrib = result.get("distribution");
            arch = result.get("architecture");
            distribName = distrib.get("name");
            distribRelease = distrib.get("release");
        }
        catch (Wt::Json::ParseError const& e)
        {
            Wt::log("warning") << "[Asset Ressource] Problems parsing JSON: " << sRequest;
            res = 400;
            responseMsg = "{\n\t\"message\": \"Problems parsing JSON\"\n}";
        }
        catch (Wt::Json::TypeException const& e)
        {
            Wt::log("warning") << "[Asset Ressource] Problems parsing JSON: " << sRequest;
            res = 400;
            responseMsg = "{\n\t\"message\": \"Problems parsing JSON\"\n}";
        }
    }
    else
    {
        res = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }


    if(!responseMsg.compare(""))
    {
        try 
        {
            Wt::Dbo::Transaction transaction(m_session);
            Wt::Dbo::ptr<Echoes::Dbo::Asset> asset = this->m_session.find<Echoes::Dbo::Asset> ()
                    .where("\"AST_ORG_ORG_ID\" = ?").bind(this->m_session.user()->organization.id())
                    .where("\"AST_ID\" = ?").bind(this->m_pathElements[1])
                    .where("\"AST_DELETE\" IS NULL");

            if (Utils::checkId<Echoes::Dbo::Asset>(asset)) 
            {
                Wt::Dbo::ptr<Echoes::Dbo::AssetArchitecture> assetArchitecture = this->m_session.find<Echoes::Dbo::AssetArchitecture>()
                        .where("\"ASA_NAME\" = ?").bind(arch);
                if (!Utils::checkId<Echoes::Dbo::AssetArchitecture> (assetArchitecture)) 
                {
                    Echoes::Dbo::AssetArchitecture *newAssetArchitecture = new Echoes::Dbo::AssetArchitecture();
                    newAssetArchitecture->name = arch;
                    assetArchitecture = this->m_session.add<Echoes::Dbo::AssetArchitecture>(newAssetArchitecture);
                    assetArchitecture.flush();
                }
                asset.modify()->assetArchitecture = assetArchitecture;
                
                Wt::Dbo::ptr<Echoes::Dbo::AssetDistribution> assetDistribution = this->m_session.find<Echoes::Dbo::AssetDistribution>()
                        .where("\"ASD_NAME\" = ?").bind(distribName);
                if (!Utils::checkId<Echoes::Dbo::AssetDistribution> (assetDistribution)) 
                {
                    Echoes::Dbo::AssetDistribution *newAssetDistribution = new Echoes::Dbo::AssetDistribution();
                    newAssetDistribution->name = distribName;
                    assetDistribution = this->m_session.add<Echoes::Dbo::AssetDistribution>(newAssetDistribution);
                    assetDistribution.flush();
                }
                asset.modify()->assetDistribution = assetDistribution;
                
                Wt::Dbo::ptr<Echoes::Dbo::AssetRelease> assetRelease = this->m_session.find<Echoes::Dbo::AssetRelease>()
                        .where("\"ASR_NAME\" = ?").bind(distribRelease);
                if (!Utils::checkId<Echoes::Dbo::AssetRelease> (assetRelease)) 
                {
                    Echoes::Dbo::AssetRelease *newAssetRelease = new Echoes::Dbo::AssetRelease();
                    newAssetRelease->name = distribRelease;
                    assetRelease = this->m_session.add<Echoes::Dbo::AssetRelease>(newAssetRelease);
                    assetRelease.flush();
                }
                asset.modify()->assetRelease = assetRelease;

                res = 200;
            } 
            else 
            {
                res = 404;
                responseMsg = "{\n\t\"message\":\"Asset not found\"\n}";
            }
            transaction.commit();
        } 
        catch (Wt::Dbo::Exception const& e) 
        {
            Wt::log("error") << "[Asset Ressource] " << e.what();
            res = 503;
            responseMsg = "{\n\t\"message\": \"Service Unavailable\"\n}";
        }
    }
    
    return res;
}

unsigned short AssetResource::putAlias(string &responseMsg, const string &sRequest)
{
    unsigned short res = 500;
    Wt::WString sRole = "Unknown";
    Wt::WString sMedia = "Unknown";
    Wt::WString sValue = "Unknown";
    if(sRequest.compare(""))
    {    
        try
        {
            Wt::Json::Object result;

            Wt::Json::parse(sRequest, result);

            Wt::Json::Object alias = result.get("alias");
            sRole = alias.get("role");
            sMedia = alias.get("media");
            sValue = alias.get("value");
        }
        catch (Wt::Json::ParseError const& e)
        {
            Wt::log("warning") << "[Asset Ressource] Problems parsing JSON: " << sRequest;
            res = 400;
            responseMsg = "{\n\t\"message\": \"Problems parsing JSON\"\n}";
        }
        catch (Wt::Json::TypeException const& e)
        {
            Wt::log("warning") << "[Asset Ressource] Problems parsing JSON.: " << sRequest;
            res = 400;
            responseMsg = "{\n\t\"message\": \"Problems parsing JSON.\"\n}";
        }
    }
    else
    {
        res = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }


    if(!responseMsg.compare(""))
    {
        try 
        {
            Wt::Dbo::Transaction transaction(m_session);
            Wt::Dbo::ptr<Echoes::Dbo::Asset> ptrAsset = this->m_session.find<Echoes::Dbo::Asset> ()
                    .where("\"AST_ORG_ORG_ID\" = ?").bind(this->m_session.user()->organization.id())
                    .where("\"AST_ID\" = ?").bind(this->m_pathElements[1])
                    .where("\"AST_DELETE\" IS NULL");

            if (Utils::checkId<Echoes::Dbo::Asset>(ptrAsset)) 
            {
                Wt::Dbo::ptr<Echoes::Dbo::UserRole> ptrRole = this->m_session.find<Echoes::Dbo::UserRole>()
                        .where("\"URO_ID\" = ?").bind(sRole)
                        .where("\"URO_DELETE\" IS NULL");
                
                if (!ptrRole)
                {
                    res = 404;
                    responseMsg = "{\n\t\"message\":\"Role not found\"\n}";
                    return res;
                }
                
                Wt::Dbo::ptr<Echoes::Dbo::Media> ptrMedia = this->m_session.find<Echoes::Dbo::Media>()
                        .where("\"MED_ID\" = ?").bind(sMedia)
                        .where("\"MED_DELETE\" IS NULL");
                
                if (!ptrMedia)
                {
                    res = 404;
                    responseMsg = "{\n\t\"message\":\"Media not found\"\n}";
                    return res;
                }
                
                 
                Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasAsset> ptrAssetAlias = this->m_session.find<Echoes::Dbo::AlertMessageAliasAsset>()
                        .where("\"URO_ID_URO_ID\" = ?").bind(sRole)
                        .where("\"AST_ID_AST_ID\" = ?").bind(ptrAsset.id())
                        .where("\"MED_ID_MED_ID\" = ?").bind(sMedia);
                if (ptrAssetAlias) 
                {
                    ptrAssetAlias.modify()->alias = sValue;
                }
                else
                {
                    Echoes::Dbo::AlertMessageAliasAsset *newAssetAlias = new Echoes::Dbo::AlertMessageAliasAsset();
                    newAssetAlias->pk.asset = ptrAsset;
                    newAssetAlias->pk.userRole = ptrRole;
                    newAssetAlias->pk.media = ptrMedia;
                    newAssetAlias->alias = sValue;
                    ptrAssetAlias = this->m_session.add<Echoes::Dbo::AlertMessageAliasAsset>(newAssetAlias);
                }
                res = 200;
            } 
            else 
            {
                res = 404;
                responseMsg = "{\n\t\"message\":\"Asset not found\"\n}";
            }
            transaction.commit();
        } 
        catch (Wt::Dbo::Exception const& e) 
        {
            Wt::log("error") << "[Asset Ressource] " << e.what();
            res = 503;
            responseMsg = "{\n\t\"message\": \"Service Unavailable\"\n}";
        }
    }
    
    return res;
}

void AssetResource::processPutRequest(Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";

    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->m_statusCode = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }
    else
    {   
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();

            if(!nextElement.compare(""))
            {
                this->m_statusCode = putAsset(responseMsg, m_requestData);
            }
            else if(!nextElement.compare("alias"))
            {
                this->m_statusCode = putAlias(responseMsg, m_requestData);
            }
            else
            {
                this->m_statusCode = 400;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            this->m_statusCode = 400;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(this->m_statusCode);
    response.out() << responseMsg;
    return;
}

void AssetResource::processDeleteRequest(Wt::Http::Response &response)
{
    return;
}


void AssetResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    this->m_role = "";
    this->m_media = "";
    if (!request.getParameterValues("role").empty())
    {
        this->m_role = request.getParameterValues("role")[0];
    }
    
    if (!request.getParameterValues("media").empty())
    {
        this->m_media = request.getParameterValues("media")[0];
    }
    PublicApiResource::handleRequest(request, response);
    return;
}

