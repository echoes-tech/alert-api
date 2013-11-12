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

OrganizationResource::~OrganizationResource() 
{
}

unsigned short OrganizationResource::getOrganization(std::string &responseMsg)
{
     unsigned short res = 500;
     Wt::Dbo::Transaction transaction(m_session);
     Wt::Dbo::ptr<Echoes::Dbo::Organization> organization = m_session.find<Echoes::Dbo::Organization>().where("\"ORG_ID\" = ?").bind(this->m_session.user().get()->currentOrganization.id());
    try
    {
        if(organization)
        {
            organization.modify()->setId(organization.id());
            responseMsg = organization->toJSON();
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

unsigned short OrganizationResource::getUsersForOrganization(std::string &responseMsg)
{
    unsigned short res = 500;
    unsigned idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        std::string queryStr =  "SELECT usr FROM \"T_USER_USR\" usr where \"USR_ID\" IN"
            "("
                "SELECT \"T_USER_USR_USR_ID\" FROM \"TJ_USR_ORG\" where \"T_ORGANIZATION_ORG_ORG_ID\" IN "
                "("
                    "SELECT \"T_ORGANIZATION_ORG_ORG_ID\" from \"TJ_USR_ORG\" where \"T_USER_USR_USR_ID\" = " + boost::lexical_cast<std::string > (this->m_session.user().id()) +
                ")"
            ")"
            "AND \"USR_DELETE\" IS NULL";

        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::User> > queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::User> >(queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::User> > user = queryRes.resultList();

        if (user.size() > 0)
        {
            responseMsg = "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::User> >::const_iterator i = user.begin(); i != user.end(); i++) 
            {
                i->modify()->setId(i->id());
                responseMsg += "\t" + i->get()->toJSON();
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

unsigned short OrganizationResource::getRolesForOrganization(std::string &responseMsg)
{
    unsigned short res = 500;
    unsigned idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::UserRole> > userRoles = m_session.find<Echoes::Dbo::UserRole>()
                .where("\"URO_ORG_ORG_ID\" = ?").bind(this->m_session.user().get()->currentOrganization.id())
                .where("\"URO_DELETE\" IS NULL");

        std::cout << this->m_session.user().get()->currentOrganization.id() << std::endl;
        if (userRoles.size() > 0)
        {
            responseMsg = "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::UserRole> >::const_iterator i = userRoles.begin(); i != userRoles.end(); i++) 
            {
                i->modify()->setId(i->id());
                responseMsg += "\t" + i->get()->toJSON();
                ++idx;
                if(userRoles.size()-idx > 0)
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
            responseMsg = "{\"message\":\"Roles not found\"}";
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

 unsigned short OrganizationResource::getMediasForUserForOrganization(std::string &responseMsg)
 {
 unsigned short res = 500;
    unsigned idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);
        string queryStr = "SELECT med FROM \"T_MEDIA_MED\" med where \"MED_ID\" IN"
                " ("
                " SELECT \"MEV_MED_MED_ID\" FROM \"T_MEDIA_VALUE_MEV\" "
                " WHERE \"MEV_USR_USR_ID\" = " + boost::lexical_cast<std::string>(m_pathElements[2]) +
                " AND \"MEV_DELETE\" IS NULL"
                " )"
                " AND \"MED_DELETE\" IS NULL";
 
        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Media> > queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::Media> >(queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Media> > media = queryRes.resultList();
        
        if (media.size() > 0)
        {
            responseMsg += "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Media> >::const_iterator i = media.begin(); i != media.end(); i++) 
            {
                i->modify()->setId(i->id());
                responseMsg += "\t" + i->modify()->toJSON();
                ++idx;
                if(media.size()-idx > 0)
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
            responseMsg = "{\"message\":\"Media not found\"}";
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
 
unsigned short OrganizationResource::getMediasValuesForUserForOrganization(std::string &responseMsg)
{
    unsigned short res = 500;
    int idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);
        Wt::Dbo::ptr<Echoes::Dbo::User> user = m_session.find<Echoes::Dbo::User>().where("\"USR_ID\" = ?").bind(m_pathElements[2])
                                                         .where("\"CURRENT_ORG_ID\" = ?").bind(m_session.user().get()->currentOrganization.id())
                                                        .where("\"USR_DELETE\" is NULL");
        if(user)
        {
            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::MediaValue>> mediaValues = m_session.find<Echoes::Dbo::MediaValue>().where("\"MEV_USR_USR_ID\" = ?").bind(user.id())
                                                                                                   .where("\"MEV_MED_MED_ID\" = ?").bind(m_pathElements[4])
                                                                                                   .where("\"MEV_DELETE\" is NULL");
            responseMsg = "[\n";
            if(mediaValues.size() > 0)
            {
                for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::MediaValue> >::const_iterator i = mediaValues.begin(); i != mediaValues.end(); i++) 
                {
                    i->modify()->setId(i->id());
                    responseMsg += "\t" + i->modify()->toJSON();
                    ++idx;
                    if(mediaValues.size()-idx > 0)
                    {
                        responseMsg += ",\n";
                    }
                }
                responseMsg += "\n]\n";
                res = 200;
            }
            else
            {
                responseMsg = "{\"message\":\"Media value not found\"}";
                 res = 404;
            }
        }
        else 
        {
             responseMsg = "{\"message\":\"User not found\"}";
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

unsigned short OrganizationResource::getQuotasAsset(std::string &responseMsg)
{
    unsigned short res = 500;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);
        
        Wt::Dbo::ptr<Echoes::Dbo::PackOption> ptrPackOption = m_session.find<Echoes::Dbo::PackOption>()
                .where("\"POP_PCK_PCK_ID\" = ?").bind(this->m_session.user()->currentOrganization->pack.id())
                .where("\"POP_OPT_OPT_ID\" = 1")
                .limit(1);
        if (ptrPackOption.get())
        {
            Wt::Dbo::ptr<Echoes::Dbo::OptionValue> ptrOptionValue = m_session.find<Echoes::Dbo::OptionValue>()
                    .where("\"OPT_ID_OPT_ID\" = ?").bind(ptrPackOption->pk.option.id())
                    .where("\"ORG_ID_ORG_ID\" = ?").bind(this->m_session.user()->currentOrganization.id())
                    .limit(1);
            if (ptrOptionValue.get())
            {
                responseMsg += ptrOptionValue->toJSON();
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

unsigned short OrganizationResource::getQuotasSms(std::string &responseMsg)
{
    unsigned short res = 500;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::ptr<Echoes::Dbo::PackOption> ptrPackOption = m_session.find<Echoes::Dbo::PackOption>()
                .where("\"POP_PCK_PCK_ID\" = ?").bind(this->m_session.user()->currentOrganization->pack.id())
                .where("\"POP_OPT_OPT_ID\" = 2")
                .limit(1);
        if (ptrPackOption.get())
        {
            Wt::Dbo::ptr<Echoes::Dbo::OptionValue> ptrOptionValue = m_session.find<Echoes::Dbo::OptionValue>()
                    .where("\"OPT_ID_OPT_ID\" = ?").bind(ptrPackOption.get()->pk.option.id())
                    .where("\"ORG_ID_ORG_ID\" = ?").bind(this->m_session.user()->currentOrganization.id())
                    .limit(1);
            if (ptrOptionValue.get())
            {
                responseMsg += ptrOptionValue->toJSON();
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
        this->m_statusCode = getOrganization(responseMsg);
    }
    else
    {
        if(!nextElement.compare("quotas_sms"))
        {
            this->m_statusCode = getQuotasSms(responseMsg);
        }
        else if(!nextElement.compare("quotas_assets"))
        {
            this->m_statusCode = getQuotasAsset(responseMsg);
        }
        else if(!nextElement.compare("users"))
        {
            nextElement = getNextElementFromPath();
            if(!nextElement.compare(""))
            {
            this->m_statusCode = getUsersForOrganization(responseMsg);
            }
            else
            {
                try
                {
                    boost::lexical_cast<unsigned long long>(nextElement);

                    nextElement = getNextElementFromPath();

                    if(!nextElement.compare("medias"))
                    {
                        nextElement = getNextElementFromPath();
                        if(!nextElement.compare(""))
                        {
                            this->m_statusCode =  getMediasForUserForOrganization(responseMsg);
                        }
                        else
                        {
                            boost::lexical_cast<unsigned long long>(nextElement);
                            nextElement = getNextElementFromPath();
                            if(!nextElement.compare(""))
                            {
                                this->m_statusCode = getMediasValuesForUserForOrganization(responseMsg);
                            }
                            else
                            {
                                this->m_statusCode = 400;
                                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                            }
                        }
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
        }
        else if(!nextElement.compare("roles"))
        {
            this->m_statusCode = getRolesForOrganization(responseMsg);
        }
        else
        {
            this->m_statusCode = 400;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(this->m_statusCode);
    response.out() << responseMsg;
    return;
}


void OrganizationResource::processPostRequest(Wt::Http::Response &response)
{

    return ;
}


void OrganizationResource::processPutRequest(Wt::Http::Response &response)
{
    return;
}


void OrganizationResource::processPatchRequest(Wt::Http::Response &response)
{
    return;
}



void OrganizationResource::processDeleteRequest(Wt::Http::Response &response)
{
    return;
}


void OrganizationResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);

    return;
}

