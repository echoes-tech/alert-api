/* 
 * API Configuration
 * @author ECHOES Technologies (FPO)
 * @date 05/11/2012
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2012-2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "Conf.h"

using namespace std;

Conf conf;

Conf::Conf() :
m_sessConnectParams(""),
m_smtpHost(""),
m_smtpPort(0),
m_alertMailSenderName("ECHOES Alert"),
m_alertMailSenderAddress("noreply-alert@echoes-tech.com"),
m_smsLogin(""),
m_smsPassword(""),
m_smsHttps(true),
m_routeurHost(""),
m_routeurPort(0),
m_serverPort(0),
m_fqdn("")
{
}

Conf::~Conf()
{
}

bool Conf::readProperties(Wt::WServer& server)
{
    Wt::log("debug") << "[Conf] Read properties from " << WT_CONFIG_XML ;

    bool res = false;
    struct Db
    {
        string host;
        string port;
        string name;
        string user;
        string password;
    };
    Db db;
    struct Smtp
    {
        string host;
        string port;
    };
    Smtp smtp;
    struct AlertMailSender
    {
        string name;
        string address;
    };
    AlertMailSender alertMailSender;
    struct Sms
    {
        string login;
        string password;
        string https;
        string host;
        string port;
    };
    Sms sms;

    if
      (
       server.readConfigurationProperty("db-host", db.host)
       && server.readConfigurationProperty("db-port", db.port)
       && server.readConfigurationProperty("db-name", db.name)
       && server.readConfigurationProperty("db-user", db.user)
       && server.readConfigurationProperty("db-password", db.password)
      )
    {
        try
        {
            setSessConnectParams
            (
                db.host,
                boost::lexical_cast<unsigned>(db.port),
                db.name,
                db.user,
                db.password
            );
        }
        catch (boost::bad_lexical_cast &)
        {
             Wt::log("error") << "[Conf] Property named 'db-port' in " << WT_CONFIG_XML << " should be an unsigned integer";
             return res;
        }

        if (server.readConfigurationProperty("smtp-host", smtp.host) && server.readConfigurationProperty("smtp-port", smtp.port))
        {
            setSMTPHost(smtp.host);
            try
            {
                setSMTPPort(boost::lexical_cast<unsigned>(smtp.port));
            }
            catch (boost::bad_lexical_cast &)
            {
                 Wt::log("error") << "[Conf] Property named 'smtp-port' in " << WT_CONFIG_XML << " should be an unsigned integer";
                 return res;
            }
            
            if (server.readConfigurationProperty("alert-mail-sender-name", alertMailSender.name))
            {
                setAlertMailSenderName(alertMailSender.name);
            }
            else
            {
                Wt::log("warning") << "[Conf] Property named 'alert-mail-sender-name' is set 'ECHOES Alert' because it is not set in " << WT_CONFIG_XML;
            }
            
            if (server.readConfigurationProperty("alert-mail-sender-address", alertMailSender.address))
            {
                setAlertMailSenderName(alertMailSender.address);
            }
            else
            {
                Wt::log("warning") << "[Conf] Property named 'alert-mail-sender-address' is set 'noreply-alert@echoes-tech.com' because it is not set in " << WT_CONFIG_XML;
            }

            if
            (
                server.readConfigurationProperty("sms-login", sms.login)
                && server.readConfigurationProperty("sms-password", sms.password)
                && server.readConfigurationProperty("sms-https", sms.https)
                && server.readConfigurationProperty("sms-routeur-addr", sms.host)
                && server.readConfigurationProperty("sms-routeur-port", sms.port)
            )
            {
                setSmsLogin(sms.login);
                setSmsPassword(sms.password);
                if (sms.https.compare("true") == 0)
                {
                    setSmsHttps(true);
                }
                else if (sms.https.compare("false") == 0)
                {
                    setSmsHttps(false);
                }
                else
                {
                     Wt::log("error") << "[Conf] Property named 'sms-https' in " << WT_CONFIG_XML << " should be an boolean (true or false). By default: true";
                }
                
                setRouteurHost(sms.host);
                try
                {
                    setRouteurPort(boost::lexical_cast<unsigned>(sms.port));
                }
                catch (boost::bad_lexical_cast &)
                {
                     Wt::log("error") << "[Conf] Property named 'sms-routeur-port' in " << WT_CONFIG_XML << " should be an unsigned integer";
                     return res;
                }
                
                std::string fqdn;
                if(server.readConfigurationProperty("fqdn", fqdn))
                {
                    setFQDN(fqdn);
                }
                else
                {
                    setFQDN("127.0.0.1");
                    Wt::log("warning") << "[Conf] no fqdn in " << WT_CONFIG_XML << "set to 127.0.0.1";
                }
            }
            else
            {
                Wt::log("warning") << "[Conf] Incomplete SMS properties in " << WT_CONFIG_XML;
            }

            res = true;
        }
        else
        {
            Wt::log("error") << "[Conf] Incomplete SMTP properties in " << WT_CONFIG_XML;
        }
    }
    else
    {
        Wt::log("error") << "[Conf] Incomplete DB properties in " << WT_CONFIG_XML;
    }
    
    return res;
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
    m_sessConnectParams = "hostaddr=" + dbHost +
                         " port=" + boost::lexical_cast<string>(dbPort) +
                         " dbname=" + dbName +
                         " user=" + dbUser +
                         " password=" + dbPassword;
    return;
}

string Conf::getSessConnectParams() const
{
    return m_sessConnectParams;
}

void Conf::setSMTPHost(string smtpHost)
{
    m_smtpHost = smtpHost;
    return;
}

string Conf::getSMTPHost() const
{
    return m_smtpHost;
}

void Conf::setSMTPPort(unsigned smtpPort)
{
    m_smtpPort = smtpPort;
    return;
}

unsigned Conf::getSMTPPort() const
{
    return m_smtpPort;
}

void Conf::setAlertMailSenderAddress(string alertMailSenderAddress)
{
    m_alertMailSenderAddress = alertMailSenderAddress;
    return;
}

string Conf::getAlertMailSenderAddress() const
{
    return m_alertMailSenderAddress;
}

void Conf::setAlertMailSenderName(string alertMailSenderName)
{
    m_alertMailSenderName = alertMailSenderName;
    return;
}

string Conf::getAlertMailSenderName() const
{
    return m_alertMailSenderName;
}

void Conf::setSmsHttps(bool smsHttps)
{
    m_smsHttps = smsHttps;
    return;
}

bool Conf::isSmsHttps() const
{
    return m_smsHttps;
}

void Conf::setSmsPassword(string smsPassword)
{
    m_smsPassword = smsPassword;
    return;
}

string Conf::getSmsPassword() const
{
    return m_smsPassword;
}

void Conf::setSmsLogin(string smsLogin)
{
    m_smsLogin = smsLogin;
    return;
}

string Conf::getSmsLogin() const
{
    return m_smsLogin;
}

void Conf::setRouteurPort(unsigned routeurPort)
{
    m_routeurPort = routeurPort;
    return;
}

unsigned Conf::getRouteurPort() const
{
    return m_routeurPort;
}

void Conf::setRouteurHost(std::string routeurHost)
{
    m_routeurHost = routeurHost;
    return;
}

string Conf::getRouteurHost() const
{
    return m_routeurHost;
}

void Conf::setServerPort(unsigned port)
{
    m_serverPort = port;
    return;
}

unsigned Conf::getServerPort() const
{
    return m_serverPort;
}

void Conf::setFQDN(std::string fqdn)
{
    m_fqdn = fqdn;
    return;
}

std::string Conf::getFQDN() const
{
    return m_fqdn;
}