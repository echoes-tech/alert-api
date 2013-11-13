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

PublicApiResource::PublicApiResource() : Wt::WResource(), m_session(), m_indexPathElement(1), m_statusCode(500)
{
}

PublicApiResource::~PublicApiResource()
{
    beingDeleted();
}

unsigned short PublicApiResource::retrieveCurrentHttpMethod(const string &method) const
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
    boost::split(m_pathElements, path, boost::is_any_of("/"), boost::token_compress_on);

    return;
}

void PublicApiResource::setRequestData(const Wt::Http::Request &request)
{
    m_requestData = request2string(request);
    return;
}

string PublicApiResource::getNextElementFromPath()
{
    string res = "";
    
    if (m_pathElements.size() > m_indexPathElement)
    {
        res =  m_pathElements[m_indexPathElement++];
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

    // WHY: Dans l'appli mobile lorsqu'on fait un post les premiers caractères de la requête sont remplacés par des caractères spéciaux. 
    // N'ayant pas su résoudre ce probléme, l'appli mobile envoie ses requêtes avec des caractères en trop au début du JSON. Il faut donc les supprimer.
    // On supprime donc tous les caractére avant "[" ou "{" suivant les cas pour éviter les problèmes de parse.
    const size_t pos1 = s.find("{", 0);
    const size_t pos2 = s.find("[", 0);
    if (pos1 != 0 || pos2 != 0)
    {
        if (pos2 != string::npos && pos2 < pos1)
        {
            s = s.erase(0, pos2);
        }
        else
        {
            s = s.erase(0, pos1);
        }
    }

    return s;
}

void PublicApiResource::processGetRequest(Wt::Http::Response &response)
{
    return;
}

void PublicApiResource::processPostRequest(Wt::Http::Response &response)
{
    return;
}

void PublicApiResource::processPutRequest(Wt::Http::Response &response)
{
    return;
}

void PublicApiResource::processDeleteRequest(Wt::Http::Response &response)
{
    return;
}

void PublicApiResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    Wt::log("info") << "[PUBLIC API] Identifying";

    // default : not authentified
    m_authentified = false;
    bool notAllowed = false;
    
    string login = "", password = "", token = "", eno_token = "";

    if (!request.getParameterValues("login").empty())
    {
        login = Wt::Utils::urlDecode(request.getParameterValues("login")[0]);
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

    m_session.initConnection(conf.getSessConnectParams());
    if (login.compare("") != 0)
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session);

            // check whether the user exists
            Wt::Dbo::ptr<AuthInfo::AuthIdentityType> authIdType = m_session.find<AuthInfo::AuthIdentityType > ().where("\"identity\" = ?").bind(login);
            if (Utils::checkId<AuthInfo::AuthIdentityType > (authIdType)) 
            {
                // find the user from his login
                Wt::Auth::User user = m_session.users().findWithIdentity(Wt::Auth::Identity::LoginName,login);

                if (user.isValid()) 
                {
                    if (password.compare(""))
                    {
                        
                        // ToDo: find problem cause : why rereadAll ??
                        m_session.rereadAll();
                        // verify
                        switch (m_session.passwordAuth().verifyPassword(user, password))
                        {
                            case Wt::Auth::PasswordValid:
                                m_session.login().login(user);
                                m_authentified = true;
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
                        Wt::Dbo::ptr<Echoes::Dbo::User> userPtr = m_session.find<Echoes::Dbo::User>()
                                .where("\"USR_MAIL\" = ?").bind(login)
                                .where("\"USR_TOKEN\" = ?").bind(token)
                                .limit(1);
                        if(userPtr)
                        {
                            m_session.login().login(user);
                            m_authentified = true;
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
            Wt::Dbo::Transaction transaction(m_session);
            Wt::Dbo::ptr<Echoes::Dbo::Engine> enginePtr = m_session.find<Echoes::Dbo::Engine>()
                    .where("\"ENG_FQDN\" = ?").bind(request.clientAddress())
                    .where("\"ENG_DELETE\" IS NULL")
                    .limit(1);

            if(enginePtr)
            {
                Wt::Dbo::ptr<Echoes::Dbo::EngOrg> engOrgPtr = m_session.find<Echoes::Dbo::EngOrg>()
                        .where("\"ENG_ID_ENG_ID\" = ?").bind(enginePtr.id())
                        .where("\"ENO_TOKEN\" = ?").bind(eno_token)
                        .where("\"ENO_DELETE\" IS NULL")
                        .limit(1);
                if(engOrgPtr)
                {
                    m_authentified = true;
                }
                else
                    Wt::log("error") << "[PUBLIC API] Bad eno_token";
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
            Wt::log("error") << "[PUBLIC API] " << e.what();
        }
    }
   
    // set Content-Type
    response.setMimeType("application/json; charset=utf-8");

    if (m_authentified) {
        setPathElementsVector(request.pathInfo());
        setRequestData(request);

        switch(retrieveCurrentHttpMethod(request.method()))
        {
            case Wt::Http::Get:
                processGetRequest(response);
                break;
            case Wt::Http::Post:
                
                processPostRequest(response);
                break;
            case Wt::Http::Put:
                processPutRequest(response);
                break;
            case Wt::Http::Delete:
                processDeleteRequest(response);
                break;
            default:
                response.setStatus(405);
                response.out() << "{\n\t\"message\": \"Only GET, POST, PUT and DELETE methods are allowed.\n\"}";
                break;
        }
    }
    else if (notAllowed)
    {
        response.setStatus(400);
        response.out() << "{\n\t\"message\": \"Bad Request\"\n}";
    }
    else
    {
        response.setStatus(401);
        response.out() << "{\n\t\"message\": \"Authentication failure\"\n}";
    }

    m_requestData = "";
    m_indexPathElement = 1;
    m_statusCode = 500;
}

