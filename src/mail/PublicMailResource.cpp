/**
 * @file PublicMailResource.cpp
 * @author Thomas Saquet, Florent Poinsaut
 * @date 
 * @brief File containing example of doxygen usage for quick reference.
 *
 * Alert - API is a part of the Alert software
 * Copyright (C) 2013-2017
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 * 
 */

#include "mail/PublicMailResource.h"

using namespace std;

template<>
std::string PublicMailResource::getTableName<boost::bad_lexical_cast>(boost::bad_lexical_cast const& e)
{
    Wt::log("error") << e.what();
    return "";
}
template<>
std::string PublicMailResource::getTableName<Wt::Dbo::Exception>(Wt::Dbo::Exception const& e)
{
    Wt::log("error") << "Wt::Dbo: " << e.what();
    return "";
}
template<>
std::string PublicMailResource::getTableName<Wt::Json::ParseError>(Wt::Json::ParseError const& e)
{
    Wt::log("error") << "Wt::Json::Parse: " << e.what();
    return "";
}
template<>
std::string PublicMailResource::getTableName<Wt::Json::TypeException>(Wt::Json::TypeException const& e)
{
    Wt::log("error") << "Wt::Json::Type: " << e.what();
    return "";
}
template<>
std::string PublicMailResource::getTableName<string>(string const& string)
{
    Wt::log("error") << string;
    return "";
}

PublicMailResource::PublicMailResource(Echoes::Dbo::Session& session) : Wt::WResource(),
m_session(session)
{
}

PublicMailResource::~PublicMailResource()
{
    beingDeleted();
}

string PublicMailResource::file2base64(const string &path)
{         
    ifstream ifs(path);
    string content((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
    content = Wt::Utils::base64Encode(content);
    boost::erase_all(content, "\r\n");
    return content;
}

unsigned short PublicMailResource::retrieveCurrentHttpMethod(const string &method) const
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

string PublicMailResource::getNextElementFromPath(unsigned short &indexPathElement, vector<string> &pathElements)
{
    string res = "";

    if (pathElements.size() > indexPathElement)
    {
        res = pathElements[indexPathElement++];
    }

    return res;
}

string PublicMailResource::request2string(const Wt::Http::Request &request)
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

string PublicMailResource::processRequestParameters(const Wt::Http::Request &request, vector<string> &pathElements, map<string, string> &parameters)
{
    const string path = request.pathInfo();
    boost::split(pathElements, path, boost::is_any_of("/"), boost::token_compress_on);

    for (map<std::string, string>::iterator it = parameters.begin(); it != parameters.end(); ++it)
    {
        if (request.getParameter(it->first) != 0)
        {
            try
            {
                it->second = boost::lexical_cast<string>(*request.getParameter(it->first));
            }
            catch (boost::bad_lexical_cast const& e)
            {
                Wt::log("warning") << "[PUBLIC API] Bad URL parameter: " << e.what();
            }
        }
    }
    
    return request2string(request);
}

string PublicMailResource::processRequestParameters(const Wt::Http::Request &request, vector<string> &pathElements, map<string, long long> &parameters)
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

EReturnCode PublicMailResource::processGetRequest(const Wt::Http::Request &request, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::BAD_REQUEST;
    const string err = "[Public Api Resource] GET Method not implemented";
    responseMsg = "Une erreur est survenue dans la demande. Votre client mail semble incomptible. Essayez avec un autre client ou bien passez par l'interface classique";
    return res;
}

EReturnCode PublicMailResource::processPostRequest(const Wt::Http::Request &request, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::BAD_REQUEST;
    const string err = "[Public Api Resource] POST Method not implemented";
    responseMsg = httpCodeToJSON(res, err);
    return res;
}

EReturnCode PublicMailResource::processPutRequest(const Wt::Http::Request &request, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::BAD_REQUEST;
    const string err = "[Public Api Resource] PUT Method not implemented";
    responseMsg = httpCodeToJSON(res, err);
    return res;
}

EReturnCode PublicMailResource::processDeleteRequest(const Wt::Http::Request &request, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::BAD_REQUEST;
    const string err = "[Public Api Resource] DELETE Method not implemented";
    responseMsg = httpCodeToJSON(res, err);
    return res;
}

void PublicMailResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{

        EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
        string responseMsg = "";
        switch (retrieveCurrentHttpMethod(request.method()))
        {
            case Wt::Http::Get:
                res = processGetRequest(request, responseMsg);
                break;
            case Wt::Http::Post:
                res = processPostRequest(request, responseMsg);
                break;
            case Wt::Http::Put:
                res = processPutRequest(request, responseMsg);
                break;
            case Wt::Http::Delete:
                res = processDeleteRequest(request, responseMsg);
                break;
            default:
                res = EReturnCode::METHOD_NOT_ALLOWED;
                responseMsg = "{\n\t\"message\": \"Only GET, POST, PUT and DELETE methods are allowed.\n\"}";
                break;
        }

        response.setStatus(res);
        response.out() << responseMsg;

}
