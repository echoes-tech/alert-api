/* 
 * File:   ItookiSendedReceiver.h
 * Author: cde
 *
 * Created on 10 juillet 2015, 10:35
 */

#ifndef ITOOKISENDEDRECEIVER_H
#define	ITOOKISENDEDRECEIVER_H

#include <Wt/Json/Parser>
#include <Wt/Json/Object>

//#include <boost/property_tree/ptree.hpp>
//#include <boost/property_tree/json_parser.hpp>

#include <tools/Session.h>
#include <tools/MainIncludeFile.h>
#include <tools/Enums.h>

#include <Wt/WResource>
#include <Wt/Http/Response>
#include "Conf.h"
#include "Enums.h"


class ItookiSendedReceiver   : public Wt::WResource
{
    public:
        ItookiSendedReceiver(Echoes::Dbo::Session& session);
        virtual ~ItookiSendedReceiver();

    protected:
        Echoes::Dbo::Session& m_session;

        std::string m_number;
        std::string m_message;
        
        EReturnCode postSended(map<string, long long> parameters, const vector<string> &pathElements, const string &sRequest, string &responseMsg);
        string getNextElementFromPath(unsigned short &indexPathElement, vector<string> &pathElements);
        string processRequestParameters(const Wt::Http::Request &request, vector<string> &pathElements, map<string, long long> &parameters);
        string request2string(const Wt::Http::Request &request);
        EReturnCode processPostRequest(const Wt::Http::Request &request, std::string &responseMsg);
        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        template<class C>
    std::string httpCodeToJSON(EReturnCode code, C &obj) {
        std::string message;
        //std::string tableName = getTableName(obj);
        std::stringstream ss;
        boost::property_tree::ptree elem;

        switch (code) {
            case OK:
                message = "OK";
                break;
            case CREATED:
                message = "table created";
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
                message = "table not found";
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

#endif	/* ITOOKISENDEDRECEIVER_H */

