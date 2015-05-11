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
        UnitResource(Echoes::Dbo::Session& session);
        virtual ~UnitResource();

    protected :
        EReturnCode getUnitsList(const long long &grpId, std::string &responseMsg);
        EReturnCode getUnit(const std::vector<std::string> &pathElements, const long long &grpId, std::string &responseMsg);
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);

        EReturnCode postUnit(const std::string &sRequest,  const long long &grpId, std::string &responseMsg);
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);

        EReturnCode putUnit(const std::vector<std::string> &pathElements, const std::string &sRequest, const long long &grpId, std::string &responseMsg);
        virtual EReturnCode processPutRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);

        EReturnCode deleteUnit(const std::vector<std::string> &pathElements, const long long &grpId, std::string &responseMsg);
        virtual EReturnCode processDeleteRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);
};


#endif	/* UNITRESOURCE_H */

