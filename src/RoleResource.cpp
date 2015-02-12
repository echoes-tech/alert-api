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
    Call<RoleResource> tmp;
    tmp.method = "GET";
    tmp.path = "";
    tmp.function = &RoleResource::getRolesList;
    calls.push_back(tmp);
    tmp.method = "GET";
    tmp.path = "/[0-9]+";
    tmp.function = &RoleResource::getRole;
    calls.push_back(tmp);
    tmp.method = "GET";
    tmp.path = "/(\\D)*";
    tmp.function = &RoleResource::Error;
    calls.push_back(tmp);
}

RoleResource::~RoleResource()
{
}

EReturnCode RoleResource::getRolesList(const std::vector<std::string> &pathElements, const long long &orgId, string &responseMsg)
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

EReturnCode RoleResource::Error(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    
    res = EReturnCode::BAD_REQUEST;
    const string err = "[Role Resource] bad nextElement";
    responseMsg = httpCodeToJSON(res, err);
    return res;
}

EReturnCode RoleResource::getRole(const std::vector<std::string> &pathElements, const long long &orgId, string &responseMsg)
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
        res = getRolesList(pathElements, orgId, responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.empty())
            {
                res = getRole(pathElements, orgId, responseMsg);
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

EReturnCode RoleResource::postRole(const string &sRequest, const long long &orgId, string &responseMsg)
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
        res = postRole(sRequest, orgId, responseMsg);
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Role Resource] bad nextElement";
        responseMsg = httpCodeToJSON(res, err);
    }

    return res;
}

EReturnCode RoleResource::putRole(const std::vector<std::string> &pathElements, const string &sRequest, const long long &orgId, string &responseMsg)
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
                res = putRole(pathElements, sRequest, orgId, responseMsg);
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

EReturnCode RoleResource::processRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    vector<Call<RoleResource>>::iterator it = calls.begin();
    vector<string> pathElements;
    
    boost::split(pathElements, (const string)request.pathInfo(), boost::is_any_of("/"), boost::token_compress_on);
    while((!request.method().compare(it.base()->method) && boost::regex_match(request.pathInfo(), it.base()->path) ) && (it != calls.end()))
        it++;
    if (it != calls.end())
    {
        EReturnCode (RoleResource::*function)(const std::vector<std::string> &, const long long &, string &) = it.base()->function;
        res = (this->*function)(pathElements, orgId, responseMsg);
    }
    else
        res = EReturnCode::BAD_REQUEST;
    return res;
}

void RoleResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    Wt::log("info") << "[Role API] Identifying";

    // default : not authentified
    bool authentified = false;
    bool notAllowed = false;

    string login = "";
    string password = "";
    string token = "";
    string eno_token = "";

    long long orgId = 0;

    if (request.getParameter("login") != 0)
    {
        login = Wt::Utils::urlDecode(*request.getParameter("login"));
        if (!login.empty())
        {
            if (request.getParameter("password") != 0)
            {
                password = *request.getParameter("password");
            }
            else if (request.getParameter("token") != 0)
            {
                token = *request.getParameter("token");
            }
            else
            {
                Wt::log("error") << "[Public API Resource] No password or token parameter";
            }
        }
        else
        {
            Wt::log("error") << "[Public API Resource] login is empty";
        }
    }
    else if (request.getParameter("eno_token") != 0)
    {
        eno_token = *request.getParameter("eno_token");
        if (eno_token.empty())
        {
            Wt::log("error") << "[Public API Resource] org_token is empty";
        }
    }
    else
    {
        Wt::log("error") << "[Public API Resource] No login or eno_token parameter";
    }

    if (!login.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session, true);

            // check whether the user exists
            Wt::Dbo::ptr<AuthInfo::AuthIdentityType> authIdType = m_session.find<AuthInfo::AuthIdentityType > ().where("\"identity\" = ?").bind(login);
            if (Utils::checkId<AuthInfo::AuthIdentityType > (authIdType))
            {
                // find the user from his login
                Wt::Auth::User user = m_session.users().findWithIdentity(Wt::Auth::Identity::LoginName, login);

                if (user.isValid())
                {
                    if (!password.empty())
                    {
                        // ToDo: find problem cause : why rereadAll ??
                        m_session.rereadAll();
                        // verify
                        switch (m_session.passwordAuth().verifyPassword(user, password))
                        {
                            case Wt::Auth::PasswordValid:
                                m_session.login().login(user);
                                authentified = true;
                                Wt::log("info") << "[PUBLIC API] " << user.id() << " logged.";
                                break;
                            case Wt::Auth::LoginThrottling:
                                Wt::log("info") << "[PUBLIC API] too many attempts.";
                                break;
                            case Wt::Auth::PasswordInvalid:
                                Wt::log("info") << "[PUBLIC API] " << user.id() << " failure number: " << user.failedLoginAttempts();
                                break;
                            default:
                                break;
                        }
                    }
                    else if (!token.empty())
                    {
                        Wt::Dbo::ptr<Echoes::Dbo::User> userPtr = m_session.find<Echoes::Dbo::User>()
                                .where(QUOTE(TRIGRAM_USER SEP "MAIL") " = ?").bind(login)
                                .where(QUOTE(TRIGRAM_USER SEP "TOKEN") " = ?").bind(token)
                                .where(QUOTE(TRIGRAM_USER SEP "DELETE") " IS NULL")
                                .limit(1);
                        if (userPtr)
                        {
                            m_session.login().login(user);
                            authentified = true;
                        }
                        else
                        {
                            Wt::log("error") << "[PUBLIC API] Bad Token";
                        }
                    }
                    else
                    {
                        Wt::log("error") << "[Public API Resource] Password or token is empty";
                    }
                    
                    if (authentified)
                    {
                        orgId = m_session.user()->organization.id();
                    }
                }
                else
                {
                    Wt::log("info") << "[PUBLIC API] User invalid";
                }
            }
            else
            {
                Wt::log("error") << "[PUBLIC API] User " << login << " not found";
            }

            transaction.commit();
        }
        catch (Wt::Dbo::Exception const& e)
        {
            Wt::log("error") << "[PUBLIC API] User log: " << e.what();
        }
    }
    else if (!eno_token.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session, true);
            Wt::Dbo::ptr<Echoes::Dbo::Engine> enginePtr = m_session.find<Echoes::Dbo::Engine>()
                    .where(QUOTE(TRIGRAM_ENGINE SEP "FQDN") " = ?").bind(request.clientAddress())
                    .where(QUOTE(TRIGRAM_ENGINE SEP "DELETE") " IS NULL")
                    .limit(1);

            if (enginePtr)
            {
                Wt::Dbo::ptr<Echoes::Dbo::EngOrg> engOrgPtr = m_session.find<Echoes::Dbo::EngOrg>()
                        .where(QUOTE(TRIGRAM_ENGINE ID SEP TRIGRAM_ENGINE ID) " = ?").bind(enginePtr.id())
                        .where(QUOTE(TRIGRAM_ENG_ORG SEP "TOKEN") " = ?").bind(eno_token)
                        .where(QUOTE(TRIGRAM_ENG_ORG SEP "DELETE") " IS NULL")
                        .limit(1);
                if (engOrgPtr)
                {
                    authentified = true;
                    orgId = engOrgPtr->pk.organization.id();
                }
                else
                {
                    Wt::log("error") << "[PUBLIC API] Bad eno_token";
                }
            }
            else
            {
                Wt::log("error") << "[PUBLIC API] Engine with IP " << request.clientAddress() << " not found";
                notAllowed = true;
            }

            transaction.commit();
        }
        catch (Wt::Dbo::Exception const& e)
        {
            Wt::log("error") << "[PUBLIC API] Engine log: " << e.what();
        }
    }

    // set Content-Type
    response.setMimeType("application/json; charset=utf-8");

    if (authentified)
    {
        EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
        string responseMsg = "";

        if (request.method() == "GET")
            res = processRequest(request, orgId, responseMsg);
        else if (request.method() == "POST")
            res = processPostRequest(request, orgId, responseMsg);
        else if (request.method() == "PUT")
            res = processPutRequest(request, orgId, responseMsg);
        else if (request.method() == "DELETE")
            res = processDeleteRequest(request, orgId, responseMsg);
        else
        {
            res = EReturnCode::METHOD_NOT_ALLOWED;
            responseMsg = "{\n\t\"message\": \"Only GET, POST, PUT and DELETE methods are allowed.\n\"}";
        }   
        response.setStatus(res);
        response.out() << responseMsg;

        try
        {
            Wt::Dbo::Transaction transaction(m_session, true);
            Wt::log("info") << "[PUBLIC API] " << m_session.user().id() << " logged out.";
            m_session.login().logout();
            transaction.commit();
        }
        catch (Wt::Dbo::Exception const& e) 
        {
            Wt::log("error") << "[PUBLIC API] User logout" << e.what();
        }
    }
    else if (notAllowed)
    {
        response.setStatus(EReturnCode::BAD_REQUEST);
        const string err = "[Public API Resource] not allowed";
        response.out() << httpCodeToJSON(EReturnCode::BAD_REQUEST, err);
    }
    else
    {
        response.setStatus(EReturnCode::UNAUTHORIZED);
        response.out() << "{\n\t\"message\": \"Authentication failure\"\n}";
    }
}