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

class Conf {
    public:
        Conf();
        Conf(const Conf& orig);
        std::string getDbHost();
        int getDbPort();
        std::string getDbName();
        std::string getDbUser();
        std::string getDbPassword();
        virtual ~Conf();

    protected:
        std::string dBhost, dBname, dBuser, dBpassword;
        int dBport;
};

#endif	/* CONF_H */

