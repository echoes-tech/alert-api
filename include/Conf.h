/* 
 * Header of API Configuration
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

#ifndef CONF_H
#define	CONF_H

#include <Wt/WServer>
#include <Wt/WLogger>

class Conf {
    public:
        Conf();
        Conf(const Conf& orig);
        virtual ~Conf();

        /**
         * Reads ECHOES configuration properties.
         * @param WServer
         * @return true if every value are defined or false
         */
        bool readProperties(Wt::WServer& server);

        std::string getDBHost() const;
        unsigned getDBPort() const;
        std::string getDBName() const;
        std::string getDBUser() const;
        std::string getDBPassword() const;
        std::string getSessConnectParams() const;
        std::string getSMTPHost() const;
        unsigned getSMTPPort() const;
        std::string getAlertMailSenderAddress() const;
        std::string getAlertMailSenderName() const;       

    protected:
        std::string _dbHost, _dbName, _dbUser, _dbPassword, _sessConnectParams, _smtpHost, _alertMailSenderName, _alertMailSenderAddress;
        unsigned _dbPort, _smtpPort;

        void setDBPassword(std::string dbPassword);
        void setDBPort(unsigned dbPort);
        void setDBUser(std::string dbUser);
        void setDBName(std::string dbName);
        void setDBHost(std::string dbHost);
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
};

extern Conf conf;

#endif	/* CONF_H */

