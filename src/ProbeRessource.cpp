/* 
 * API Probe Ressource
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

#include "ProbeRessource.h"

using namespace std;

ProbeRessource::ProbeRessource() {
}

ProbeRessource::ProbeRessource(const ProbeRessource& orig) {
}

ProbeRessource::~ProbeRessource() {
    beingDeleted();
}

void ProbeRessource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);
    
//    this->probeId = PublicApiResource::getIdFromRequest(request);
//
//    if (this->probeId > 0)
//    {
//        // Check probe existence and authorization
//        try
//        {
//            Wt::Dbo::Transaction transaction(*this->session);
//            Wt::Dbo::ptr<Probe> probe = this->session->find<Probe > ().where("\"PRB_ID\" = ?").bind(this->probeId);
//
//            if (Utils::checkId<Probe> (probe))
//            {
//                if(this->session->user().get()->currentOrganization.id() != probe.get()->organization.id())
//                {
//                    response.setStatus(403);
//                    response.out() << "{\"message\":\"Not allowed to manage this probe\"}";
//                    return;
//                }
//            }
//            else 
//            {
//                response.setStatus(404);
//                response.out() << "{\"message\":\"Probe not found\"}";
//                return;
//            }
//        }
//        catch (Wt::Dbo::Exception const& e) 
//        {
//            Wt::log("error") << e.what();
//            response.setStatus(503);
//            response.out() << "{\"message\":\"Service Unavailable\"}";
//            return;
//        }
//
//        // Check asset existence and retrieve info
//        try
//        {
//            Wt::Dbo::Transaction transaction(*this->session);
//            Wt::Dbo::ptr<Asset> asset = this->session->find<Asset> ().where("\"AST_PRB_PRB_ID\" = ?").bind(this->probeId).where("\"AST_IS_HOST\" = ?").bind(true);
//
//            if (Utils::checkId<Asset> (asset))
//            {
//                if (asset.get()->distribName)
//                {
//                    this->distribName = asset.get()->distribName.get();
//                }
//                else
//                {
//                    this->distribName = "";
//                }
//                if (asset.get()->distribRelease)
//                {
//                    this->distribRelease = asset.get()->distribRelease.get();
//                }
//                else
//                {
//                    this->distribRelease = "";
//                }
//                if (asset.get()->architecture)
//                {
//                    this->arch = asset.get()->architecture.get();
//                }
//                else
//                {
//                    this->arch = "";
//                }
//            }
//            else 
//            {
//                response.setStatus(404);
//                response.out() << "{\"message\":\"No asset found for this probe\"}";
//                return;
//            }
//        }
//        catch (Wt::Dbo::Exception const& e) 
//        {
//            Wt::log("error") << e.what();
//            response.setStatus(503);
//            response.out() << "{\"message\":\"Service Unavailable\"}";
//            return;
//        }
//
//        // GET ?
//        if (request.method() == "GET")
//        {
//            ifstream probeFile;
//            const char* probeFilePath;
//
//            int release = PublicApiResource::releaseSplit(response, distribRelease.toUTF8());
//            if (release < 0)
//                return;
//            
//            // distrib verification
//            if(distribName == "Debian" || distribName == "Ubuntu")
//            {
//                if(distribName == "Debian" && release < 7)
//                {
//
//                    if(arch == "i386" || arch == "i486" || arch == "i586" || arch == "i686" || arch == "i786" || arch == "i886" || arch == "i986")
//                    {
//                        probeFilePath = "/var/www/wt/probe/ea-probe_0.1.0.beta-2_Debian_Squeeze_i386.deb";
//                    }
//                    else if(arch == "x86_64")
//                    {
//                        probeFilePath = "/var/www/wt/probe/ea-probe_0.1.0.beta-2_Debian_Squeeze_amd64.deb";
//                    }
//                    else
//                    {
//                        response.setStatus(501);
//                        response.out() << "{\"message\":\"Architecture not supported\"}";
//                        return;
//                    }
//                }
//                else
//                {
//                    if(arch == "i386" || arch == "i486" || arch == "i586" || arch == "i686" || arch == "i786" || arch == "i886" || arch == "i986")
//                    {
//                        probeFilePath = "/var/www/wt/probe/ea-probe_0.1.0.beta-2_Ubuntu_12.04_i386.deb";
//                    }
//                    else if(arch == "x86_64")
//                    {
//                        probeFilePath = "/var/www/wt/probe/ea-probe_0.1.0.beta-2_Ubuntu_12.04_amd64.deb";
//                    }
//                    else
//                    {
//                        response.setStatus(501);
//                        response.out() << "{\"message\":\"Architecture not supported\"}";
//                        return;
//                    }
//                }
//            }
//            else if(distribName == "CentOS")
//            {
//                if(release < 6)
//                {
//                    if(arch == "i386" || arch == "i486" || arch == "i586" || arch == "i686" || arch == "i786" || arch == "i886" || arch == "i986")
//                    {
//                        probeFilePath = "/var/www/wt/probe/ea-probe-0.1.0.beta-2.CentOS-5.i386.rpm";
//                    }
//                    else if(arch == "x86_64")
//                    {
//                        probeFilePath = "/var/www/wt/probe/ea-probe-0.1.0.beta-2.CentOS-5.x86_64.rpm";
//                    }
//                    else
//                    {
//                        response.setStatus(501);
//                        response.out() << "{\"message\":\"Architecture not supported\"}";
//                        return;
//                    }
//                }
//                else
//                {
//                    if(arch == "i386" || arch == "i486" || arch == "i586" || arch == "i686" || arch == "i786" || arch == "i886" || arch == "i986")
//                    {
//                        probeFilePath = "/var/www/wt/probe/ea-probe-0.1.0.beta-2.CentOS-6.i386.rpm";
//                    }
//                    else if(arch == "x86_64")
//                    {
//                        probeFilePath = "/var/www/wt/probe/ea-probe-0.1.0.beta-2.CentOS-6.x86_64.rpm";
//                    }
//                    else
//                    {
//                        response.setStatus(501);
//                        response.out() << "{\"message\":\"Architecture not supported\"}";
//                        return;
//                    }
//                }
//            }
//            else
//            {
//                response.setStatus(501);
//                response.out() << "{\"message\":\"Distrib not supported\"}";
//                return;
//            }
//
//            probeFile.open(probeFilePath);
//            if (probeFile.is_open())
//            {
//                response.setMimeType("application/octet-stream");
//                response.setStatus(202);
//                response.out() << probeFile.rdbuf();
//                probeFile.close();
//            }
//            else
//            {
//                response.setStatus(503);
//                response.out() << "{\"message\":\"Service Unavailable\"}";
//            }
//
//        }
//        else
//        {
//            response.setStatus(405);
//            response.out() << "{\"message\":\"Only GET method are allowed.\"}";
//            return;
//        }
//
//    }
//    else
//    {
//        response.setStatus(422);
//        response.out() << "{\"message\":\"Validation Failed\"}";
//        return;
//    }
}

void ProbeRessource::processGetRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}

void ProbeRessource::processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}

void ProbeRessource::processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}

void ProbeRessource::processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}

void ProbeRessource::processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}

