/* 
 * API Configuration
 * @author ECHOES Technologies (FPO)
 * @date 05/11/2012
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2012 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "includeFile.h"

Conf::Conf()
{
    // Create an empty property tree object
    using boost::property_tree::ptree;
    ptree pt;
    
    // Load the INI file into the property tree. If reading fails
    // (cannot open file, parse error), an exception is thrown.
    try
    {
        boost::property_tree::read_ini("api.conf", pt);
        dBhost = pt.get<std::string>("database.host");
        dBport = pt.get<int>("database.port");
        dBname = pt.get<std::string>("database.name");
        dBuser = pt.get<std::string>("database.user");
        dBpassword = pt.get<std::string>("database.password");
        
    }
    catch (boost::property_tree::ini_parser_error e)
    {
        Wt::log("error") << "[CONF] " << e.what();
    }
    
    
}

Conf::Conf(const Conf& orig) {
}

std::string Conf::getDbHost()
{
    return dBhost;
}

int Conf::getDbPort()
{
    return dBport;
}

std::string Conf::getDbName()
{
    return dBname;
}

std::string Conf::getDbUser()
{
    return dBuser;
}

std::string Conf::getDbPassword()
{
    return dBpassword;
}

Conf::~Conf() {
}

