/**
 * @file PublicApiResource.h
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

#ifndef PUBLICAPIRESOURCE_H
#define	PUBLICAPIRESOURCE_H

#include <unistd.h>

#include <boost/algorithm/string.hpp>
#include <boost/exception/detail/exception_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/thread/recursive_mutex.hpp>

#include <Wt/Http/Client>
#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include <Wt/Json/Array>
#include <Wt/Json/Object>
#include <Wt/Dbo/FixedSqlConnectionPool>
#include <Wt/Json/Parser>
#include <Wt/Utils>
#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WLogger>
#include <Wt/WRandom>
#include <Wt/WResource>
#include <Wt/WServer>

namespace boost
{
  namespace property_tree
  {

    // id_translator template specialization to add quotes " " to strings in json
    template<>
    struct id_translator<std::string> {
        typedef std::string internal_type;
        typedef std::string external_type;

        boost::optional<std::string> get_value(const std::string &v) {
            return v;
        }

        boost::optional<std::string> put_value(const std::string &v) {
            return '"' + json_parser::create_escapes(v) + '"';
        }
    };

   
  }
}

#include <tools/Session.h>
#include <tools/MainIncludeFile.h>
#include <tools/Enums.h>

#include "JsonSerializer.h"
#include "Enums.h"
#include "Conf.h"
#include "Utils.h"

class PublicApiResource : public Wt::WResource {
public:
    PublicApiResource(Echoes::Dbo::Session& session);
    virtual ~PublicApiResource();
    
protected:
    Echoes::Dbo::Session& m_session;

    static std::string file2base64(const std::string &path);
    
    unsigned short retrieveCurrentHttpMethod(const std::string &method) const;
    std::string getNextElementFromPath(unsigned short &indexPathElement, std::vector<std::string> &pathElements);
    std::string request2string(const Wt::Http::Request &request);

    std::string processRequestParameters(const Wt::Http::Request &request, std::vector<std::string> &pathElements, std::map<std::string, long long> &parameters);
    virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);
    virtual EReturnCode processPostRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);
    virtual EReturnCode processPutRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);
    virtual EReturnCode processDeleteRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

    virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

    // a priori, useless
    template<class C>
    EReturnCode serialize(C &obj, std::string &responseMsg, EReturnCode successRes = EReturnCode::INTERNAL_SERVER_ERROR)
    {
        responseMsg = httpCodeToJSON(successRes,obj);
        return successRes;
    }
    template<class C>
    EReturnCode serialize(Wt::Dbo::ptr<C> &obj, std::string &responseMsg, EReturnCode successRes = EReturnCode::OK)
    {
        EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
        if(obj)
        {
            responseMsg += serializeToJSON(obj);
            res = successRes;
        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = httpCodeToJSON(res,obj);
        }
        return res;
    }
    template<class C>
    EReturnCode serialize(Wt::Dbo::collection<Wt::Dbo::ptr<C>> &obj, std::string &responseMsg, EReturnCode successRes = EReturnCode::OK)
    {
        EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
        if(obj.size() > 0)
        {
            responseMsg += serializeToJSON(obj);
            res = successRes;
        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = httpCodeToJSON(res,obj);
        }
        return res;
    }

    template<class C>
    std::string serializeToJSON(C &obj) {
        std::string responseMsg;
        Wt::Dbo::JsonSerializer jsonSerializer(m_session);
        jsonSerializer.serialize(obj);
        responseMsg = jsonSerializer.getResult();
        return responseMsg;
    }

    template<class C>
    std::string getTableName(C const &obj)
    {
        std::cout << "obj is: " << typeid(C).name() << '\n';
        return "";
    }
    template<class C>
    std::string getTableName(Wt::Dbo::ptr<C> const &ptr)
    {
        return Wt::Dbo::JsonSerializer::transformTableName(m_session.tableName<C>());
    }
    template<class C>
    std::string getTableName(Wt::Dbo::collection<Wt::Dbo::ptr<C>> const &ptrCol)
    {
        return Wt::Dbo::JsonSerializer::transformTableName(m_session.tableName<C>());
    }

    template<class C>
    std::string httpCodeToJSON(EReturnCode code, C &obj) {
        std::string message;
        std::string tableName = getTableName(obj);
        std::stringstream ss;
        boost::property_tree::ptree elem;

        switch (code) {
            case OK:
                message = "OK";
                break;
            case CREATED:
                message = tableName + " created";
                break;
            case ACCEPTED:
                message = "Accepted";
                break;
            case NO_CONTENT:
                message = "No Content";
                break;
            case RESET_CONTENT:
                message = "Reset Content";
                break;
            case PARTIAL_CONTENT:
                message = "Partial Content";
                break;
            case MULTIPLE_CHOICES:
                message = "Multiple Choices";
                break;
            case MOVED_PERMANENTLY:
                message = "Moved Permanently";
                break;
            case FOUND:
                message = "Found";
                break;
            case SEE_OTHER:
                message = "See Other";
                break;
            case NOT_MODIFIED:
                message = "Not Modified";
                break;
            case USE_PROXY:
                message = "Use Proxy";
                break;
            case TEMPORARY_REDIRECT:
                message = "Temporary Redirect";
                break;
            case TOO_MANY_REDIRECT:
                message = "Permanent Redirect";
                break;
            case BAD_REQUEST:
                message = "Bad Request";
                break;
            case UNAUTHORIZED:
                message = "Unauthorized";
                break;
            case PAYMENT_REQUIRED:
                message = "Payment Required";
                break;
            case FORBIDDEN:
                message = "Forbidden";
                break;
            case NOT_FOUND:
                message = tableName + " not found";
                break;
            case METHOD_NOT_ALLOWED:
                message = "Method Not Allowed";
                break;
            case NOT_ACCEPTABLE:
                message = "Not Acceptable";
                break;
            case PROXY_AUTHENTICATION_REQUIRED:
                message = "Proxy Authentication Required";
                break;
            case REQUEST_TIME_OUT:
                message = "Request Timeout";
                break;
            case CONFLICT:
                message = "Conflict";
                break;
            case GONE:
                message = "Gone";
                break;
            case LENGTH_REQUIRED:
                message = "Length Required";
                break;
            case PRECONDITION_FAILED:
                message = "Precondition Failed";
                break;
            case REQUEST_ENTITY_TOO_LARGE:
                message = "Request Entity Too Large";
                break;
            case REQUEST_URI_TOO_LONG:
                message = "Request-URI Too Long";
                break;
            case UNSUPPORTED_MEDIA_TYPE:
                message = "Unsupported Media Type";
                break;
            case REQUESTED_RANGE_UNSATISFIABLE:
                message = "Requested Range Not Satisfiable";
                break;
            case EXPECTATION_FAILED:
                message = "Expectation Failed";
                break;
            case I_AM_A_TEAPOT:
                message = "I'm a teapot (RFC 2324)";
                break;
            case UPGRADE_REQUIRED:
                message = "Upgrade Required";
                break;
            case INTERNAL_SERVER_ERROR:
                message = "Internal Server Error";
                break;
            case NOT_IMPLEMENTED:
                message = "Not Implemented";
                break;
            case BAD_GATEWAY_OR_PROXY_ERROR:
                message = "Bad Gateway";
                break;
            case SERVICE_UNAVAILABLE:
                message = "Service Unavailable";
                break;
            case GATEWAY_TIME_OUT:
                message = "Gateway Timeout";
                break;
            case HTTP_VERSION_NOT_SUPPORTED:
                message = "HTTP Version Not Supported";
                break;
            case BANDWIDTH_LIMIT_EXCEDED:
                message = "Bandwidth Limit Exceeded";
                break;
            default:
                break;
        }
        elem.put("message", message);
        boost::property_tree::json_parser::write_json(ss, elem);
        
        return ss.str();
    }
    
};

#endif	/* PUBLICAPIRESOURCE_H */
