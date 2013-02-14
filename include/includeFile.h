/* 
 * File:   includeFile.h
 * Author: tsa
 *
 * Created on 8 août 2012, 14:43
 */

#ifndef INCLUDEFILE_H
#define	INCLUDEFILE_H

#include <unistd.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>

#include <Wt/WServer>
#include <Wt/Http/Client>
#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include <Wt/WResource>
#include <Wt/WLogger>
#include <Wt/Utils>
#include <Wt/WEnvironment>
#include <Wt/Json/Object>
#include <Wt/Json/Parser>
//#include <EchoesHome.h>

#include <tools/Session.h>
#include <tools/MainIncludeFile.h>

#include "Utils.h"

#include "Conf.h"

#include "PublicApiResource.h"

#include "InformationResource.h"
#include "MediaResource.h"
#include "PluginResource.h"
#include "UserResource.h"
#include "AlertResource.h"
#include "AssetResource.h"
#include "ProbeResource.h"

#include "itooki/ItookiSMSSender.h"
#include "ItookiAckReceiver.h"
#include "ItookiAswReceiver.h"
//
//#include "SendSMS.h"
//#include "SrReception.h"

#include "PublicApiResource.h"

typedef std::vector<std::string> ParameterValues;
typedef std::map<std::string, ParameterValues> ParameterMap;

#endif	/* INCLUDEFILE_H */

