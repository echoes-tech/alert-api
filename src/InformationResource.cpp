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
    m_parameters["media_type_id"] = 0;
    m_parameters["user_role_id"] = 0;
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

        Wt::Dbo::ptr<Echoes::Dbo::Information> infPtr = m_session.find<Echoes::Dbo::Information>()
                .where(QUOTE(TRIGRAM_INFORMATION ID) " = ?").bind(m_pathElements[1])
                .where(QUOTE(TRIGRAM_INFORMATION SEP "DELETE") " IS NULL");

        res = serialize(infPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode InformationResource::getAliasForInformation(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    if (m_parameters["media_type_id"] <= 0 || m_parameters["user_role_id"] <= 0)
    {
        const string err = "[Assert Resource] media_types or/and user_role are empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session);

            Wt::Dbo::ptr<Echoes::Dbo::UserRole> uroPtr = m_session.find<Echoes::Dbo::UserRole>()
                    .where(QUOTE(TRIGRAM_USER_ROLE ID) " = ?").bind(m_parameters["user_role_id"])
                    .where(QUOTE(TRIGRAM_USER_ROLE SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(m_session.user()->organization.id())
                    .where(QUOTE(TRIGRAM_USER_ROLE SEP "DELETE") " IS NULL");
            if (uroPtr)
            {
                Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasInformation> aaiPtr = m_session.find<Echoes::Dbo::AlertMessageAliasInformation>()
                        .where(QUOTE(TRIGRAM_ALERT_MESSAGE_ALIAS_INFORMATION SEP "DELETE") " IS NULL")
                        .where(QUOTE(TRIGRAM_USER_ROLE ID SEP TRIGRAM_USER_ROLE ID) " = ?").bind(m_parameters["user_role_id"])
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE ID SEP TRIGRAM_MEDIA_TYPE ID) " = ?").bind(m_parameters["media_type_id"])
                        .where(QUOTE(TRIGRAM_INFORMATION ID SEP TRIGRAM_INFORMATION ID) " = ?").bind(m_pathElements[1]);

                res = serialize(aaiPtr, responseMsg);
            }
            else
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, uroPtr);
            }

            transaction.commit();
        }
        catch (Wt::Dbo::Exception const& e)
        {
            res = EReturnCode::SERVICE_UNAVAILABLE;
            responseMsg = httpCodeToJSON(res, e);
        }
    }

    return res;
}

void InformationResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = getInformationsList(responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();
            if (nextElement.empty())
            {
                m_statusCode = getInformation(responseMsg);
            }
            else if (!nextElement.compare("alias"))
            {
                m_statusCode = getAliasForInformation(responseMsg);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                const string err = "[Information Resource] bad nextElement";
                responseMsg = httpCodeToJSON(m_statusCode, err);
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

EReturnCode InformationResource::postInformation(string& responseMsg, const string& sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    Wt::WString name;
    Wt::WString calculate;
    Wt::WString desc;
    bool display;
    long long inuId;

    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            name = result.get("name");
            desc = result.get("desc");
            display = result.get("display");
            inuId = result.get("unit_id");
            if(result.contains("calculate"))
            {
                calculate = result.get("calculate");
            }
        }
        catch (Wt::Json::ParseError const& e)
        {
            res = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(res, e);
        }
        catch (Wt::Json::TypeException const& e)
        {
            res = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(res, e);
        }
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Information Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }


    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session);

            Wt::Dbo::ptr<Echoes::Dbo::InformationUnit> inuPtr = m_session.find<Echoes::Dbo::InformationUnit>()
                    .where(QUOTE(TRIGRAM_INFORMATION_UNIT ID) " = ?").bind(inuId)
                    .where(QUOTE(TRIGRAM_INFORMATION_UNIT SEP "DELETE") " IS NULL");
            if (inuPtr)
            {
                Echoes::Dbo::Information *newInf = new Echoes::Dbo::Information();
                newInf->name = name;
                newInf->display = display;
                newInf->calculate = calculate;
                newInf->desc = desc;
                newInf->informationUnit = inuPtr;

                Wt::Dbo::ptr<Echoes::Dbo::Information> newInfPtr = m_session.add<Echoes::Dbo::Information>(newInf);
                newInfPtr.flush();

                res = serialize(newInfPtr, responseMsg, EReturnCode::CREATED);
            }
            else
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, inuPtr);
            }

            transaction.commit();
        }
        catch (Wt::Dbo::Exception const& e)
        {
            res = EReturnCode::SERVICE_UNAVAILABLE;
            responseMsg = httpCodeToJSON(res, e);
        }
    }

    return res; 
}

void InformationResource::processPostRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = postInformation(responseMsg, m_requestData);
    }
    else
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
        const string err = "[Information Resource] bad nextElement";
        responseMsg = httpCodeToJSON(m_statusCode, err);
    }

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}

