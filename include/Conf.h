/* 
 * Header of API Configuration
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

#ifndef CONF_H
#define	CONF_H

#include <Wt/WServer>
#include <Wt/WLogger>

#include <unistd.h>

class Conf {
    public:
        Conf();
        virtual ~Conf();

        /**
         * Reads ECHOES configuration properties.
         * @param WServer
         * @return true if every value are defined or false
         */
        bool readProperties(Wt::WServer& server);

        std::string getSessConnectParams() const;
        std::string getSMTPHost() const;
        unsigned getSMTPPort() const;
        std::string getAlertMailSenderAddress() const;
        std::string getAlertMailSenderName() const;
        bool isSmsHttps() const;
        std::string getSmsPassword() const;
        std::string getSmsLogin() const;
        std::string getRouteurHost() const;
        unsigned getRouteurPort() const;
        unsigned getServerPort() const;
        std::string getFQDN() const;
        
        void setServerPort(unsigned port);
   
    protected:
        std::string m_sessConnectParams;
        std::string m_smtpHost;
        unsigned m_smtpPort;
        std::string m_alertMailSenderName;
        std::string m_alertMailSenderAddress;
        std::string m_smsLogin;
        std::string m_smsPassword;
        bool m_smsHttps;
        std::string m_routeurHost;
        unsigned m_routeurPort;
        unsigned m_serverPort;
        std::string m_fqdn;
        
        void setSessConnectParams
        (
                std::string dbHost,
                unsigned dbPort,
                std::string dbName,
                std::string dbUser,
                std::string dbPassword
        );
        void setSMTPPort(unsigned smtpPort);
        void setAlertMailSenderAddress(std::string alertMailSenderAddress);
        void setAlertMailSenderName(std::string alertMailSenderName);
        void setSMTPHost(std::string smtpHost);
        void setSmsHttps(bool smsHttps);
        void setSmsPassword(std::string smsPassword);
        void setSmsLogin(std::string smsLogin);
        void setRouteurPort(unsigned routeurPort);
        void setRouteurHost(std::string routeurHost);
        void setFQDN(std::string fqdn);

};

extern Conf conf;

#endif	/* CONF_H */

