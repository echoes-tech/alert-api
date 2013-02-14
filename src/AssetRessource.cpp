/* 
 * API Asset Ressource
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

#include "AssetRessource.h"

using namespace std;

AssetRessource::AssetRessource() {
}

AssetRessource::AssetRessource(const AssetRessource& orig) {
}

AssetRessource::~AssetRessource() {
    beingDeleted();
}

string AssetRessource::getAssetsList()
{
    string res = "";
    unsigned long idx = 0;
    
    try 
    {
        Wt::Dbo::Transaction transaction(*this->session);
        Wt::Dbo::collection<Wt::Dbo::ptr<Asset>> listAssets = this->session->find<Asset> ()
                .where("\"AST_ORG_ORG_ID\" = ? AND \"AST_DELETE\" IS NULL")
                .bind(this->session->user()->currentOrganization.id());

        res = "[\n";
        for (Wt::Dbo::collection<Wt::Dbo::ptr<Asset>>::const_iterator i = listAssets.begin(); i != listAssets.end(); ++i)
        {
            res += "\t{\n";
            res += "\t\t\"id\": " + boost::lexical_cast<string, long long>(i->id()) + ",\n";
            res += "\t\t\"name\": \"" + i->get()->name.toUTF8() + "\"\n";
            res += "\t}";
            if (++idx < listAssets.size())
            {
                res += ",";
            }
            res += "\n";
        }
        res += "]";

        transaction.commit();
        this->statusCode = 200;
    } 
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\n\t\"message\":\"Service Unavailable\n\"}";
    }
    
    return res;
}

string AssetRessource::getAsset()
{
    string res = "";
    
    try 
    {
        Wt::Dbo::Transaction transaction(*this->session);
        Wt::Dbo::ptr<Asset> asset = this->session->find<Asset> ()
                .where("\"AST_ORG_ORG_ID\" = ? AND \"AST_ID\" = ? AND \"AST_DELETE\" IS NULL")
                .bind(this->session->user()->currentOrganization.id())
                .bind(this->vPathElements[1]);

        if (Utils::checkId<Asset>(asset)) 
        {
            res += "{\n";
            res += "\t\"id\": " + boost::lexical_cast<string, long long>(asset.id()) + ",\n";
            res += "\t\"name\": \"" + asset->name.toUTF8() + "\",\n";
            Wt::Dbo::ptr<AssetArchitecture> assetArchitecture = this->session->find<AssetArchitecture> ()
                .where("\"ASA_ID\" = ?")
                .bind(asset->assetArchitecture.id());
            if (Utils::checkId<AssetArchitecture>(assetArchitecture)) 
            {
                res += "\t\"architecture\": \"" + assetArchitecture->name.toUTF8() + "\",\n";
            }
            else 
            {
                res += "\t\"architecture\": \"Unknown\",\n";
            }
            res += "\t\"distribution\": {\n";
            Wt::Dbo::ptr<AssetDistribution> assetDistribution = this->session->find<AssetDistribution> ()
                .where("\"ASD_ID\" = ?")
                .bind(asset->assetDistribution.id());
            if (Utils::checkId<AssetDistribution>(assetDistribution)) 
            {
                res += "\t\t\"name\": \"" + assetDistribution->name.toUTF8() + "\",\n";
            }
            else 
            {
                res += "\t\t\"name\": \"Unknown\",\n";
            }
            Wt::Dbo::ptr<AssetRelease> assetRelease = this->session->find<AssetRelease> ()
                .where("\"ASR_ID\" = ?")
                .bind(asset->assetRelease.id());
            if (Utils::checkId<AssetRelease>(assetRelease)) 
            {
                res += "\t\t\"release\": \"" + assetRelease->name.toUTF8() + "\"\n";
            }
            else 
            {
                res += "\t\t\"release\": \"Unknown\"\n";
            }
            res += "\t}\n";
            res += "}";
            
            this->statusCode = 200;
        } 
        else 
        {
            res = "{\n\t\"message\":\"Asset not found\"\n}";
            this->statusCode = 404;
        }

        transaction.commit();
    } 
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\n\t\"message\":\"Service Unavailable\"\n}";
    }
    
    return res;
}

string AssetRessource::getPluginsListForAsset()
{
    string res = "";
    
    try 
    {
        Wt::Dbo::Transaction transaction(*this->session);
        Wt::Dbo::ptr<Asset> asset = this->session->find<Asset> ()
                .where("\"AST_ORG_ORG_ID\" = ? AND \"AST_ID\" = ? AND \"AST_DELETE\" IS NULL")
                .bind(this->session->user()->currentOrganization.id())
                .bind(this->vPathElements[1]);

        if (Utils::checkId<Asset>(asset)) 
        {
            res = "[\n";
            res += "\t{\n";
            res += "\t\t\"id\": 1,\n";
            res += "\t\t\"name\": \"Linux-System.json\"\n";
            res += "\t}\n";
            res += "]";
        } 
        else 
        {
            res = "{\n\t\"message\":\"Asset not found\"\n}";
            this->statusCode = 404;
        }

        transaction.commit();
    } 
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\n\t\"message\":\"Service Unavailable\"\n}";
    }
    
    return res;
}

string AssetRessource::getProbesListForAsset()
{
    string res = "";
    
    try 
    {
        Wt::Dbo::Transaction transaction(*this->session);
        Wt::Dbo::ptr<Asset> asset = this->session->find<Asset> ()
                .where("\"AST_ORG_ORG_ID\" = ? AND \"AST_ID\" = ? AND \"AST_DELETE\" IS NULL")
                .bind(this->session->user()->currentOrganization.id())
                .bind(this->vPathElements[1]);

        if (Utils::checkId<Asset>(asset)) 
        {
            Wt::Dbo::ptr<Probe> probe = this->session->find<Probe> ()
                .where("\"PRB_ID\" = ? AND \"PRB_DELETE\" IS NULL")
                .bind(asset->probe.id());
            if (Utils::checkId<Probe>(probe)) 
            {
                res += "[\n";
                res += "\t{\n";
                res += "\t\t\"id\": " + boost::lexical_cast<string, long long>(probe.id()) + ",\n";
                res += "\t\t\"name\": \"" + probe->name.toUTF8() + "\"\n";
                res += "\t}\n";
                res += "]";
                this->statusCode = 200;
            }
            else 
            {
                res = "{\n\t\"message\":\"Probe not found\"\n}";
                this->statusCode = 404;
            }
        } 
        else 
        {
            res = "{\n\t\"message\":\"Asset not found\"\n}";
            this->statusCode = 404;
        }

        transaction.commit();
    } 
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\n\t\"message\":\"Service Unavailable\"\n}";
    }
    
    return res;
}

void AssetRessource::processGetRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";

    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        responseMsg = getAssetsList();
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned int>(nextElement);

            nextElement = getNextElementFromPath();

            if(!nextElement.compare(""))
            {
                responseMsg = getAsset();
            }
            else if(!nextElement.compare("plugins"))
            {
                responseMsg = getPluginsListForAsset();
            }
            else if(!nextElement.compare("probes"))
            {
                responseMsg = getProbesListForAsset();
            }
            else
            {
                this->statusCode = 400;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            this->statusCode = 400;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(this->statusCode);
    response.out() << responseMsg;
    return;
}

string AssetRessource::postProbeForAsset(string sRequest)
{
    string res = "";
    Wt::WString name;
    
    try
    {
        Wt::Json::Object result;

        Wt::Json::parse(sRequest, result);

        name = result.get("name");
    }
    catch (Wt::Json::ParseError const& e)
    {
        Wt::log("warning") << "[Asset Ressource] Problems parsing JSON: " << sRequest;
        this->statusCode = 400;
        res = "{\n\t\"message\": \"Problems parsing JSON\"\n}";
    }
    catch (Wt::Json::TypeException const& e)
    {
        Wt::log("warning") << "[Asset Ressource] Problems parsing JSON: " << sRequest;
        this->statusCode = 400;
        res = "{\n\t\"message\": \"Problems parsing JSON\"\n}";
    }
    
    if(!res.compare(""))
    {
        try 
        {
            Wt::Dbo::Transaction transaction(*this->session);
            
            // Est-ce que l'asset existe ?
            Wt::Dbo::ptr<Asset> asset = this->session->find<Asset>()
                    .where("\"AST_ORG_ORG_ID\" = ? AND \"AST_ID\" = ? AND \"AST_DELETE\" IS NULL")
                    .bind(this->session->user()->currentOrganization.id())
                    .bind(this->vPathElements[1]);
            if (Utils::checkId<Asset>(asset))
            {
                res += "{\n";
                // Est-ce que la probe existe pour cet asset ?
                if (!Utils::checkId<Probe>(asset->probe))
                {
                    Probe *newProbe = new Probe();
                    newProbe->name = "Probe_" + this->session->user()->lastName + "_" + asset->name;
                    newProbe->organization = this->session->user()->currentOrganization;
                    Wt::Dbo::ptr<ProbePackageParameter> probePackageParameter = this->session->find<ProbePackageParameter>()
                            .where("\"PPP_ASA_ASA_ID\" = ? AND PPP_ASD_ASD_ID\" = ? AND PPP_ASR_ASR_ID\" = ? AND \"PPP_DELETE\" IS NULL")
                            .bind(asset->assetArchitecture.id())
                            .bind(asset->assetDistribution.id())
                            .bind(asset->assetRelease.id())
//                            .orderBy("\"PPP_PROBE_VERSION DESC\"", "\"PPP_PACKAGE_VERSION DESC\"")
                            .limit(1);
                    if (Utils::checkId<ProbePackageParameter>(probePackageParameter))
                    {
                        newProbe->probePackageParamater = probePackageParameter;
                    }
                    asset.modify()->probe = this->session->add<Probe>(newProbe);
                    asset->probe.flush();
                }
                res += "\t\"id\": " + boost::lexical_cast<string, long long>(asset->probe.id()) + ",\n";
                res += "\t\"name\": \"" + asset->probe->name.toUTF8() + "\",\n";

                // Est-ce que les param pkg de cette probe existent ?
                if (Utils::checkId<ProbePackageParameter>(asset->probe->probePackageParamater))
                {
                    res += "\t\"version\": \"" + asset->probe->probePackageParamater->probeVersion.toUTF8() + "\",\n";
                    res += "\t\"package\": {\n";
                    // Est-ce que le pkg de cette probe existent ?
                    if (Utils::checkId<ProbePackage>(asset->probe->probePackageParamater->probePackage))
                    {
                        res += "\t\t\"filename\": \"" + asset->probe->probePackageParamater->probePackage->filename.toUTF8() + "\",\n";
                    }
                    else
                    {
                        res += "\t\t\"filename\": \"Unknown\",\n";
                    }
                    res += "\t\t\"version\": \"" + asset->probe->probePackageParamater->packageVersion.toUTF8() + "\",\n";
                    res += "\t}\n";

                }
                else
                {
                    res += "\t\"version\": \"Unknown\",\n";
                    res += "\t\"package\": {\n";
                    res += "\t\t\"filename\": \"Unknown\",\n";
                    res += "\t\t\"version\": \"Unknown\",\n";
                    res += "\t}\n";
                }
            } 
            else 
            {
                this->statusCode = 404;
                res = "{\n\t\"message\":\"Asset not found\"\n}";
            }
            transaction.commit();
        } 
        catch (Wt::Dbo::Exception const& e) 
        {
            Wt::log("error") << "[Asset Ressource] " << e.what();
            this->statusCode = 503;
            res = "{\n\t\"message\": \"Service Unavailable\"\n}";
        }
    }
    
    return res;
}

void AssetRessource::processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "", sRequest = "";

    sRequest = request2string(request);
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->statusCode = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }
    else
    {   
        try
        {
            boost::lexical_cast<unsigned int>(nextElement);

            nextElement = getNextElementFromPath();

            if(!nextElement.compare("probes"))
            {
                responseMsg = postProbeForAsset(sRequest);
            }
            else
            {
                this->statusCode = 400;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            this->statusCode = 400;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(this->statusCode);
    response.out() << responseMsg;
    return;
}

void AssetRessource::processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}


string AssetRessource::patchAsset(string sRequest)
{
    string res = "";
    Wt::WString arch, distribName, distribRelease;
    
    try
    {
        Wt::Json::Object result;

        Wt::Json::parse(sRequest, result);

        Wt::Json::Object distrib = result.get("distrib");
        arch = result.get("arch");
        distribName = distrib.get("name");
        distribRelease = distrib.get("release");
    }
    catch (Wt::Json::ParseError const& e)
    {
        Wt::log("warning") << "[Asset Ressource] Problems parsing JSON: " << sRequest;
        this->statusCode = 400;
        res = "{\n\t\"message\": \"Problems parsing JSON\"\n}";
    }
    catch (Wt::Json::TypeException const& e)
    {
        Wt::log("warning") << "[Asset Ressource] Problems parsing JSON: " << sRequest;
        this->statusCode = 400;
        res = "{\n\t\"message\": \"Problems parsing JSON\"\n}";
    }

    if(!res.compare(""))
    {
        try 
        {
            Wt::Dbo::Transaction transaction(*this->session);
            Wt::Dbo::ptr<Asset> asset = this->session->find<Asset> ()
                    .where("\"AST_ORG_ORG_ID\" = ? AND \"AST_ID\" = ? AND \"AST_DELETE\" IS NULL")
                    .bind(this->session->user()->currentOrganization.id())
                    .bind(this->vPathElements[1]);
            if (Utils::checkId<Asset>(asset)) 
            {
                Wt::Dbo::ptr<AssetArchitecture> assetArchitecture = this->session->find<AssetArchitecture>()
                        .where("\"ASA_NAME\" = ?")
                        .bind(arch);
                if (!Utils::checkId<AssetArchitecture> (assetArchitecture)) 
                {
                    AssetArchitecture *newAssetArchitecture = new AssetArchitecture();
                    newAssetArchitecture->name = arch;
                    assetArchitecture = this->session->add<AssetArchitecture>(newAssetArchitecture);
                    assetArchitecture.flush();
                }
                asset.modify()->assetArchitecture = assetArchitecture;
                
                Wt::Dbo::ptr<AssetDistribution> assetDistribution = this->session->find<AssetDistribution>()
                        .where("\"ASD_NAME\" = ?")
                        .bind(distribName);
                if (!Utils::checkId<AssetDistribution> (assetDistribution)) 
                {
                    AssetDistribution *newAssetDistribution = new AssetDistribution();
                    newAssetDistribution->name = distribName;
                    assetDistribution = this->session->add<AssetDistribution>(newAssetDistribution);
                    assetDistribution.flush();
                }
                asset.modify()->assetDistribution = assetDistribution;
                
                Wt::Dbo::ptr<AssetRelease> assetRelease = this->session->find<AssetRelease>()
                        .where("\"ASR_NAME\" = ?")
                        .bind(distribRelease);
                if (!Utils::checkId<AssetRelease> (assetRelease)) 
                {
                    AssetRelease *newAssetRelease = new AssetRelease();
                    newAssetRelease->name = distribRelease;
                    assetRelease = this->session->add<AssetRelease>(newAssetRelease);
                    assetRelease.flush();
                }
                asset.modify()->assetRelease = assetRelease;
            } 
            else 
            {
                this->statusCode = 404;
                res = "{\n\t\"message\":\"Asset not found\"\n}";
            }
            transaction.commit();
        } 
        catch (Wt::Dbo::Exception const& e) 
        {
            Wt::log("error") << "[Asset Ressource] " << e.what();
            this->statusCode = 503;
            res = "{\n\t\"message\": \"Service Unavailable\"\n}";
        }
    }
    
    return res;
}

void AssetRessource::processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "", sRequest = "";

    sRequest = request2string(request);
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->statusCode = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }
    else
    {   
        try
        {
            boost::lexical_cast<unsigned int>(nextElement);

            nextElement = getNextElementFromPath();

            if(!nextElement.compare(""))
            {
                responseMsg = patchAsset(sRequest);
            }
            else
            {
                this->statusCode = 400;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            this->statusCode = 400;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(this->statusCode);
    response.out() << responseMsg;
    return;
}

void AssetRessource::processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}


void AssetRessource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);

    return;
}

