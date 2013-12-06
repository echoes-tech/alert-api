/* 
 * API InformationResource
 * @author ECHOES Technologies (GDR)
 * @date 11/02/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "InformationResource.h"

using namespace std;

InformationResource::InformationResource() : PublicApiResource::PublicApiResource()
{
}

InformationResource::~InformationResource()
{
}

EReturnCode InformationResource::getInformationsList(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Information>> infPtrCol = m_session.find<Echoes::Dbo::Information>()
                .where(QUOTE(TRIGRAM_INFORMATION SEP "DELETE") " IS NULL")
                .orderBy(QUOTE(TRIGRAM_INFORMATION ID));

        res = serialize(infPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode InformationResource::getInformation(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::ptr<Echoes::Dbo::Information> acrPtr = m_session.find<Echoes::Dbo::Information>()
                .where(QUOTE(TRIGRAM_INFORMATION ID) " = ?").bind(m_pathElements[1])
                .where(QUOTE(TRIGRAM_INFORMATION SEP "DELETE") " IS NULL");

        res = serialize(acrPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

void InformationResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if(nextElement.empty())
    {
        m_statusCode = getInformationsList(responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();
            if(nextElement.empty())
            {
                m_statusCode = getInformation(responseMsg);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                responseMsg = httpCodeToJSON(m_statusCode, "");
            }
        }
        catch (boost::bad_lexical_cast const& e)
        {
            m_statusCode = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(m_statusCode, e);
        }
    }

    response.setStatus(this->m_statusCode);
    response.out() << responseMsg;
    return;
}

void InformationResource::processPostRequest(Wt::Http::Response &response)
{   
    return ;
}

void InformationResource::processPutRequest(Wt::Http::Response &response)
{
    return;
}

void InformationResource::processDeleteRequest(Wt::Http::Response &response)
{    
    return;
}

