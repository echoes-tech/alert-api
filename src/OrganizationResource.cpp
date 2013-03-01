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

OrganizationResource::OrganizationResource() {
}

std::string OrganizationResource::getOrganization()
{
    std::string res = "";
     Wt::Dbo::Transaction transaction(*this->session);
     Wt::Dbo::ptr<Organization> organization = session->find<Organization>().where("\"ORG_ID\" = ?").bind(this->session->user().get()->currentOrganization.id());
    try
    {
        if(organization)
        {
            res = organization.modify()->toJSON();
            this->statusCode = 200;
        }
        else 
        {
            this->statusCode = 404;
            res = "{\"message\":\"Organization not found\"}";
            return res;
        }

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}

std::string OrganizationResource::getUsersForOrganization()
{
    std::string res = "";
    int idx = 0;
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
            if (user.size() > 1)
            {
            res += "[\n";
            }
            for (Wt::Dbo::collection<Wt::Dbo::ptr<User> >::const_iterator i = user.begin(); i != user.end(); i++) 
            {
                i->modify()->setId(i->id());
                res += "\t" + i->modify()->toJSON();
                +idx;
                if(user.size()-idx > 0)
                {
                    res.replace(res.size()-1, 1, "");
                    res += ",\n";
                }
            }
            if (user.size() > 1)
            {
            res += "]\n";
            }
            this->statusCode = 200;
        }
        else 
        {
            this->statusCode = 404;
            res = "{\"message\":\"User not found\"}";
            return res;
        }

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}

std::string OrganizationResource::getQuotasAsset()
{
      std::string res = "";
    try
    {
        Wt::Dbo::Transaction transaction(*this->session);
        Wt::Dbo::ptr<User> user = session->find<User>().where("\"USR_ID\" = ?").bind(this->session->user().id());

        if (user)
        {
            Wt::Dbo::ptr<Organization> tempOrga = user->currentOrganization;
            Wt::Dbo::ptr<PackOption> ptrPackOption = session->find<PackOption>()
                    .where("\"POP_PCK_PCK_ID\" = ?").bind(tempOrga.get()->pack.id())
                    .where("\"POP_OPT_OPT_ID\" = 1")
                    .limit(1);
            if (ptrPackOption.get())
            {
                Wt::Dbo::ptr<OptionValue> ptrOptionValue = session->find<OptionValue>().where("\"OPT_ID_OPT_ID\" = ?").bind(ptrPackOption.get()->pk.option.id())
                                                                .where("\"ORG_ID_ORG_ID\" = ?").bind(tempOrga.id())
                                                                .limit(1);
                if (ptrOptionValue.get())
                {
                    res += ptrOptionValue.modify()->toJSON();
                    this->statusCode = 200;
                }
            }
        }
        else
        {
            this->statusCode = 404;
            res = "{\"message\":\"user not found\"}";
            return res;
        }
               
        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\"message\":\"Service Unavailable\"}";
        return res;
    } 
    return res;
}

std::string OrganizationResource::getQuotasSms()
{
    std::string res = "";
    try
    {
        Wt::Dbo::Transaction transaction(*this->session);
        Wt::Dbo::ptr<User> user = session->find<User>().where("\"USR_ID\" = ?").bind(this->session->user().id());

        if (user)
        {
            Wt::Dbo::ptr<Organization> tempOrga = user->currentOrganization;
            Wt::Dbo::ptr<PackOption> ptrPackOption = session->find<PackOption>()
                    .where("\"POP_PCK_PCK_ID\" = ?").bind(tempOrga.get()->pack.id())
                    .where("\"POP_OPT_OPT_ID\" = 2")
                    .limit(1);
            if (ptrPackOption.get())
            {
                Wt::Dbo::ptr<OptionValue> ptrOptionValue = session->find<OptionValue>().where("\"OPT_ID_OPT_ID\" = ?").bind(ptrPackOption.get()->pk.option.id())
                                                                .where("\"ORG_ID_ORG_ID\" = ?").bind(tempOrga.id())
                                                                .limit(1);
                if (ptrOptionValue.get())
                {
                    res += ptrOptionValue.modify()->toJSON();
                    this->statusCode = 200;
                }
            }
        }
        else
        {
            this->statusCode = 404;
            res = "{\"message\":\"user not found\"}";
            return res;
        }
               
        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\"message\":\"Service Unavailable\"}";
        return res;
    } 
    return res;
}

void OrganizationResource::processGetRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    std::string responseMsg = "", nextElement = "";
    
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        responseMsg = getOrganization();
    }
    else
    {
        if(!nextElement.compare("quotas_sms"))
        {
            responseMsg = getQuotasSms();
        }
        else if(!nextElement.compare("quotas_assets"))
        {
            responseMsg = getQuotasAsset();
        }
        else if(!nextElement.compare("users"))
        {
            responseMsg = getUsersForOrganization();
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

OrganizationResource::~OrganizationResource() 
{
        beingDeleted();
}