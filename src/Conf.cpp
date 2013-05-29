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

Conf conf;

Conf::Conf()
{
}

Conf::Conf(const Conf& orig)
{
    setDBHost(orig.getDBHost());
    setDBPort(orig.getDBPort());
    setDBName(orig.getDBName());
    setDBUser(orig.getDBUser());
    setDBPassword(orig.getDBPassword());
    setSessConnectParams(_dbHost, _dbPort, _dbName, _dbUser, _dbPassword);
    setSMTPHost(orig.getSMTPHost());
    setSMTPPort(orig.getSMTPPort());
}

Conf::~Conf()
{
}

bool Conf::readProperties(Wt::WServer& server)
{
    Wt::log("debug") << "[Conf] Read properties from " << WT_CONFIG_XML;

    bool res = false;
    string dbHost = "", dbPort = "", dbName = "", dbUser = "", dbPassword = "", smtpHost = "", smtpPort = "", alertMailSenderName = "", alertMailSenderAddress = "";

    if
      (
       server.readConfigurationProperty("db-host", dbHost)
       && server.readConfigurationProperty("db-port", dbPort)
       && server.readConfigurationProperty("db-name", dbName)
       && server.readConfigurationProperty("db-user", dbUser)
       && server.readConfigurationProperty("db-password", dbPassword)
      )
    {
        setDBHost(dbHost);
        try
        {
            setDBPort(boost::lexical_cast<unsigned>(dbPort));
        }
        catch (boost::bad_lexical_cast &)
        {
             Wt::log("error") << "[Conf] Property named 'db-port' in " << WT_CONFIG_XML << " should be an unsigned integer";
             return res;
        }
        setDBName(dbName);
        setDBUser(dbUser);
        setDBPassword(dbPassword);

        setSessConnectParams(_dbHost, _dbPort, _dbName, _dbUser, _dbPassword);

        if (server.readConfigurationProperty("smtp-host", smtpHost) && server.readConfigurationProperty("smtp-port", smtpPort))
        {
            setSMTPHost(smtpHost);
            try
            {
                setSMTPPort(boost::lexical_cast<unsigned>(smtpPort));
            }
            catch (boost::bad_lexical_cast &)
            {
                 Wt::log("error") << "[Conf] Property named 'smtp-port' in " << WT_CONFIG_XML << " should be an unsigned integer";
                 return res;
            }
            
            if (server.readConfigurationProperty("alert-mail-sender-name", alertMailSenderName))
                setAlertMailSenderName(alertMailSenderName);
            else
            {
                setAlertMailSenderName("ECHOES Alert");
                Wt::log("warning") << "[Conf] Property named 'alert-mail-sender-name' is set 'ECHOES Alert' because it is not set in " << WT_CONFIG_XML << ".";
            }
            
            if (server.readConfigurationProperty("alert-mail-sender-address", alertMailSenderAddress))
                setAlertMailSenderName(alertMailSenderAddress);
            else
            {
                setAlertMailSenderName("noreply-alert@echoes-tech.com");
                Wt::log("warning") << "[Conf] Property named 'alert-mail-sender-address' is set 'noreply-alert@echoes-tech.com' because it is not set in " << WT_CONFIG_XML << ".";
            }

            res = true;
        }
        else
            Wt::log("error") << "[Conf] Incomplete SMTP properties in " << WT_CONFIG_XML;
    }
    else
        Wt::log("error") << "[Conf] Incomplete DB properties in " << WT_CONFIG_XML;
    
    return res;
}

void Conf::setDBHost(string dbHost)
{
    _dbHost = dbHost;

    return;
}

string Conf::getDBHost() const
{
    return _dbHost;
}

void Conf::setDBPort(unsigned dbPort)
{
    _dbPort = dbPort;

    return;
}

unsigned Conf::getDBPort() const
{
    return _dbPort;
}

void Conf::setDBName(string dbName)
{
    _dbName = dbName;

    return;
}

string Conf::getDBName() const
{
    return _dbName;
}

void Conf::setDBUser(string dbUser)
{
    _dbUser = dbUser;

    return;
}

string Conf::getDBUser() const
{
    return _dbUser;
}

void Conf::setDBPassword(string dbPassword)
{
    _dbPassword = dbPassword;

    return;
}

string Conf::getDBPassword() const
{
    return _dbPassword;
}

void Conf::setSessConnectParams
(
    string dbHost,
    unsigned dbPort,
    string dbName,
    string dbUser,
    string dbPassword
)
{   
    _sessConnectParams = "hostaddr=" + dbHost +
                         " port=" + boost::lexical_cast<string>(dbPort) +
                         " dbname=" + dbName +
                         " user=" + dbUser +
                         " password=" + dbPassword;
    return;
}

string Conf::getSessConnectParams() const
{
    return _sessConnectParams;
}

void Conf::setSMTPHost(std::string smtpHost)
{
    _smtpHost = smtpHost;
    return;
}

std::string Conf::getSMTPHost() const
{
    return _smtpHost;
}

void Conf::setSMTPPort(unsigned smtpPort)
{
    _smtpPort = smtpPort;
    return;
}

unsigned Conf::getSMTPPort() const
{
    return _smtpPort;
}

void Conf::setAlertMailSenderAddress(std::string alertMailSenderAddress)
{
    _alertMailSenderAddress = alertMailSenderAddress;
    return;
}

std::string Conf::getAlertMailSenderAddress() const
{
    return _alertMailSenderAddress;
}

void Conf::setAlertMailSenderName(std::string alertMailSenderName)
{
    _alertMailSenderName = alertMailSenderName;
    return;
}

std::string Conf::getAlertMailSenderName() const
{
    return _alertMailSenderName;
}

