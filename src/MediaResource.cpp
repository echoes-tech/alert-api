/* 
 * API MediaResource
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
#include "MediaResource.h"

using namespace std;

MediaResource::MediaResource() : PublicApiResource::PublicApiResource()
{
}

MediaResource::~MediaResource()
{
}

Wt::Dbo::ptr<Echoes::Dbo::Media> MediaResource::selectMedia(const long long &medId, Echoes::Dbo::Session &session)
{
    return selectMedia(boost::lexical_cast<string>(medId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::Media> MediaResource::selectMedia(const string &medId, Echoes::Dbo::Session &session)
{
    string queryStr = 
" SELECT med"
"   FROM " QUOTE("T_MEDIA_MED") " med"
"   WHERE"
"     " QUOTE(TRIGRAM_MEDIA ID) " = " + medId +
"     AND " QUOTE(TRIGRAM_USER ID) " IN"
"       ("
"         SELECT " QUOTE(TRIGRAM_USER ID)
"           FROM " QUOTE("T_USER_USR")
"           WHERE " QUOTE(TRIGRAM_USER SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = " + boost::lexical_cast<string>(session.user()->organization.id()) +
"             AND " QUOTE(TRIGRAM_USER SEP "DELETE") " IS NULL"
"       )"
"     AND " QUOTE(TRIGRAM_MEDIA SEP "DELETE") " IS NULL"
"   LIMIT 1";
 
    Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Media>> queryRes = session.query<Wt::Dbo::ptr<Echoes::Dbo::Media>>(queryStr);

    return queryRes.resultValue();
}

EReturnCode MediaResource::getMediasList(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session);
        string queryStr = 
" SELECT med"
"   FROM " QUOTE("T_MEDIA_MED") " med"
"   WHERE"
"     " QUOTE(TRIGRAM_USER ID) " IN"
"       ("
"         SELECT " QUOTE(TRIGRAM_USER ID)
"           FROM " QUOTE("T_USER_USR")
"           WHERE " QUOTE(TRIGRAM_USER SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = " + boost::lexical_cast<string>(this->m_session.user()->organization.id()) +
"             AND " QUOTE(TRIGRAM_USER SEP "DELETE") " IS NULL"
"       )"
"     AND " QUOTE(TRIGRAM_MEDIA SEP "DELETE") " IS NULL";
 
        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Media>> queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::Media>>(queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Media>> medPtrCol = queryRes.resultList();

        res = serialize(medPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode MediaResource::getMedia(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::ptr<Echoes::Dbo::Media> medPtrCol = selectMedia(m_pathElements[1], m_session);

        res = serialize(medPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

void MediaResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if(nextElement.empty())
    {
        m_statusCode = getMediasList(responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();
            if(nextElement.empty())
            {
                m_statusCode = getMedia(responseMsg);
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

EReturnCode MediaResource::postMedia(string &responseMsg, const string &sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    long long mtyId;
    Wt::WString value;

    if(!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

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
        responseMsg = httpCodeToJSON(res, "");
    }

    if(responseMsg.empty())
    {        
        try
        {
            Wt::Dbo::Transaction transaction(m_session);

            Wt::Dbo::ptr<Echoes::Dbo::MediaType> mtyPtr = m_session.find<Echoes::Dbo::MediaType>()
                    .where(QUOTE(TRIGRAM_MEDIA_TYPE ID) " = ?").bind(mtyId)
                    .where(QUOTE(TRIGRAM_MEDIA_TYPE SEP "DELETE") " IS NULL");
            if (mtyPtr)
            {
                Echoes::Dbo::Media *newMed = new Echoes::Dbo::Media();
                newMed->user= m_session.user();
                newMed->mediaType = mtyPtr;
                newMed->value = value;
                newMed->token = Wt::WRandom::generateId(25);
                Wt::Dbo::ptr<Echoes::Dbo::Media> newMedPtr = m_session.add<Echoes::Dbo::Media>(newMed);
                newMedPtr.flush();

                res = serialize(newMedPtr, responseMsg, EReturnCode::CREATED);

                transaction.commit();
            }
            else
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, mtyPtr);
            }
        }
        catch (Wt::Dbo::Exception const& e)
        {
            res = EReturnCode::SERVICE_UNAVAILABLE;
            responseMsg = httpCodeToJSON(res, e);
        }
    }

    return res;
}

void MediaResource::processPostRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if(nextElement.empty())
    {
        m_statusCode = postMedia(responseMsg, m_requestData);
    }
    else
    {   
        m_statusCode = EReturnCode::BAD_REQUEST;
        responseMsg = httpCodeToJSON(m_statusCode, "");
    }

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}

EReturnCode MediaResource::putMedia(string &responseMsg, const string &sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    Wt::WString token;
    Wt::WString value;
    bool isConfirmed;
    bool isConfirmedIsPresent = false;
    bool isDefault;
    bool isDefaultIsPresent = false;

    if(!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            if(result.contains("token"))
            {
                token = result.get("token");
            }
            if(result.contains("value"))
            {
                value = result.get("value");
            }
            if(result.contains("is_confirmed"))
            {
                isConfirmed = result.get("is_confirmed");
                isConfirmedIsPresent = true;
            }
            if(result.contains("is_default"))
            {
                isDefault = result.get("is_default");
                isDefaultIsPresent = true;
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
        responseMsg = httpCodeToJSON(res, "");
    }

    if(responseMsg.empty())
    {
        try 
        {
            Wt::Dbo::Transaction transaction(m_session);

            Wt::Dbo::ptr<Echoes::Dbo::Media> medPtr = selectMedia(m_pathElements[1], m_session);

            if (medPtr) 
            {
                if(!token.empty())
                {
                    medPtr.modify()->token = token;
                }

                if(!value.empty())
                {
                    medPtr.modify()->value = value;
                }

                if(isConfirmedIsPresent)
                {
                    medPtr.modify()->isConfirmed = isConfirmed;
                }

                if(isDefaultIsPresent)
                {
                    medPtr.modify()->isDefault = isDefault;
                }

                res = serialize(medPtr, responseMsg);
            } 
            else 
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, medPtr);
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

void MediaResource::processPutRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if(nextElement.empty())
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
        responseMsg = httpCodeToJSON(m_statusCode, "");
    }
    else
    {   
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();

            if(nextElement.empty())
            {
                m_statusCode = putMedia(responseMsg, m_requestData);
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

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}

EReturnCode MediaResource::deleteMedia(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try 
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::ptr<Echoes::Dbo::Media> medPtr = selectMedia(m_pathElements[1], m_session);

        if (medPtr) 
        {
            medPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
        } 
        else 
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = httpCodeToJSON(res, medPtr);
        }
    }
    catch(boost::bad_lexical_cast const& e)
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
        responseMsg = httpCodeToJSON(m_statusCode, e);
    }

    return res;
}

void MediaResource::processDeleteRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if(nextElement.empty())
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
        responseMsg = httpCodeToJSON(m_statusCode, "");
    }
    else
    {   
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();

            if(nextElement.empty())
            {
                m_statusCode = deleteMedia(responseMsg);
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

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}

