/* 
 * API Ressource
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

PublicApiResource::~PublicApiResource() {
    beingDeleted();
}

unsigned short PublicApiResource::retrieveCurrentHttpMethod(string method)
{
    unsigned short res;
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
    else if(!method.compare("PATCH"))
    {
        res = 4;
    }
    else if(!method.compare("DELETE"))
    {
        res = 5;
    }
    else
    {
        res = 0;
    }

    return res;
}

int PublicApiResource::releaseSplit(Wt::Http::Response &response, string distribRelease)
{
    int release;
    vector< string > splitRelease;

    boost::split(splitRelease, distribRelease, boost::is_any_of("."), boost::token_compress_on);

    // Convert 1st release element to int
    try
    {
        release = boost::lexical_cast<int>(splitRelease[0]);
    }
    catch(boost::bad_lexical_cast &)
    {
        response.setStatus(422);
        response.out() << "{\"message\":\"Validation Failed\"}";
        return -1;
    }

    return release;
}

unsigned int PublicApiResource::getIdFromRequest(const Wt::Http::Request &request)
{
    // URL path after /resources
    string path = request.pathInfo();

    vector< string > splitPath;
    boost::split(splitPath, path, boost::is_any_of("/"), boost::token_compress_on);

    // Convert 2nd path element to int
    try
    {
        return boost::lexical_cast<int>(splitPath[1]);
    }
    catch(boost::bad_lexical_cast &)
    {
        return 0;
    }
}

void PublicApiResource::setPathElementsVector(std::string path)
{
    boost::split(this->vPathElements, path, boost::is_any_of("/"), boost::token_compress_on);
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

string PublicApiResource::request2string(Wt::Http::Request r) {
    char c;
    std::string s;

    // Getting the input stream for the request char by char
    c = r.in().get();
    while (r.in())
    {
        s.append(1,c);
        c = r.in().get();
    }
    return s;
}

void PublicApiResource::processGetRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
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

void PublicApiResource::processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}

void PublicApiResource::processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}

void PublicApiResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response) {
    Wt::log("info") << "[PUBLIC API] Identifying";
    // Setting the session
    session = new Session(Utils::connection);
    Session::configureAuth();

    try
    {
        session->createTables();
        std::cerr << "Created database." << std::endl;
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "Using existing database";
    }

    // default : not authentified
    this->authentified = false;

    this->login = "";
    this->password = "";
    if (!request.getParameterValues("login").empty()) {
        this->login = request.getParameterValues("login")[0];
    }
    if (!request.getParameterValues("password").empty()) {
        this->password = request.getParameterValues("password")[0];
    }

    const Wt::WString pass = this->password;

    // transaction
    {
        try {
            Wt::Dbo::Transaction transaction(*session);

            // check whether the user exists
            Wt::Dbo::ptr<AuthInfo::AuthIdentityType> authIdType = session->find<AuthInfo::AuthIdentityType > ().where("\"identity\" = ?").bind(this->login);
            if (Utils::checkId<AuthInfo::AuthIdentityType > (authIdType)) 
            {
                // find the user from his login
                Wt::Auth::User user = session->users().findWithIdentity(Wt::Auth::Identity::LoginName,this->login);

                if (!user.isValid()) 
                {
                    Wt::log("info") << "[PUBLIC API] User invalid";
                    return;
                }

                // verify
                switch (session->passwordAuth().verifyPassword(user, pass))
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
            } else 
            {
                Wt::log("error") << "[PUBLIC API] User " << this->login << " not found";
            }
        } catch (Wt::Dbo::Exception const& e) 
        {
            Wt::log("error") << "[PUBLIC API] " << e.what();
        }
    }

    // set Content-Type
    response.setMimeType("application/json; charset=utf-8");

    if (!this->authentified) {
        response.setStatus(401);
        response.out() << "{\n\t\"message\": \"Authentification Failed\"\n}";
        return;
    }

    setPathElementsVector(request.pathInfo());

    switch(retrieveCurrentHttpMethod(request.method()))
    {
        case Wt::Http::Get:
            processGetRequest(request, response);
            break;
        case Wt::Http::Post:
            processPostRequest(request, response);
            break;
        case Wt::Http::Put:
            processPutRequest(request, response);
            break;
        case 4:
            processPatchRequest(request, response);
            break;
        case 5:
            processDeleteRequest(request, response);
            break;
        default:
            response.setStatus(405);
            response.out() << "{\n\t\"message\": \"Only GET, POST, PUT, PATCH and DELETE methods are allowed.\n\"}";
            return;
            break;
    }
    
    this->indexPathElement = 1;
    this->statusCode = 500;
}
        
