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

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace boost
{
  namespace property_tree
  {

    // the write_json_helper template specialization to remove the quotes " " 
    // added to every json values, strings or not
    namespace json_parser
    {

      template<>
      void write_json_helper<ptree>(std::basic_ostream<typename ptree::key_type::value_type> &stream,
              const ptree &pt,
              int indent, bool pretty)
      {
          typedef typename ptree::key_type::value_type Ch;
          typedef typename std::basic_string<Ch> Str;

          // Value or object or array
          if (indent > 0 && pt.empty())
          {
              // Write value
              stream << pt.get_value<Str>();

          }
          else if (indent > 0 && pt.count(Str()) == pt.size())
          {
              // Write array
              stream << Ch('[');
              if (pretty) stream << Ch('\n');
              typename ptree::const_iterator it = pt.begin();
              for (; it != pt.end(); ++it)
              {
                  if (pretty) stream << Str(4 * (indent + 1), Ch(' '));
                  write_json_helper(stream, it->second, indent + 1, pretty);
                  if (boost::next(it) != pt.end())
                      stream << Ch(',');
                  if (pretty) stream << Ch('\n');
              }
              stream << Str(4 * indent, Ch(' ')) << Ch(']');

          }
          else
          {
              // Write object
              stream << Ch('{');
              if (pretty) stream << Ch('\n');
              typename ptree::const_iterator it = pt.begin();
              for (; it != pt.end(); ++it)
              {
                  if (pretty) stream << Str(4 * (indent + 1), Ch(' '));
                  stream << Ch('"') << create_escapes(it->first) << Ch('"') << Ch(':');
                  if (pretty)
                  {
                      if (it->second.empty())
                          stream << Ch(' ');
                      else
                          stream << Ch('\n') << Str(4 * (indent + 1), Ch(' '));
                  }
                  write_json_helper(stream, it->second, indent + 1, pretty);
                  if (boost::next(it) != pt.end())
                      stream << Ch(',');
                  if (pretty) stream << Ch('\n');
              }
              if (pretty) stream << Str(4 * indent, Ch(' '));
              stream << Ch('}');
          }
      }
    }
  }
}

#include "PublicApiResource.h"

using namespace std;

template<>
std::string PublicApiResource::getTableName<boost::bad_lexical_cast>(boost::bad_lexical_cast const& e)
{
    Wt::log("error") << e.what();
    return "";
}
template<>
std::string PublicApiResource::getTableName<Wt::Dbo::Exception>(Wt::Dbo::Exception const& e)
{
    Wt::log("error") << "Wt::Dbo: " << e.what();
    return "";
}
template<>
std::string PublicApiResource::getTableName<Wt::Json::ParseError>(Wt::Json::ParseError const& e)
{
    Wt::log("error") << "Wt::Json::Parse: " << e.what();
    return "";
}
template<>
std::string PublicApiResource::getTableName<Wt::Json::TypeException>(Wt::Json::TypeException const& e)
{
    Wt::log("error") << "Wt::Json::Type: " << e.what();
    return "";
}
template<>
std::string PublicApiResource::getTableName<string>(string const& string)
{
    Wt::log("error") << string;
    return "";
}

PublicApiResource::PublicApiResource(Echoes::Dbo::Session& session) : Wt::WResource(),
m_session(session)
{
}

PublicApiResource::~PublicApiResource()
{
    beingDeleted();
}

string PublicApiResource::file2base64(const string &path)
{         
    ifstream ifs(path);
    string content((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
    content = Wt::Utils::base64Encode(content);
    boost::erase_all(content, "\r\n");
    return content;
}

unsigned short PublicApiResource::retrieveCurrentHttpMethod(const string &method) const
{
    unsigned short res = 0;

    if (!method.compare("GET"))
    {
        res = Wt::Http::Get;
    }
    else if (!method.compare("POST"))
    {
        res = Wt::Http::Post;
    }
    else if (!method.compare("PUT"))
    {
        res = Wt::Http::Put;
    }
    else if (!method.compare("DELETE"))
    {
        res = Wt::Http::Delete;
    }

    return res;
}

string PublicApiResource::getNextElementFromPath(unsigned short &indexPathElement, vector<string> &pathElements)
{
    string res = "";

    if (pathElements.size() > indexPathElement)
    {
        res = pathElements[indexPathElement++];
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
        s.append(1, c);
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

string PublicApiResource::processRequestParameters(const Wt::Http::Request &request, vector<string> &pathElements, map<string, long long> &parameters)
{
    const string path = request.pathInfo();
    boost::split(pathElements, path, boost::is_any_of("/"), boost::token_compress_on);

    for (map<std::string, long long>::iterator it = parameters.begin(); it != parameters.end(); ++it)
    {
        if (request.getParameter(it->first) != 0)
        {
            try
            {
                it->second = boost::lexical_cast<unsigned long long>(*request.getParameter(it->first));
            }
            catch (boost::bad_lexical_cast const& e)
            {
                Wt::log("warning") << "[PUBLIC API] Bad URL parameter: " << e.what();
            }
        }
    }
    
    return request2string(request);
}

EReturnCode PublicApiResource::processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::BAD_REQUEST;
    const string err = "[Public Api Resource] GET Method not implemented";
    responseMsg = httpCodeToJSON(res, err);
    return res;
}

EReturnCode PublicApiResource::processPostRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::BAD_REQUEST;
    const string err = "[Public Api Resource] POST Method not implemented";
    responseMsg = httpCodeToJSON(res, err);
    return res;
}

EReturnCode PublicApiResource::processPutRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::BAD_REQUEST;
    const string err = "[Public Api Resource] PUT Method not implemented";
    responseMsg = httpCodeToJSON(res, err);
    return res;
}

EReturnCode PublicApiResource::processDeleteRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::BAD_REQUEST;
    const string err = "[Public Api Resource] DELETE Method not implemented";
    responseMsg = httpCodeToJSON(res, err);
    return res;
}

void PublicApiResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    Wt::log("info") << "[PUBLIC API] Identifying";

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

        switch (retrieveCurrentHttpMethod(request.method()))
        {
            case Wt::Http::Get:
                res = processGetRequest(request, orgId, responseMsg);
                break;
            case Wt::Http::Post:
                res = processPostRequest(request, orgId, responseMsg);
                break;
            case Wt::Http::Put:
                res = processPutRequest(request, orgId, responseMsg);
                break;
            case Wt::Http::Delete:
                res = processDeleteRequest(request, orgId, responseMsg);
                break;
            default:
                res = EReturnCode::METHOD_NOT_ALLOWED;
                responseMsg = "{\n\t\"message\": \"Only GET, POST, PUT and DELETE methods are allowed.\n\"}";
                break;
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

