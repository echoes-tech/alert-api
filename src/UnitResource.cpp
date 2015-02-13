/* 
 * API UnitResource
 * @author ECHOES Technologies (GDR)
 * @date 21/02/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "UnitResource.h"

using namespace std;

UnitResource::UnitResource(Echoes::Dbo::Session& session) : PublicApiResource::PublicApiResource(session)
{
    Call<UnitResource> tmp;
    tmp.method = "GET";
    tmp.path = "";
    tmp.function = &UnitResource::getUnitsList;
    calls.push_back(tmp);
    tmp.method = "GET";
    tmp.path = "/[0-9]+";
    tmp.function = &UnitResource::getUnit;
    calls.push_back(tmp);
    tmp.method = "GET";
    tmp.path = "/(\\D)*";
    tmp.function = &UnitResource::Error;
    calls.push_back(tmp);
}

UnitResource::~UnitResource()
{
}

EReturnCode UnitResource::getUnitsList(const std::vector<std::string> &pathElements, const long long &orgId, string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationUnit>> inuPtrCol = m_session.find<Echoes::Dbo::InformationUnit>()
                .where(QUOTE(TRIGRAM_INFORMATION_UNIT SEP "DELETE") " IS NULL")
                .orderBy(QUOTE(TRIGRAM_INFORMATION_UNIT ID));

        res = serialize(inuPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode UnitResource::getUnit(const std::vector<std::string> &pathElements, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::InformationUnit> inuPtr = m_session.find<Echoes::Dbo::InformationUnit>()
                .where(QUOTE(TRIGRAM_INFORMATION_UNIT ID) " = ?").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_INFORMATION_UNIT SEP "DELETE") " IS NULL");

        res = serialize(inuPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode UnitResource::Error(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    
    res = EReturnCode::BAD_REQUEST;
    const string err = "[Unit Resource] bad nextElement";
    responseMsg = httpCodeToJSON(res, err);
    return res;
}

EReturnCode UnitResource::processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        res = getUnitsList(pathElements, orgId, responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.empty())
            {
                res = getUnit(pathElements, orgId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Unit Resource] bad nextElement";
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

EReturnCode UnitResource::postUnit(const string& sRequest, const long long &orgId, string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    long long iutId;
    Wt::WString name;
    long long baseInuId = 0;
    Wt::WString baseOperation;

    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            name = result.get("name");
            iutId = result.get("type_id");
            if (result.contains("base_inu_id") && result.contains("base_operation"))
            {
                baseInuId = result.get("base_inu_id");
                baseOperation = result.get("base_operation");
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
        const string err = "[Unit Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session, true);

            Wt::Dbo::ptr<Echoes::Dbo::InformationUnitType> iutPtr = m_session.find<Echoes::Dbo::InformationUnitType>()
                    .where(QUOTE(TRIGRAM_INFORMATION_UNIT_TYPE ID) " = ?").bind(iutId)
                    .where(QUOTE(TRIGRAM_INFORMATION_UNIT_TYPE SEP "DELETE") " IS NULL");
            if (!iutPtr)
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, iutPtr);
                return res;
            }

            Wt::Dbo::ptr<Echoes::Dbo::InformationUnit> baseInuPtr;
            if (baseInuId > 0)
            {
                baseInuPtr = m_session.find<Echoes::Dbo::InformationUnit>()
                        .where(QUOTE(TRIGRAM_INFORMATION_UNIT ID) " = ?").bind(baseInuId)
                        .where(QUOTE(TRIGRAM_INFORMATION_UNIT SEP "DELETE") " IS NULL");
                if (!baseInuPtr)
                {
                    res = EReturnCode::NOT_FOUND;
                    responseMsg = httpCodeToJSON(res, baseInuPtr);
                    return res;
                }
            }

            Echoes::Dbo::InformationUnit *newInu = new Echoes::Dbo::InformationUnit();
            newInu->name = name;
            newInu->unitType = iutPtr;
            if (baseInuId > 0)
            {
                 newInu->informationUnitBelongTo = baseInuPtr;
            }
            newInu->baseOperation = baseOperation;

            Wt::Dbo::ptr<Echoes::Dbo::InformationUnit> newInuPtr = m_session.add<Echoes::Dbo::InformationUnit>(newInu);
            newInuPtr.flush();

            res = serialize(newInuPtr, responseMsg, EReturnCode::CREATED);

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

EReturnCode UnitResource::processPostRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        res = postUnit(sRequest, orgId, responseMsg);
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Unit Resource] bad nextElement";
        responseMsg = httpCodeToJSON(res, err);
    }

    return res;
}

EReturnCode UnitResource::putUnit(const std::vector<std::string> &pathElements, const string &sRequest, const long long &orgId, string &responseMsg)
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
        const string err = "[Unit Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session, true);

            Wt::Dbo::ptr<Echoes::Dbo::InformationUnit> inuPtr = m_session.find<Echoes::Dbo::InformationUnit>()
                    .where(QUOTE(TRIGRAM_INFORMATION_UNIT ID) " = ?").bind(pathElements[1])
                    .where(QUOTE(TRIGRAM_INFORMATION_UNIT SEP "DELETE") " IS NULL");

            if (inuPtr)
            {
                if (!name.empty())
                {
                    inuPtr.modify()->name = name;
                }

                res = serialize(inuPtr, responseMsg);
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

EReturnCode UnitResource::processPutRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        const string err = "[Unit Resource] bad nextElement";
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
                res = putUnit(pathElements, sRequest, orgId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Unit Resource] bad nextElement";
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

EReturnCode UnitResource::deleteUnit(const std::vector<std::string> &pathElements, const long long &orgId, string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try 
    {  
        Wt::Dbo::Transaction transaction(m_session, true);
           
        Wt::Dbo::ptr<Echoes::Dbo::InformationUnit> inuPtr = m_session.find<Echoes::Dbo::InformationUnit>()
                .where(QUOTE(TRIGRAM_INFORMATION_UNIT ID) " = ?").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_INFORMATION_UNIT SEP "DELETE") " IS NULL");

        if(inuPtr)
        {
            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Information>> infPtrCol = m_session.find<Echoes::Dbo::Information>()
                    .where(QUOTE(TRIGRAM_INFORMATION SEP TRIGRAM_INFORMATION_UNIT SEP TRIGRAM_INFORMATION_UNIT ID)" = ?").bind(pathElements[1])
                    .where(QUOTE(TRIGRAM_INFORMATION SEP "DELETE") " IS NULL");

            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationData>> idaPtrCol = m_session.find<Echoes::Dbo::InformationData>()
                    .where(QUOTE(TRIGRAM_INFORMATION_DATA SEP TRIGRAM_INFORMATION_UNIT SEP TRIGRAM_INFORMATION_UNIT ID)" = ?").bind(pathElements[1])
                    .where(QUOTE(TRIGRAM_INFORMATION_DATA SEP "DELETE") " IS NULL");

            if (infPtrCol.size() == 0 && idaPtrCol.size() == 0)
            {
                inuPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
                res = EReturnCode::NO_CONTENT;
            }
            else
            {
                res = EReturnCode::CONFLICT;
                responseMsg = httpCodeToJSON(res, inuPtr);
            } 
        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = httpCodeToJSON(res, inuPtr);
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

EReturnCode UnitResource::processDeleteRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        const string err = "[Unit Resource] bad nextElement";
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
                res = deleteUnit(pathElements, orgId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Unit Resource] bad nextElement";
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

EReturnCode UnitResource::processRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    vector<Call<UnitResource>>::iterator it = calls.begin();
    vector<string> pathElements;
    
    cout << "step 2 : " << request.pathInfo()<< endl;
    
    boost::split(pathElements, (const string)request.pathInfo(), boost::is_any_of("/"), boost::token_compress_on);
    while(!(!request.method().compare(it.base()->method)
            && boost::regex_match(request.pathInfo(), it.base()->path) )
            && (it != calls.end()))
    {
        it++;
    }
    cout << "step 3" << endl;
    if (it != calls.end())
    {
        cout << "step 4 : " << it.base()->path << endl;
        EReturnCode (UnitResource::*tmp_fct)(const std::vector<std::string> &, const long long &, string &) = it.base()->function;
        res = (this->*tmp_fct)(pathElements, orgId, responseMsg);
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        responseMsg = httpCodeToJSON(res, (const string)("[Unit Resource] bad nextElement"));
    }
    return res;
}

void UnitResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    Wt::log("info") << "[Unit API] Identifying";

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
        cout << "step 1 : " << request.pathInfo() << endl;
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