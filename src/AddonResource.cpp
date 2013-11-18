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

unsigned short AddonResource::getSearchTypeForAddon(string &responseMsg)
{
    unsigned short res = EReturnCode::INTERNAL_SERVER_ERROR;
    unsigned idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);
        string queryStr = "SELECT set FROM \"T_SEARCH_TYPE_STY\" set "
                " WHERE \"STY_ID\" IN"
                "("
                "SELECT \"T_SEARCH_TYPE_STY_STY_ID\" FROM \"TJ_ADO_STY\" "
                "WHERE \"T_ADDON_ADO_ADO_ID\" = " + m_pathElements[1] +
                ")";
        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::SearchType> > queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::SearchType> >(queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SearchType> > seaTypePtr = queryRes.resultList();

        if (seaTypePtr.size() != 0)
        {
            responseMsg = "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SearchType> >::const_iterator i = seaTypePtr.begin(); i != seaTypePtr.end(); ++i)
            {
                i->modify()->setId(i->id());
                responseMsg += "\t" + i->get()->toJSON();
                ++idx;
                if (seaTypePtr.size() - idx > 0)
                {
                    responseMsg += ",\n";
                }
            }
            responseMsg += "]\n";
            res = EReturnCode::OK;
        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = "{\"message\":\"Search parameter not found\"}";
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

unsigned short AddonResource::getParameterForAddon(string& responseMsg)
{
    unsigned short res = EReturnCode::INTERNAL_SERVER_ERROR;
    unsigned idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);


        string queryStr = "SELECT srp FROM \"T_SOURCE_PARAMETER_SRP\" srp"
                " WHERE \"SRP_ID\" IN "
                "("
                "SELECT \"T_SOURCE_PARAMETER_SRP_SRP_ID\" FROM \"TJ_ADO_SRP\" WHERE \"T_ADDON_ADO_ADO_ID\" ="
                + m_pathElements[1]
                + " )";

        Wt::Dbo::Query < Wt::Dbo::ptr < Echoes::Dbo::SourceParameter >> queryRes = m_session.query < Wt::Dbo::ptr < Echoes::Dbo::SourceParameter >> (queryStr);

        Wt::Dbo::collection < Wt::Dbo::ptr < Echoes::Dbo::SourceParameter >> srcParamPtr = queryRes.resultList();
        if (srcParamPtr.size() > 0)
        {
            responseMsg = "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SourceParameter> >::const_iterator i = srcParamPtr.begin(); i != srcParamPtr.end(); ++i)
            {
                i->modify()->setId(i->id());
                responseMsg += "\t" + i->get()->toJSON();
                ++idx;
                if (srcParamPtr.size() - idx > 0)
                {
                    responseMsg += ",\n";
                }
            }
            responseMsg += "]\n";
            res = EReturnCode::OK;
        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = "{\"message\":\"Source parameter not found\"}";
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

EReturnCode AddonResource::getAddonList(string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session);
        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Addon>> adoCol = m_session.find<Echoes::Dbo::Addon>().orderBy(QUOTE(TRIGRAM_ADDON ID));
        
        res = this->serialize(adoCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = this->httpCodeToJSON(res, e);
    }

    return res;
}

EReturnCode AddonResource::getAddon(string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);
        Wt::Dbo::ptr<Echoes::Dbo::Addon> adoPtr = m_session
                .find<Echoes::Dbo::Addon>()
                .where(QUOTE(TRIGRAM_ADDON ID) " = ?")
                .bind(m_pathElements[1]);
        res = this->serialize(adoPtr, responseMsg);
        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = this->httpCodeToJSON(res, e);
    }
    return res;
}

void AddonResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";

    nextElement = getNextElementFromPath();

    if (!nextElement.compare(""))
    {
        m_statusCode = (unsigned short) getAddonList(responseMsg);
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
            else if (!nextElement.compare("parameters"))
            {
                m_statusCode = getParameterForAddon(responseMsg);
            }
            else if (!nextElement.compare("search_types"))
            {
                m_statusCode = getSearchTypeForAddon(responseMsg);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch (boost::bad_lexical_cast &)
        {
            m_statusCode = EReturnCode::BAD_REQUEST;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}

