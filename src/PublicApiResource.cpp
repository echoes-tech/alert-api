/* 
 * API Resource
 * @author ECHOES Technologies (TSA)
 * @date 10/02/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "PublicApiResource.h"

using namespace std;

PublicApiResource::PublicApiResource() : Wt::WResource(){
    this->indexPathElement = 1;
    this->statusCode = 500;
}

PublicApiResource::PublicApiResource(const PublicApiResource& orig) : Wt::WResource()
{
    this->session = orig.session;
    this->authentified = orig.authentified;
    this->statusCode = orig.statusCode;
    this->indexPathElement = orig.indexPathElement;
    this->vPathElements =  orig.vPathElements;
}

PublicApiResource::~PublicApiResource() {
    beingDeleted();
}

unsigned short PublicApiResource::retrieveCurrentHttpMethod(const string &method)
{
    unsigned short res = 0;

    if(!method.compare("GET"))
    {
        res = Wt::Http::Get;
    }
    else if(!method.compare("POST"))
    {
        res = Wt::Http::Post;
    }
    else if(!method.compare("PUT"))
    {
        res = Wt::Http::Put;
    }
    else if(!method.compare("DELETE"))
    {
        res = Wt::Http::Delete;
    }

    return res;
}

void PublicApiResource::setPathElementsVector(const string &path)
{
    boost::split(this->vPathElements, path, boost::is_any_of("/"), boost::token_compress_on);

    return;
}

string PublicApiResource::getNextElementFromPath()
{
    string res = "";
    
    if (this->vPathElements.size() > this->indexPathElement)
    {
        res =  vPathElements[this->indexPathElement++];
    }
    
    return res;
}

string PublicApiResource::request2string(const Wt::Http::Request &request)
{
    char c;
    string s;

    // Getting the input stream for the request char by char
    c = request.in().get();
    while (request.in())
    {
        s.append(1,c);
        c = request.in().get();
    }
    return s;
}

void PublicApiResource::processGetRequest(Wt::Http::Response &response)
{
    return;
}

void PublicApiResource::processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}

void PublicApiResource::processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}

void PublicApiResource::processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}

void PublicApiResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    Wt::log("info") << "[PUBLIC API] Identifying";
    // Setting the session
    session = new Session(Utils::connection);
    Session::configureAuth();

    // default : not authentified
    this->authentified = false;
    
    string login = "", password = "", token = "", eno_token = "";

    if (!request.getParameterValues("login").empty())
    {
        login = request.getParameterValues("login")[0];
        if (login.compare(""))
        {
            if (!request.getParameterValues("password").empty())
                password = request.getParameterValues("password")[0];
            else if (!request.getParameterValues("token").empty())
                token = request.getParameterValues("token")[0];
            else
                Wt::log("error") << "[Public API Resource] No password or token parameter";
        }
        else
            Wt::log("error") << "[Public API Resource] login is empty";
    }
    else if(!request.getParameterValues("eno_token").empty())
    {
        eno_token = request.getParameterValues("eno_token")[0];
        if (!eno_token.compare(""))
            Wt::log("error") << "[Public API Resource] org_token is empty";
    }
    else
        Wt::log("error") << "[Public API Resource] No login or eno_token parameter";

    if (login.compare("") != 0)
    {
        try
        {
            Wt::Dbo::Transaction transaction(*session);

            // check whether the user exists
            Wt::Dbo::ptr<AuthInfo::AuthIdentityType> authIdType = session->find<AuthInfo::AuthIdentityType > ().where("\"identity\" = ?").bind(login);
            if (Utils::checkId<AuthInfo::AuthIdentityType > (authIdType)) 
            {
                // find the user from his login
                Wt::Auth::User user = session->users().findWithIdentity(Wt::Auth::Identity::LoginName,login);

                if (user.isValid()) 
                {
                    if (password.compare(""))
                    {
                        // verify
                        switch (session->passwordAuth().verifyPassword(user, password))
                        {
                            case Wt::Auth::PasswordValid:
                                session->login().login(user);
                                this->authentified = true;
                                Wt::log("info") << "[PUBLIC API] " << user.id() << " logged.";
                                break;
                            case Wt::Auth::LoginThrottling:
                                Wt::log("info") << "[PUBLIC API] too many attempts.";
                                break;
                            case Wt::Auth::PasswordInvalid:
                                Wt::log("info") << "[PUBLIC API] " << user.id() << " failure number : " << user.failedLoginAttempts();
                                break;
                            default:
                                break;
                        }
                    }
                    else if (token.compare(""))
                    {
                        string queryStr = "SELECT \"expires\" FROM \"auth_token\" "
                                                "WHERE \"value\" = '" + boost::lexical_cast<string>(token) +
                                                "' and \"auth_info_id\" IN "
                                                        "(SELECT \"auth_info_id\" FROM \"auth_identity\" "
                                                        "WHERE \"identity\" = '" + boost::lexical_cast<string>(login) +
                                                        "')";

                        Wt::WDateTime dateTime = session->query<Wt::WDateTime>(queryStr);

                        if(!dateTime.isNull())
                        {
                            if (dateTime > Wt::WDateTime::currentDateTime())
                            {
                                session->login().login(user);
                                this->authentified = true;
                            }
                            else
                                Wt::log("error") << "[PUBLIC API] This token is too old";
                        }
                        else
                            Wt::log("error") << "[PUBLIC API] Bad Token";
                    }
                    else
                        Wt::log("error") << "[Public API Resource] Password or token is empty";
                }
                else
                    Wt::log("info") << "[PUBLIC API] User invalid";
            }
            else 
                Wt::log("error") << "[PUBLIC API] User " << login << " not found";

            transaction.commit();
        }
        catch (Wt::Dbo::Exception const& e) 
        {
            Wt::log("error") << "[PUBLIC API] " << e.what();
        }
    }
    else if (eno_token.compare("") != 0)
    {
        try
        {
            Wt::Dbo::Transaction transaction(*session);
            Wt::Dbo::ptr<Engine> enginePtr = session->find<Engine>()
                    .where("\"ENG_FQDN\" = ?").bind(request.clientAddress())
                    .where("\"ENG_DELETE\" IS NULL")
                    .limit(1);

            if(enginePtr)
            {
                Wt::Dbo::ptr<EngOrg> engOrgPtr = session->find<EngOrg>()
                        .where("\"ENG_ID_ENG_ID\" = ?").bind(enginePtr.id())
                        .where("\"ENO_token\" = ?").where(eno_token)
                        .where("\"ENO_DELETE\" IS NULL")
                        .limit(1);
                if(engOrgPtr)
                {
                    this->authentified = true;
                }
                else
                    Wt::log("error") << "[PUBLIC API] Bad eno_token";
            }
            else
                Wt::log("error") << "[PUBLIC API] Engine with IP " << request.clientAddress() << " not found";

            transaction.commit();
        }
        catch (Wt::Dbo::Exception const& e) 
        {
            Wt::log("error") << "[PUBLIC API] " << e.what();
        }
    }
   
    // set Content-Type
    response.setMimeType("application/json; charset=utf-8");

    if (this->authentified) {
        setPathElementsVector(request.pathInfo());

        switch(retrieveCurrentHttpMethod(request.method()))
        {
            case Wt::Http::Get:
                processGetRequest(response);
                break;
            case Wt::Http::Post:
                processPostRequest(request, response);
                break;
            case Wt::Http::Put:
                processPutRequest(request, response);
                break;
            case Wt::Http::Delete:
                processDeleteRequest(request, response);
                break;
            default:
                response.setStatus(405);
                response.out() << "{\n\t\"message\": \"Only GET, POST, PUT and DELETE methods are allowed.\n\"}";
                break;
        }
    }
    else
    {
        response.setStatus(401);
        response.out() << "{\n\t\"message\": \"Authentication failure\"\n}";
    }

    this->indexPathElement = 1;
    this->statusCode = 500;
    Wt::log("debug") << "[PUBLIC API] Session delete";
    delete session;
}
        
