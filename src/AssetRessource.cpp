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

void AssetRessource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);

    // set Content-Type
    response.setMimeType("application/json; charset=utf-8");

    if (!this->authentified) {
        response.setStatus(401);
        response.out() << "{\"message\":\"Authentification Failed\"}";
        return;
    }

    // URL path after /asset
    string path = request.pathInfo();

    vector< string > splitPath;
    boost::split(splitPath, path, boost::is_any_of("/"), boost::token_compress_on);

    // Convert 2nd path element to int
    try
    {
        this->assetId = boost::lexical_cast<int>(splitPath[1]);
    }
    catch(boost::bad_lexical_cast &)
    {
        response.setStatus(422);
        response.out() << "{\"message\":\"Validation Failed\"}";
        return;
    }

    if (this->assetId)
    {
        try
        {
            Wt::Dbo::Transaction transaction(*this->session);
            Wt::Dbo::ptr<Asset> asset = this->session->find<Asset > ().where("\"AST_ID\" = ?").bind(this->assetId);

            if (Utils::checkId<Asset> (asset))
            {
                this->probeId = asset.get()->probe.id();
            }
            else 
            {
                response.setStatus(404);
                response.out() << "{\"message\":\"Asset not found\"}";
                return;
            }
        } 
        catch (Wt::Dbo::Exception const& e) 
        {
            Wt::log("error") << e.what();
            response.setStatus(503);
            response.out() << "{\"message\":\"Service Unavailable\"}";
            return;
        }

        // GET or POST ?
        if (request.method() == "POST")
        {
            string json, pkgName, pkgType;
            Wt::WString arch, distribName, distribRelease;
                       
            json = request2string(request);

            try
            {
                Wt::Json::Object result;

                Wt::Json::parse(json, result);

                Wt::Json::Object distrib = result.get("distrib");
                arch = result.get("arch");
                distribName = distrib.get("name");
                distribRelease = distrib.get("release");
            }
            catch (Wt::Json::ParseError const& e)
            {
                response.setStatus(400);
                response.out() << "{\"message\":\"Problems parsing JSON\"}";
                Wt::log("warning") << "[Asset Ressource] Problems parsing JSON:" << json;
                return;
            }
            catch (Wt::Json::TypeException const& e)
            {
                response.setStatus(400);
                response.out() << "{\"message\":\"Problems parsing JSON\"}";
                Wt::log("warning") << "[Asset Ressource] Problems parsing JSON:" << json;
                return;
            }

            // persistance of asset data
            try 
            {
                Wt::Dbo::Transaction transaction(*this->session);
                Wt::Dbo::ptr<Asset> asset = this->session->find<Asset > ().where("\"AST_ID\" = ?").bind(this->assetId);
                if (Utils::checkId<Asset > (asset)) 
                {
                    asset.modify()->distribName = distribName;
                    asset.modify()->distribRelease = distribRelease;
                    asset.modify()->architecture = arch;
                } 
                else 
                {
                    response.setStatus(404);
                    response.out() << "{\"message\":\"Asset not found\"}";
                    return;
                }
                transaction.commit();
            } 
            catch (Wt::Dbo::Exception const& e) 
            {
                Wt::log("error") << e.what();
                response.setStatus(503);
                response.out() << "{\"message\":\"Service Unavailable\"}";
                return;
            }

            // distrib verification
            if(distribName == "Debian" || distribName == "Ubuntu")
            {
                int release;
                string distribReleaseTmp = distribRelease.toUTF8();
                vector< string > splitRelease;

                pkgType = "deb";

                boost::split(splitRelease, distribReleaseTmp, boost::is_any_of("."), boost::token_compress_on);

                // Convert 1st release element to int
                try
                {
                    release = boost::lexical_cast<int>(splitRelease[0]);
                }
                catch(boost::bad_lexical_cast &)
                {
                    response.setStatus(422);
                    response.out() << "{\"message\":\"Validation Failed\"}";
                    return;
                }


                if(distribName == "Debian" && release < 7)
                {

                    if(arch == "i386" || arch == "i486" || arch == "i586" || arch == "i686" || arch == "i786" || arch == "i886" || arch == "i986")
                    {
                        pkgName = "ea-probe_0.1.0.beta-1_squeeze_i386.deb";
                    }
                    else if(arch == "x86_64")
                    {
                        pkgName = "ea-probe_0.1.0.beta-1_squeeze_amd64.deb";
                    }
                    else
                    {
                        response.setStatus(501);
                        response.out() << "{\"message\":\"Architecture not supported\"}";
                        return;
                    }
                }
                else
                {
                    if(arch == "i386" || arch == "i486" || arch == "i586" || arch == "i686" || arch == "i786" || arch == "i886" || arch == "i986")
                    {
                        pkgName = "ea-probe_0.1.0.beta-1_i386.deb";
                    }
                    else if(arch == "x86_64")
                    {
                        pkgName = "ea-probe_0.1.0.beta-1_amd64.deb";
                    }
                    else
                    {
                        response.setStatus(501);
                        response.out() << "{\"message\":\"Architecture not supported\"}";
                        return;
                    }
                }
            }
            else if(distribName == "CentOS")
            {
                pkgType = "rpm";

                if(arch == "i386" || arch == "i486" || arch == "i586" || arch == "i686" || arch == "i786" || arch == "i886" || arch == "i986")
                {
                    pkgName = "ea-probe-0.1.0.beta-1.i386.rpm";
                }
                else if(arch == "x86_64")
                {
                    pkgName = "ea-probe-0.1.0.beta-1.x86_64.rpm";
                }
                else
                {
                    response.setStatus(501);
                    response.out() << "{\"message\":\"Architecture not supported\"}";
                    return;
                }
            }
            else
            {
                response.setStatus(501);
                response.out() << "{\"message\":\"Distrib not supported\"}";
                return;
            }


            response.out() << "{\n\
    \"id\" : " << this->probeId << ",\n\
    \"pakage_name\" : \"" << pkgName << "\",\n\
    \"pakage_type\" : \"" << pkgType << "\",\n\
    \"install_dir\" : \"/opt/echoes-alert/probe\"\n\
}";
            return;
        }
        else if (request.method() == "GET")
        {
            switch (splitPath.size())
            {
                case 3:
                    if (splitPath[2] == "plugin")
                    {
                        response.out() << "[\n\
    {\n\
        \"id\" : 1,\n\
        \"name\" : \"Linux-System.json\"\n\
    }\n\
]";
                    }
                    else
                    {
                        response.setStatus(422);
                        response.out() << "{\"message\":\"Validation Failed\"}";
                    }
                    return;
                    break;
                case 4:
                    if (splitPath[2] == "plugin")
                    {
                        // Convert 3rd path element to int
                        try
                        {
                            this->pluginId = boost::lexical_cast<int>(splitPath[3]);
                        }
                        catch(boost::bad_lexical_cast &)
                        {
                            response.setStatus(422);
                            response.out() << "{\"message\":\"Validation Failed\"}";
                            return;
                        }

                        if (this->pluginId == 1)
                        {
                            ifstream myfile ("/var/www/wt/probe/plugins/Linux-System.json");
                            if (myfile.is_open())
                            {
                                response.out() << myfile.rdbuf();
                                myfile.close();
                            }
                            else
                            {
                                response.setStatus(503);
                                response.out() << "{\"message\":\"Service Unavailable\"}";
                            }
                        }
                        else
                        {
                            response.setStatus(404);
                            response.out() << "{\"message\":\"Plugin not found.\"}";
                        }                                 
                    }
                    return;
                    break;
                default:
                    response.setStatus(422);
                    response.out() << "{\"message\":\"Validation Failed\"}";
                    return;
                    break;
            }
        }
        else
        {
            response.setStatus(405);
            response.out() << "{\"message\":\"Only GET and POST method are allowed.\"}";
            return;
        }
    }
    else
    {
        response.setStatus(422);
        response.out() << "{\"message\":\"Validation Failed\"}";
        return;
    }
}

AssetRessource::~AssetRessource() {
    beingDeleted();
}

