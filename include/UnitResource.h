/* 
 * HEADER OF API UnitResource
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
        UnitResource(const UnitResource& orig);
        virtual ~UnitResource();
        
    protected :

        unsigned short getTypeOfUnits(std::string &responseMsg);
        unsigned short getTypeForUnit(std::string &responseMsg);
        unsigned short getSubUnitsForUnit(std::string &responseMsg);
        unsigned short getListUnits(std::string &responseMsg);
        unsigned short getUnit(std::string &responseMsg);
        virtual void processGetRequest(Wt::Http::Response &response);

        unsigned short postUnit(std::string &responseMsg, const std::string &sRequest);
        virtual void processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        
        virtual void processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        unsigned short deleteUnit(std::string &responseMsg);
        virtual void processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);          
};


#endif	/* UNITRESOURCE_H */

