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

#include <boost/config.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <Wt/WLogger>

class Conf {
    public:
        Conf();
        Conf(const Conf& orig);
        virtual ~Conf();

        std::string getDBHost() const;
        unsigned getDBPort() const;
        std::string getDBName() const;
        std::string getDBUser() const;
        std::string getDBPassword() const;
        std::string getSessConnectParams() const;

    protected:
        std::string _dBhost, _dBname, _dBuser, _dBpassword, _sessConnectParams;
        unsigned _dBport;

        void setDBpassword(std::string dBpassword);
        void setDBport(unsigned _dBport);
        void setDBuser(std::string dBuser);
        void setDBname(std::string dBname);
        void setDBhost(std::string dBhost);
        void setSessConnectParams
        (
                std::string dBhost,
                unsigned dBport,
                std::string dBname,
                std::string dBuser,
                std::string dBpassword
        );
};

#endif	/* CONF_H */

