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

#include <boost/algorithm/string.hpp>
#include <boost/exception/detail/exception_ptr.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <Wt/Http/Client>
#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include <Wt/Json/Array>
#include <Wt/Json/Object>
#include <Wt/Json/Parser>
#include <Wt/Utils>
#include <Wt/WApplication>
#include <Wt/WEnvironment>
#include <Wt/WLogger>
#include <Wt/WResource>
#include <Wt/WServer>

#include <tools/Session.h>
#include <tools/MainIncludeFile.h>

#include "Conf.h"
#include "Utils.h"

class PublicApiResource : public Wt::WResource {
    public:
        PublicApiResource();
        PublicApiResource(const PublicApiResource& orig);
        virtual ~PublicApiResource();

    protected:
        Session *session;
        bool authentified;
        std::vector<std::string> vPathElements;
        unsigned short indexPathElement, statusCode;

        unsigned short retrieveCurrentHttpMethod(const std::string &method);

        void setPathElementsVector(const std::string &path);
        std::string getNextElementFromPath();

        std::string request2string(const Wt::Http::Request &request);

        virtual void processGetRequest(Wt::Http::Response &response);
        virtual void processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        virtual void processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        virtual void processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        
        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
};


#endif	/* PUBLICAPIRESOURCE_H */

