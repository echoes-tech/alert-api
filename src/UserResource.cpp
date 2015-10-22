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

UserResource::UserResource(Echoes::Dbo::Session& session) : PublicApiResource::PublicApiResource(session)
{
    resourceClassName = "UserResource";

    functionMap["getUsersList"] = boost::bind(&UserResource::getUsersList, this, _1, _2, _3, _4, _5);
    functionMap["getUser"] = boost::bind(&UserResource::getUser, this, _1, _2, _3, _4, _5);
    functionMap["postActionForUser"] = boost::bind(&UserResource::postActionForUser, this, _1, _2, _3, _4, _5);
    functionMap["putUser"] = boost::bind(&UserResource::putUser, this, _1, _2, _3, _4, _5);
    
    calls = FillCallsVector();
    
    /*Call structFillTmp;
    
    structFillTmp.method = "GET";
    structFillTmp.path = "";
    structFillTmp.function = boost::bind(&UserResource::getUsersList, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "GET";
    structFillTmp.path = "/[0-9]+";
    structFillTmp.function = boost::bind(&UserResource::getUser, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "GET";
    structFillTmp.path = "/(\\D)*";
    structFillTmp.function = boost::bind(&UserResource::Error, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "POST";
    structFillTmp.path = "";
    structFillTmp.function = boost::bind(&UserResource::Error, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "POST";
    structFillTmp.path = "/action";
    structFillTmp.function = boost::bind(&UserResource::postActionForUser, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "POST";
    structFillTmp.path = "/.*";
    structFillTmp.function = boost::bind(&UserResource::Error, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "PUT";
    structFillTmp.path = "";
    structFillTmp.function = boost::bind(&UserResource::Error, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "PUT";
    structFillTmp.path = "/[0-9]+";
    structFillTmp.function = boost::bind(&UserResource::putUser, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "PUT";
    structFillTmp.path = "/(\\D)*";
    structFillTmp.function = boost::bind(&UserResource::Error, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "DELETE";
    structFillTmp.path = ".*";
    structFillTmp.function = boost::bind(&UserResource::Error, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);*/
}

UserResource::~UserResource()
{
}

EReturnCode UserResource::getUsersList(const long long &grpId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::User>> usrPtrCol = m_session.find<Echoes::Dbo::User>()
                .where(QUOTE(TRIGRAM_USER SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_USER SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = ?").bind(grpId)
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

EReturnCode UserResource::getUser(const std::vector<std::string> &pathElements, const long long &grpId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::User> usrPtr = m_session.find<Echoes::Dbo::User>()
                .where(QUOTE(TRIGRAM_USER ID) " = ?").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_USER SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_USER SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = ?").bind(grpId);

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

EReturnCode UserResource::processGetRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    string nextElement = "";
    unsigned short indexPathElement = 1;
    vector<string> pathElements;
    map<string, long long> parameters;

    const string sRequest = processRequestParameters(request, pathElements, parameters);

    nextElement = getNextElementFromPath(indexPathElement, pathElements);
    if (nextElement.empty())
    {
        res = getUsersList(grpId, responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.empty())
            {
                res = getUser(pathElements, grpId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[User Resource] bad nextElement";
                responseMsg = httpCodeToJSON(res, err);
            }
        }
        catch (boost::bad_lexical_cast const& e)
        {
            res = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(res, e);
        }
    }

    return res;
}

EReturnCode UserResource::postActionForUser(const string& sRequest, const long long &grpId, string& responseMsg)
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
            Wt::Dbo::Transaction transaction(m_session, true);

            Wt::Dbo::ptr<Echoes::Dbo::UserActionType> uatPtr = m_session.find<Echoes::Dbo::UserActionType>()
                    .where(QUOTE(TRIGRAM_USER_ACTION_TYPE ID) " = ?").bind(uacId);

            Echoes::Dbo::UserHistoricalAction *newUha = new Echoes::Dbo::UserHistoricalAction();
            newUha->tableObject = tableObject;
            newUha->tableObjectId = tableObjectId;
            newUha->userAction = uatPtr;
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

EReturnCode UserResource::processPostRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    string nextElement = "";
    unsigned short indexPathElement = 1;
    vector<string> pathElements;
    map<string, long long> parameters;

    const string sRequest = processRequestParameters(request, pathElements, parameters);

    nextElement = getNextElementFromPath(indexPathElement, pathElements);
    if (nextElement.empty())
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[User Resource] bad nextElement";
        responseMsg = httpCodeToJSON(res, err);
    }
    else
    {
        if(nextElement.compare("action") == 0)
        {
            res = postActionForUser(sRequest, grpId, responseMsg);
        }
        else
        {
            res = EReturnCode::BAD_REQUEST;
            const string err = "[User Resource] bad nextElement";
            responseMsg = httpCodeToJSON(res, err);
        }
    }

    return res;
}

EReturnCode UserResource::putUser(const std::vector<std::string> &pathElements, const string &sRequest, const long long &grpId, string &responseMsg)
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
            Wt::Dbo::Transaction transaction(m_session, true);

            Wt::Dbo::ptr<Echoes::Dbo::User> usrPtr = m_session.find<Echoes::Dbo::User>()
                .where(QUOTE(TRIGRAM_USER ID) " = ?").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_USER SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_USER SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = ?").bind(grpId);

            if (usrPtr)
            {
                if (uroId > 0)
                {
                    Wt::Dbo::ptr<Echoes::Dbo::UserRole> usoPtr = m_session.find<Echoes::Dbo::UserRole>()
                        .where(QUOTE(TRIGRAM_USER_ROLE ID) " = ?").bind(uroId)
                        .where(QUOTE(TRIGRAM_USER_ROLE SEP "DELETE") " IS NULL")
                        .where(QUOTE(TRIGRAM_USER_ROLE SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = ?").bind(grpId);

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

EReturnCode UserResource::processPutRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    string nextElement = "";
    unsigned short indexPathElement = 1;
    vector<string> pathElements;
    map<string, long long> parameters;

    const string sRequest = processRequestParameters(request, pathElements, parameters);

    nextElement = getNextElementFromPath(indexPathElement, pathElements);
    if (nextElement.empty())
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[User Resource] bad nextElement";
        responseMsg = httpCodeToJSON(res, err);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);

            if (nextElement.empty())
            {
                res = putUser(pathElements, sRequest, grpId, responseMsg);
                res = putUser(orgId, responseMsg, pathElements, sRequest);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[User Resource] bad nextElement";
                responseMsg = httpCodeToJSON(res, err);
            }
        }
        catch (boost::bad_lexical_cast const& e)
        {
            res = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(res, e);
        }
    }

    return res;
}

