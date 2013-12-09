/* 
 * API UserResource
 * @author ECHOES Technologies (GDR)
 * @date 05/02/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "UserResource.h"

using namespace std;

UserResource::UserResource() : PublicApiResource::PublicApiResource()
{
}

UserResource::~UserResource()
{
}

EReturnCode UserResource::getUsersList(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::User>> usrPtrCol = m_session.find<Echoes::Dbo::User>()
                .where(QUOTE(TRIGRAM_USER SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_USER SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(m_session.user()->organization.id())
                .orderBy(QUOTE(TRIGRAM_USER ID));

        res = serialize(usrPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode UserResource::getUser(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::ptr<Echoes::Dbo::User> usrPtr = m_session.find<Echoes::Dbo::User>()
                .where(QUOTE(TRIGRAM_USER ID) " = ?").bind(m_pathElements[1])
                .where(QUOTE(TRIGRAM_USER SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_USER SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(m_session.user()->organization.id());

        res = serialize(usrPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

void UserResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = getUsersList(responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();
            if (nextElement.empty())
            {
                m_statusCode = getUser(responseMsg);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                const string err = "[User Resource] bad nextElement";
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

EReturnCode UserResource::postActionForUser(std::string &responseMsg, const std::string &sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    Wt::WString tableObject;
    int actionAfter;
    int actionBefore;
    int actionRelative;
    long long uacId;
    long long tableObjectId;

    try
    {
        
        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);

        uacId = result.get("user_action_id");
        tableObject = result.get("table_object");
        tableObjectId = result.get("table_object_id");
        actionAfter = result.get("action_after");
        actionBefore = result.get("action_before");
        actionRelative = result.get("action_relative");
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

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session);

            Wt::Dbo::ptr<Echoes::Dbo::UserAction> uacPtr = m_session.find<Echoes::Dbo::UserAction>().where(QUOTE(TRIGRAM_USER_ACTION ID) " = ?").bind(uacId);

            Echoes::Dbo::UserHistoricalAction *newUha = new Echoes::Dbo::UserHistoricalAction();
            newUha->tableObject = tableObject;
            newUha->tableObjectId = tableObjectId;
            newUha->userAction = uacPtr;
            newUha->user = m_session.user();
            newUha->dateTime = Wt::WDateTime::currentDateTime();
            newUha->actionAfter = actionAfter;
            newUha->actionBefore = actionBefore;
            newUha->actionRelative = actionRelative;

            Wt::Dbo::ptr<Echoes::Dbo::UserHistoricalAction> newUhaPtr = m_session.add<Echoes::Dbo::UserHistoricalAction>(newUha);
            newUhaPtr.flush();

            res = serialize(newUhaPtr, responseMsg, EReturnCode::CREATED);

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

void UserResource::processPostRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
        const string err = "[User Resource] bad nextElement";
        responseMsg = httpCodeToJSON(m_statusCode, err);
    }
    else
    {
        if(nextElement.compare("action") == 0)
        {
            m_statusCode = postActionForUser(responseMsg, m_requestData);
        }
        else
        {
            m_statusCode = EReturnCode::BAD_REQUEST;
            const string err = "[User Resource] bad nextElement";
            responseMsg = httpCodeToJSON(m_statusCode, err);
        }
    }

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}

EReturnCode UserResource::putUser(string &responseMsg, const string &sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    Wt::WString firstName;
    Wt::WString lastName;
    Wt::WString token;
    long long uroId = 0;

    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            if (result.contains("first_name"))
            {
                firstName = result.get("first_name");
            }
            if (result.contains("last_name"))
            {
                lastName = result.get("last_name");
            }
            if (result.contains("token"))
            {
                token = result.get("token");
            }
            if (result.contains("user_role_id"))
            {
                uroId = result.get("user_role_id");
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
        const string err = "[User Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session);

            Wt::Dbo::ptr<Echoes::Dbo::User> usrPtr = m_session.find<Echoes::Dbo::User>()
                .where(QUOTE(TRIGRAM_USER ID) " = ?").bind(m_pathElements[1])
                .where(QUOTE(TRIGRAM_USER SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_USER SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(m_session.user()->organization.id());

            if (usrPtr)
            {
                if (uroId > 0)
                {
                    Wt::Dbo::ptr<Echoes::Dbo::UserRole> usoPtr = m_session.find<Echoes::Dbo::UserRole>()
                        .where(QUOTE(TRIGRAM_USER_ROLE ID) " = ?").bind(uroId)
                        .where(QUOTE(TRIGRAM_USER_ROLE SEP "DELETE") " IS NULL")
                        .where(QUOTE(TRIGRAM_USER_ROLE SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(m_session.user()->organization.id());

                    if (usrPtr)
                    {
                        usrPtr.modify()->userRole = usoPtr;
                    }
                    else
                    {
                        res = EReturnCode::NOT_FOUND;
                        responseMsg = httpCodeToJSON(res, usoPtr);
                    }
                }

                if (responseMsg.empty())
                {
                    if (!firstName.empty())
                    {
                        usrPtr.modify()->firstName = firstName;
                    }

                    if (!lastName.empty())
                    {
                        usrPtr.modify()->firstName = lastName;
                    }

                    if (!token.empty())
                    {
                        usrPtr.modify()->token = token;
                    }

                    res = serialize(usrPtr, responseMsg);
                }
            }
            else
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, usrPtr);
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

void UserResource::processPutRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
        const string err = "[User Resource] bad nextElement";
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
                m_statusCode = putUser(responseMsg, m_requestData);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                const string err = "[User Resource] bad nextElement";
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

