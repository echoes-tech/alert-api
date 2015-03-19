/* 
 * API RoleResource
 * @author ECHOES Technologies (TSA)
 * @date 08/06/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "RoleResource.h"

using namespace std;

RoleResource::RoleResource(Echoes::Dbo::Session& session) : PublicApiResource::PublicApiResource(session)
{
    /*Call structFillTmp;
    
    structFillTmp.method = "GET";
    structFillTmp.path = "";
    structFillTmp.function = boost::bind(&RoleResource::getRolesList, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "GET";
    structFillTmp.path = "/[0-9]+";
    structFillTmp.function = boost::bind(&RoleResource::getRole, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "GET";
    structFillTmp.path = "/(\\D)*";
    //structFillTmp.function = boost::bind(&RoleResource::Error, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "POST";
    structFillTmp.path = "";
    structFillTmp.function = boost::bind(&RoleResource::postRole, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "POST";
    structFillTmp.path = ".+";
    //structFillTmp.function = boost::bind(&RoleResource::Error, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "PUT";
    structFillTmp.path = "";
    structFillTmp.function = boost::bind(&RoleResource::Error, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "PUT";
    structFillTmp.path = "/[0-9]+";
    structFillTmp.function = boost::bind(&RoleResource::putRole, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "PUT";
    structFillTmp.path = "/(\\D)*";
    structFillTmp.function = boost::bind(&RoleResource::Error, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "DELETE";
    structFillTmp.path = ".*";
    structFillTmp.function = boost::bind(&RoleResource::Error, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);*/
}

RoleResource::~RoleResource()
{
}

EReturnCode RoleResource::getRolesList(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::UserRole>> uroPtrCol = m_session.find<Echoes::Dbo::UserRole>()
                .where(QUOTE(TRIGRAM_USER_ROLE SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_USER_ROLE SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(orgId)
                .orderBy(QUOTE(TRIGRAM_USER_ROLE ID));

        res = serialize(uroPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode RoleResource::getRole(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::UserRole> uroPtr = m_session.find<Echoes::Dbo::UserRole>()
                .where(QUOTE(TRIGRAM_USER_ROLE ID) " = ?").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_USER_ROLE SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_USER_ROLE SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(orgId);

        res = serialize(uroPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode RoleResource::processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        res = getRolesList(orgId, responseMsg, pathElements);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.empty())
            {
                res = getRole(orgId, responseMsg, pathElements);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Role Resource] bad nextElement";
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

EReturnCode RoleResource::postRole(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    Wt::WString name;

    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            name = result.get("name");
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
        const string err = "[Role Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session, true);

            Wt::Dbo::ptr<Echoes::Dbo::Organization> orgPtr = m_session.find<Echoes::Dbo::Organization>()
                    .where(QUOTE(TRIGRAM_ORGANIZATION SEP "DELETE") " IS NULL")
                    .where(QUOTE(TRIGRAM_ORGANIZATION ID) " = ?").bind(orgId);
            if (!orgPtr)
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, orgPtr);
                return res;
            }

            Echoes::Dbo::UserRole *newUro = new Echoes::Dbo::UserRole();
            newUro->name = name;
            newUro->organization = orgPtr;

            Wt::Dbo::ptr<Echoes::Dbo::UserRole> newUroPtr = m_session.add<Echoes::Dbo::UserRole>(newUro);
            newUroPtr.flush();

            res = serialize(newUroPtr, responseMsg, EReturnCode::CREATED);

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

EReturnCode RoleResource::processPostRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        res = postRole(orgId, responseMsg, pathElements, sRequest);
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Role Resource] bad nextElement";
        responseMsg = httpCodeToJSON(res, err);
    }

    return res;
}

EReturnCode RoleResource::putRole(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    Wt::WString name;

    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            if (result.contains("name"))
            {
                name = result.get("name");
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
        const string err = "[Role Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session, true);

            Wt::Dbo::ptr<Echoes::Dbo::UserRole> uroPtr = m_session.find<Echoes::Dbo::UserRole>()
                .where(QUOTE(TRIGRAM_USER_ROLE ID) " = ?").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_USER_ROLE SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_USER_ROLE SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(orgId);

            if (uroPtr)
            {
                if (!name.empty())
                {
                    uroPtr.modify()->name = name;
                }

                res = serialize(uroPtr, responseMsg);
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

EReturnCode RoleResource::processPutRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        const string err = "[Role Resource] bad nextElement";
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
                res = putRole(orgId, responseMsg, pathElements, sRequest);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Role Resource] bad nextElement";
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