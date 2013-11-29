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
        OptionResource();
        virtual ~OptionResource();

    private:
        EReturnCode getOptionsList(std::string &responseMsg);
        EReturnCode getOption(std::string &responseMsg);
        virtual void processGetRequest(Wt::Http::Response &response);
};

#endif	/* OPTIONRESOURCE_H */

