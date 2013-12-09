/* 
 * API AddonResource
 * @author ECHOES Technologies (GDR)
 * @date 6/03/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "AddonResource.h"

using namespace std;

AddonResource::AddonResource() : PublicApiResource::PublicApiResource()
{
}

AddonResource::~AddonResource()
{
}

EReturnCode AddonResource::getAddonsList(string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Addon>> adoPtrCol = m_session.find<Echoes::Dbo::Addon>()
                .where(QUOTE(TRIGRAM_ADDON SEP "DELETE") " IS NULL")
                .orderBy(QUOTE(TRIGRAM_ADDON ID));

        res = serialize(adoPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode AddonResource::getAddon(string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::ptr<Echoes::Dbo::Addon> adoPtr = m_session.find<Echoes::Dbo::Addon>()
                .where(QUOTE(TRIGRAM_ADDON ID) " = ?").bind(m_pathElements[1])
                .where(QUOTE(TRIGRAM_ADDON SEP "DELETE") " IS NULL");

        res = serialize(adoPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

void AddonResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();

    if (nextElement.empty())
    {
        m_statusCode = getAddonsList(responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);
            nextElement = getNextElementFromPath();
            if (nextElement.empty())
            {
                m_statusCode = getAddon(responseMsg);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                const string err = "[Addon Resource] bad nextElement";
                responseMsg = httpCodeToJSON(m_statusCode, err);
            }
        }
        catch (boost::bad_lexical_cast const& e)
        {
            m_statusCode = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(m_statusCode, e);
        }
    }

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}