EReturnCode InformationResource::putAliasForInformation(string &responseMsg, const string &sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    long long uroId;
    long long mtyId;
    Wt::WString value;
    
    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            uroId = result.get("user_role_id");
            mtyId = result.get("media_type_id");
            value = result.get("value");
        }
        catch (Wt::Json::ParseError const& e)
        {
            res = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(res, e);
        }
        catch (Wt::Json::TypeException const& e)
        {
            res = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(res, e);
        }
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Information Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session);

            Wt::Dbo::ptr<Echoes::Dbo::Information> infPtr =  m_session.find<Echoes::Dbo::Information>()
                .where(QUOTE(TRIGRAM_INFORMATION ID) " = ?").bind(m_pathElements[1])
                .where(QUOTE(TRIGRAM_INFORMATION SEP "DELETE") " IS NULL");
            if (infPtr)
            {
                Wt::Dbo::ptr<Echoes::Dbo::UserRole> uroPtr = m_session.find<Echoes::Dbo::UserRole>()
                        .where(QUOTE(TRIGRAM_USER_ROLE ID) " = ?").bind(uroId)
                        .where(QUOTE(TRIGRAM_USER_ROLE SEP "DELETE") " IS NULL");
                if (!uroPtr)
                {
                    res = EReturnCode::NOT_FOUND;
                    responseMsg = httpCodeToJSON(res, uroPtr);
                    return res;
                }

                Wt::Dbo::ptr<Echoes::Dbo::MediaType> mtyPtr = m_session.find<Echoes::Dbo::MediaType>()
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE ID) " = ?").bind(mtyId)
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE SEP "DELETE") " IS NULL");
                if (!mtyPtr)
                {
                    res = EReturnCode::NOT_FOUND;
                    responseMsg = httpCodeToJSON(res, mtyPtr);
                    return res;
                }

                Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasInformation> aaiPtr = m_session.find<Echoes::Dbo::AlertMessageAliasInformation>()
                        .where(QUOTE(TRIGRAM_USER_ROLE ID SEP TRIGRAM_USER_ROLE ID) " = ?").bind(uroId)
                        .where(QUOTE(TRIGRAM_INFORMATION ID SEP TRIGRAM_INFORMATION ID) " = ?").bind(m_pathElements[1])
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE ID SEP TRIGRAM_MEDIA_TYPE ID) " = ?").bind(mtyId);
                if (aaiPtr)
                {
                    aaiPtr.modify()->alias = value;
                }
                else
                {
                    Echoes::Dbo::AlertMessageAliasInformation *newAai = new Echoes::Dbo::AlertMessageAliasInformation();
                    newAai->pk.information = infPtr;
                    newAai->pk.userRole = uroPtr;
                    newAai->pk.mediaType = mtyPtr;
                    newAai->alias = value;
                    aaiPtr = m_session.add<Echoes::Dbo::AlertMessageAliasInformation>(newAai);
                }
                res = EReturnCode::OK;
            }
            else
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, infPtr);
            }
            transaction.commit();
        }
        catch (Wt::Dbo::Exception const& e)
        {
            res = EReturnCode::SERVICE_UNAVAILABLE;
            responseMsg = httpCodeToJSON(res, e);
        }
    }

    return res; 
}

void InformationResource::processPutRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
        const string err = "[Information Resource] bad nextElement";
        responseMsg = httpCodeToJSON(m_statusCode, err);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();

            if (!nextElement.compare("alias"))
            {
                m_statusCode = putAliasForInformation(responseMsg, m_requestData);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                const string err = "[Information Resource] bad nextElement";
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

EReturnCode InformationResource::deleteInformation(string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try 
    {  
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::ptr<Echoes::Dbo::Information> infPtr = m_session.find<Echoes::Dbo::Information>()
                .where(QUOTE(TRIGRAM_INFORMATION ID) " = ?").bind(m_pathElements[1])
                .where(QUOTE(TRIGRAM_INFORMATION SEP "DELETE") " IS NULL");

        if(infPtr)
        {
            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationData>> idaPtrCol = m_session.find<Echoes::Dbo::InformationData>()
                    .where(QUOTE(TRIGRAM_INFORMATION_DATA SEP TRIGRAM_INFORMATION SEP TRIGRAM_INFORMATION ID)" = ?").bind(m_pathElements[1])
                    .where(QUOTE(TRIGRAM_INFORMATION_DATA SEP "DELETE") " IS NULL");

            if (idaPtrCol.size() == 0)
            {
                infPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
                res = EReturnCode::NO_CONTENT;
            }
            else
            {
                res = EReturnCode::CONFLICT;
                responseMsg = httpCodeToJSON(res, infPtr);
            }
        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = httpCodeToJSON(res, infPtr);
        }

        transaction.commit();
    }
    catch (boost::bad_lexical_cast const& e)
    {
        res = EReturnCode::BAD_REQUEST;
        responseMsg = httpCodeToJSON(res, e);
    }

    return res;
}

void InformationResource::processDeleteRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
        const string err = "[Filter Resource] bad nextElement";
        responseMsg = httpCodeToJSON(m_statusCode, err);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();

            if (nextElement.empty())
            {
                m_statusCode = deleteInformation(responseMsg);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                const string err = "[Filter Resource] bad nextElement";
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

