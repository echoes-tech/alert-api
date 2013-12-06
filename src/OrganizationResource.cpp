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
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                responseMsg = httpCodeToJSON(m_statusCode, "");
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

void OrganizationResource::processDeleteRequest(Wt::Http::Response &response)
{
    return;
}

