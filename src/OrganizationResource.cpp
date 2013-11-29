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

EReturnCode OrganizationResource::getOrganizationsList(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Organization>> orgPtrCol = m_session.find<Echoes::Dbo::Organization>()
                .where(QUOTE(TRIGRAM_ORGANIZATION SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_ORGANIZATION ID) " = ?").bind(m_session.user()->organization.id())
                .orderBy(QUOTE(TRIGRAM_ORGANIZATION ID));

        res = serialize(orgPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode OrganizationResource::getOrganization(std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::ptr<Echoes::Dbo::Organization> orgPtr = m_session.find<Echoes::Dbo::Organization>()
                .where(QUOTE(TRIGRAM_ORGANIZATION SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_ORGANIZATION ID) " = ?").bind(m_pathElements[1])
                .where(QUOTE(TRIGRAM_ORGANIZATION ID) " = ?").bind(m_session.user()->organization.id());

        res = serialize(orgPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode OrganizationResource::getQuotasAsset(std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);
        
        Wt::Dbo::ptr<Echoes::Dbo::PackOption> ptrPackOption = m_session.find<Echoes::Dbo::PackOption>()
                .where("\"POP_PCK_PCK_ID\" = ?").bind(this->m_session.user()->organization->pack.id())
                .where("\"POP_OPT_OPT_ID\" = 1")
                .limit(1);
        if (ptrPackOption.get())
        {
            Wt::Dbo::ptr<Echoes::Dbo::OptionValue> ptrOptionValue = m_session.find<Echoes::Dbo::OptionValue>()
                    .where("\"OPT_ID_OPT_ID\" = ?").bind(ptrPackOption->pk.option.id())
                    .where("\"ORG_ID_ORG_ID\" = ?").bind(this->m_session.user()->organization.id())
                    .limit(1);
            if (ptrOptionValue.get())
            {
                responseMsg += ptrOptionValue->toJSON();
                res = EReturnCode::OK;
            }
            else
            {
                responseMsg = "{\"message\":\"Option not found\"}";
                res = EReturnCode::NOT_FOUND;
            }
        }
        else
        {
            responseMsg = "{\"message\":\"Option not found\"}";
            res = EReturnCode::NOT_FOUND;
        }

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    } 
    return res;
}

EReturnCode OrganizationResource::getQuotasSms(std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::ptr<Echoes::Dbo::PackOption> ptrPackOption = m_session.find<Echoes::Dbo::PackOption>()
                .where("\"POP_PCK_PCK_ID\" = ?").bind(this->m_session.user()->organization->pack.id())
                .where("\"POP_OPT_OPT_ID\" = 2")
                .limit(1);
        if (ptrPackOption.get())
        {
            Wt::Dbo::ptr<Echoes::Dbo::OptionValue> ptrOptionValue = m_session.find<Echoes::Dbo::OptionValue>()
                    .where("\"OPT_ID_OPT_ID\" = ?").bind(ptrPackOption.get()->pk.option.id())
                    .where("\"ORG_ID_ORG_ID\" = ?").bind(this->m_session.user()->organization.id())
                    .limit(1);
            if (ptrOptionValue.get())
            {
                responseMsg += ptrOptionValue->toJSON();
                res = EReturnCode::OK;
            }
        }
               
        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    } 
    return res;
}

void OrganizationResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if(nextElement.empty())
    {
        m_statusCode = getOrganizationsList(responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();
            if(nextElement.empty())
            {
                m_statusCode = getOrganization(responseMsg);
            }
            else if(!nextElement.compare("quotas_sms"))
            {
                this->m_statusCode = getQuotasSms(responseMsg);
            }
            else if(!nextElement.compare("quotas_assets"))
            {
                this->m_statusCode = getQuotasAsset(responseMsg);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast const& e)
        {
            m_statusCode = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(m_statusCode, e);
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

