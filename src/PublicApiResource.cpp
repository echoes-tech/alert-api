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

PublicApiResource::PublicApiResource() : Wt::WResource(),
m_session(),
m_indexPathElement(1),
m_statusCode(EReturnCode::INTERNAL_SERVER_ERROR)
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

void PublicApiResource::setParameters(const Wt::Http::Request &request)
{
    for (map<std::string, long long>::iterator it = m_parameters.begin(); it != m_parameters.end(); ++it)
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

    return;
}

string PublicApiResource::getNextElementFromPath()
{
    string res = "";

    if (m_pathElements.size() > m_indexPathElement)
    {
        res = m_pathElements[m_indexPathElement++];
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

void PublicApiResource::resetAttributs()
{
    m_requestData = "";
    m_indexPathElement = 1;
    m_statusCode = EReturnCode::INTERNAL_SERVER_ERROR;

    for (map<std::string, long long>::iterator it = m_parameters.begin(); it != m_parameters.end(); ++it)
    {
        it->second = 0;
    }

    return;
}

void PublicApiResource::processGetRequest(Wt::Http::Response &response)
{
    response.setStatus(EReturnCode::BAD_REQUEST);
    response.out() << "{\n\t\"message\":\"Bad Request\"\n}";
    return;
}

void PublicApiResource::processPostRequest(Wt::Http::Response &response)
{
    response.setStatus(EReturnCode::BAD_REQUEST);
    response.out() << "{\n\t\"message\":\"Bad Request\"\n}";
    return;
}

void PublicApiResource::processPutRequest(Wt::Http::Response &response)
{
    response.setStatus(EReturnCode::BAD_REQUEST);
    response.out() << "{\n\t\"message\":\"Bad Request\"\n}";
    return;
}

void PublicApiResource::processDeleteRequest(Wt::Http::Response &response)
{
    response.setStatus(EReturnCode::BAD_REQUEST);
    response.out() << "{\n\t\"message\":\"Bad Request\"\n}";
    return;
}

void PublicApiResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    Wt::log("info") << "[PUBLIC API] Identifying";

    // default : not authentified
    m_authentified = false;
    bool notAllowed = false;

    string login = "";
    string password = "";
    string token = "";
    string eno_token = "";

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

    m_session.initConnection(conf.getSessConnectParams());
    if (!login.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session);

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
                            m_authentified = true;
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
            Wt::log("error") << "[PUBLIC API] " << e.what();
        }
    }
    else if (!eno_token.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session);
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
                    m_authentified = true;
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
            Wt::log("error") << "[PUBLIC API] " << e.what();
        }
    }

    // set Content-Type
    response.setMimeType("application/json; charset=utf-8");

    if (m_authentified)
    {
        setPathElementsVector(request.pathInfo());
        setParameters(request);
        setRequestData(request);

        switch (retrieveCurrentHttpMethod(request.method()))
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
            response.setStatus(EReturnCode::METHOD_NOT_ALLOWED);
            response.out() << "{\n\t\"message\": \"Only GET, POST, PUT and DELETE methods are allowed.\n\"}";
            break;
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

    resetAttributs();
}

