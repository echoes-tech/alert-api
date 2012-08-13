/* 
 * File:   includeFile.h
 * Author: tsa
 *
 * Created on 8 août 2012, 14:43
 */

#ifndef INCLUDEFILE_H
#define	INCLUDEFILE_H

#include <Wt/WServer>
#include <Wt/Http/Client>
#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include <Wt/WResource>
#include <Wt/WLogger>
#include <Wt/Utils>
#include <Wt/WEnvironment>
//#include <EchoesHome.h>

#include <tools/Session.h>
#include <tools/MainIncludeFile.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <unistd.h>

#include "Utils.h"

#include "PublicApiResource.h"
#include "SendSMS.h"
#include "SrReception.h"

typedef std::vector<std::string> ParameterValues;
typedef std::map<std::string, ParameterValues> ParameterMap;

#endif	/* INCLUDEFILE_H */

