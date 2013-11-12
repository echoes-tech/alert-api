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

unsigned short ProbeResource::getProbesList(string &responseMsg)
{
    unsigned short res = 500;
    unsigned long idx = 0;
    
    try 
    {
        Wt::Dbo::Transaction transaction(m_session);
        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Asset>> listAssets = this->m_session.find<Echoes::Dbo::Asset> ()
                .where("\"AST_ORG_ORG_ID\" = ?").bind(this->m_session.user()->currentOrganization.id())
                .where("\"AST_DELETE\" IS NULL");

        responseMsg = "[\n";
        for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Asset>>::const_iterator i = listAssets.begin(); i != listAssets.end(); ++i)
        {
            Wt::Dbo::ptr<Echoes::Dbo::Probe> probe = this->m_session.find<Echoes::Dbo::Probe> ()
                .where("\"PRB_ID\" = ?").bind(i->get()->probe.id())
                .where("\"PRB_DELETE\" IS NULL");
            if (Utils::checkId<Echoes::Dbo::Probe>(probe))
            {
                responseMsg += "\t{\n";
                responseMsg += "\t\t\"id\": " + boost::lexical_cast<string, long long>(probe.id()) + ",\n";
                responseMsg += "\t\t\"name\": \"" + probe->name.toUTF8() + "\"\n";
                responseMsg += "\t}";
                if (idx < listAssets.size())
                {
                    responseMsg += ",";
                }
                responseMsg += "\n";
            }
            ++idx;
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

unsigned short ProbeResource::getProbe(string &responseMsg)
{
    unsigned short res = 500;
    
    try 
    {
        Wt::Dbo::Transaction transaction(m_session);
        Wt::Dbo::ptr<Echoes::Dbo::Probe> probe = this->m_session.find<Echoes::Dbo::Probe>()
                .where("\"PRB_ORG_ORG_ID\" = ?").bind(this->m_session.user()->currentOrganization.id())
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

            res = 200;
        } 
        else 
        {
            responseMsg = "{\n\t\"message\":\"Probe not found\"\n}";
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

