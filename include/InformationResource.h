/* 
 * Header of API InformationResource
 * @author ECHOES Technologies (GDR)
 * @date 13/02/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef INFORMATIONRESOURCE_H
#define	INFORMATIONRESOURCE_H

#include "PublicApiResource.h"

class InformationResource : public PublicApiResource
{
    public :
        InformationResource();
        virtual ~InformationResource();
        
    protected :
        EReturnCode getInformationsList(std::string &responseMsg);
        EReturnCode getInformation(std::string &responseMsg);
        EReturnCode getAliasForInformation(std::string  &responseMsg);
        virtual void processGetRequest(Wt::Http::Response &response);

        EReturnCode postInformation(std::string &responseMsg, const std::string &sRequest);
        virtual void processPostRequest(Wt::Http::Response &response);

        EReturnCode putAliasForInformation(std::string &responseMsg, const std::string &sRequest);
        virtual void processPutRequest(Wt::Http::Response &response);

        virtual void processDeleteRequest(Wt::Http::Response &response);
};


#endif	/* INFORMATIONRESOURCE_H */

