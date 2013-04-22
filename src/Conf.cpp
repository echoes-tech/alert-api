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

#include "Conf.h"

using namespace std;

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
        setDBhost(pt.get<string>("database.host"));
        setDBport(pt.get<unsigned>("database.port"));
        setDBname(pt.get<string>("database.name"));
        setDBuser(pt.get<string>("database.user"));
        setDBpassword(pt.get<string>("database.password"));
        
    }
    catch (boost::property_tree::ini_parser_error e)
    {
        Wt::log("error") << "[CONF] " << e.what();
    }
    
    setSessConnectParams(_dBhost, _dBport, _dBname, _dBuser, _dBpassword);
}

Conf::Conf(const Conf& orig)
{
    setDBhost(orig.getDBHost());
    setDBport(orig.getDBPort());
    setDBname(orig.getDBName());
    setDBuser(orig.getDBUser());
    setDBpassword(orig.getDBPassword());
    setSessConnectParams(_dBhost, _dBport, _dBname, _dBuser, _dBpassword);
}

Conf::~Conf()
{
}

void Conf::setDBhost(string dBhost)
{
    _dBhost = dBhost;

    return;
}

string Conf::getDBHost() const
{
    return _dBhost;
}

void Conf::setDBport(unsigned dBport)
{
    _dBport = dBport;

    return;
}

unsigned Conf::getDBPort() const
{
    return _dBport;
}

void Conf::setDBname(string dBname)
{
    _dBname = dBname;

    return;
}

string Conf::getDBName() const
{
    return _dBname;
}

void Conf::setDBuser(string dBuser)
{
    _dBuser = dBuser;

    return;
}

string Conf::getDBUser() const
{
    return _dBuser;
}

void Conf::setDBpassword(string dBpassword)
{
    _dBpassword = dBpassword;

    return;
}

string Conf::getDBPassword() const
{
    return _dBpassword;
}

void Conf::setSessConnectParams
(
        string dBhost,
        unsigned dBport,
        string dBname,
        string dBuser,
        string dBpassword
)
{   
    try
    {
        _sessConnectParams = "hostaddr=" + dBhost +
                             " port=" + boost::lexical_cast<string>(dBport) +
                             " dbname=" + dBname +
                             " user=" + dBuser +
                             " password=" + dBpassword;
    }
    catch (boost::bad_lexical_cast &)
    {
         Wt::log("error") << "[Conf] sdPort is not an unsigned";
    }

    return;
}

string Conf::getSessConnectParams() const
{
    return _sessConnectParams;
}


