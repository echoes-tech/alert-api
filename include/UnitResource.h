/* 
 * Header of API UnitResource
 * @author ECHOES Technologies (GDR)
 * @date 21/02/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef UNITRESOURCE_H
#define	UNITRESOURCE_H

#include "PublicApiResource.h"

class UnitResource : public PublicApiResource
{
    public :
        UnitResource();
        virtual ~UnitResource();
        
    protected :
        EReturnCode getUnitsList(std::string &responseMsg);
        EReturnCode getUnit(std::string &responseMsg);
        virtual void processGetRequest(Wt::Http::Response &response);

        EReturnCode postUnit(std::string &responseMsg, const std::string &sRequest);
        virtual void processPostRequest(Wt::Http::Response &response);

        EReturnCode putUnit(std::string &responseMsg, const std::string &sRequest);
        virtual void processPutRequest(Wt::Http::Response &response);

        EReturnCode deleteUnit(std::string &responseMsg);
        virtual void processDeleteRequest(Wt::Http::Response &response);
};


#endif	/* UNITRESOURCE_H */

