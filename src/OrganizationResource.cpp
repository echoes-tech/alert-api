/* 
 * API OrganizationResource
 * @author ECHOES Technologies (GDR)
 * @date 18/02/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "OrganizationResource.h"

using namespace std;

OrganizationResource::OrganizationResource() : PublicApiResource::PublicApiResource()
{
}

OrganizationResource::OrganizationResource(const OrganizationResource& orig) : PublicApiResource::PublicApiResource(orig)
{
}

OrganizationResource::~OrganizationResource() 
{
}

unsigned short OrganizationResource::getOrganization(std::string &responseMsg) const
{
     unsigned short res = 500;
     Wt::Dbo::Transaction transaction(*this->session);
     Wt::Dbo::ptr<Organization> organization = session->find<Organization>().where("\"ORG_ID\" = ?").bind(this->session->user().get()->currentOrganization.id());
    try
    {
        if(organization)
        {
            organization.modify()->setId(organization.id());
            responseMsg = organization.modify()->toJSON();
            res = 200;
        }
        else 
        {
            res = 404;
            responseMsg = "{\"message\":\"Organization not found\"}";
        }

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;
}

unsigned short OrganizationResource::getUsersForOrganization(std::string &responseMsg) const
{
    unsigned short res = 500;
    unsigned idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(*this->session);

        std::string queryStr =  "SELECT usr FROM \"T_USER_USR\" usr where \"USR_ID\" IN"
            "("
                "SELECT \"T_USER_USR_USR_ID\" FROM \"TJ_USR_ORG\" where \"T_ORGANIZATION_ORG_ORG_ID\" IN "
                "("
                    "SELECT \"T_ORGANIZATION_ORG_ORG_ID\" from \"TJ_USR_ORG\" where \"T_USER_USR_USR_ID\" = " + boost::lexical_cast<std::string > (this->session->user().id()) +
                ")"
            ")"
            "AND \"USR_DELETE\" IS NULL";

        Wt::Dbo::Query<Wt::Dbo::ptr<User> > queryRes = session->query<Wt::Dbo::ptr<User> >(queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<User> > user = queryRes.resultList();

        if (user.size() > 0)
        {
            responseMsg = "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<User> >::const_iterator i = user.begin(); i != user.end(); i++) 
            {
                i->modify()->setId(i->id());
                responseMsg += "\t" + i->modify()->toJSON();
                ++idx;
                if(user.size()-idx > 0)
                {
                    responseMsg += ",\n";
                }
            }
            responseMsg += "\n]\n";
            res = 200;
        }
        else 
        {
            res = 404;
            responseMsg = "{\"message\":\"User not found\"}";
        }

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;
}

unsigned short OrganizationResource::getQuotasAsset(std::string &responseMsg) const
{
    unsigned short res = 500;
    try
    {
        Wt::Dbo::Transaction transaction(*this->session);
        
        Wt::Dbo::ptr<PackOption> ptrPackOption = session->find<PackOption>()
                .where("\"POP_PCK_PCK_ID\" = ?").bind(this->session->user()->currentOrganization->pack.id())
                .where("\"POP_OPT_OPT_ID\" = 1")
                .limit(1);
        if (ptrPackOption.get())
        {
            Wt::Dbo::ptr<OptionValue> ptrOptionValue = session->find<OptionValue>()
                    .where("\"OPT_ID_OPT_ID\" = ?").bind(ptrPackOption->pk.option.id())
                    .where("\"ORG_ID_ORG_ID\" = ?").bind(this->session->user()->currentOrganization.id())
                    .limit(1);
            if (ptrOptionValue.get())
            {
                responseMsg += ptrOptionValue.modify()->toJSON();
                res = 200;
            }
            else
            {
                responseMsg = "{\"message\":\"Option not found\"}";
                res = 404;
            }
        }
        else
        {
            responseMsg = "{\"message\":\"Option not found\"}";
            res = 404;
        }

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    } 
    return res;
}

unsigned short OrganizationResource::getQuotasSms(std::string &responseMsg) const
{
    unsigned short res = 500;
    try
    {
        Wt::Dbo::Transaction transaction(*this->session);

        Wt::Dbo::ptr<PackOption> ptrPackOption = session->find<PackOption>()
                .where("\"POP_PCK_PCK_ID\" = ?").bind(this->session->user()->currentOrganization->pack.id())
                .where("\"POP_OPT_OPT_ID\" = 2")
                .limit(1);
        if (ptrPackOption.get())
        {
            Wt::Dbo::ptr<OptionValue> ptrOptionValue = session->find<OptionValue>()
                    .where("\"OPT_ID_OPT_ID\" = ?").bind(ptrPackOption.get()->pk.option.id())
                    .where("\"ORG_ID_ORG_ID\" = ?").bind(this->session->user()->currentOrganization.id())
                    .limit(1);
            if (ptrOptionValue.get())
            {
                responseMsg += ptrOptionValue.modify()->toJSON();
                res = 200;
            }
        }
               
        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    } 
    return res;
}

void OrganizationResource::processGetRequest(Wt::Http::Response &response)
{
    std::string responseMsg = "", nextElement = "";
    
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->statusCode = getOrganization(responseMsg);
    }
    else
    {
        if(!nextElement.compare("quotas_sms"))
        {
            this->statusCode = getQuotasSms(responseMsg);
        }
        else if(!nextElement.compare("quotas_assets"))
        {
            this->statusCode = getQuotasAsset(responseMsg);
        }
        else if(!nextElement.compare("users"))
        {
            this->statusCode = getUsersForOrganization(responseMsg);
        }
        else
        {
            this->statusCode = 400;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(this->statusCode);
    response.out() << responseMsg;
    return;
}


void OrganizationResource::processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{

    return ;
}


void OrganizationResource::processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}


void OrganizationResource::processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}



void OrganizationResource::processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}


void OrganizationResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);

    return;
}

