/* 
 * Header of API Resource
 * @author ECHOES Technologies (TSA)
 * @date 08/08/2012
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2012 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef PUBLICAPIRESOURCE_H
#define	PUBLICAPIRESOURCE_H

#include <unistd.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>

#include <Wt/Http/Client>
#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include <Wt/WResource>
#include <Wt/WLogger>
#include <Wt/Utils>
#include <Wt/WEnvironment>
#include <Wt/Json/Object>
#include <Wt/Json/Parser>

#include <tools/Session.h>
#include <tools/MainIncludeFile.h>

#include "Utils.h"

class PublicApiResource : public Wt::WResource {
    public:
        PublicApiResource();
        virtual ~PublicApiResource();

    protected:
        std::string login;
        Wt::WString password;
        bool authentified;
        Session *session;
        std::vector<std::string> vPathElements;
        unsigned short indexPathElement, statusCode;

        unsigned short retrieveCurrentHttpMethod(std::string method);
        int releaseSplit(Wt::Http::Response &response, std::string distribRelease);
        unsigned int getIdFromRequest(const Wt::Http::Request &request);
        void setPathElementsVector(std::string path);
        std::string getNextElementFromPath();

        std::string request2string(Wt::Http::Request r);

        virtual void processGetRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        virtual void processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        virtual void processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        virtual void processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        virtual void processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        
        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
};


#endif	/* PUBLICAPIRESOURCE_H */

