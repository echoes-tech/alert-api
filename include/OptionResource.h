/* 
 * Header of API Option Resource
 * @author ECHOES Technologies (FPO)
 * @date 29/11/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef OPTIONRESOURCE_H
#define	OPTIONRESOURCE_H

#include "PublicApiResource.h"

class OptionResource : public PublicApiResource
{
    public:
        OptionResource(Echoes::Dbo::Session*);
        virtual ~OptionResource();

    private:
        EReturnCode getOptionsList(std::map<std::string, long long> &parameters, const long long &orgId, std::string &responseMsg);
        EReturnCode getOption(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);
};

#endif	/* OPTIONRESOURCE_H */

