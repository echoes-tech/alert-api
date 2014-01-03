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
        InformationResource(Echoes::Dbo::Session*);
        virtual ~InformationResource();
        
    protected :
        EReturnCode getInformationsList(const long long &orgId, std::string &responseMsg);
        EReturnCode getInformation(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        EReturnCode getAliasForInformation(const std::vector<std::string> &pathElements, std::map<std::string, long long> &parameters, const long long &orgId, std::string  &responseMsg);
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode postInformation(const std::string &sRequest, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode putInformation(const std::vector<std::string> &pathElements, const std::string &sRequest,  const long long &orgId, std::string &responseMsg);
        EReturnCode putAliasForInformation(const std::vector<std::string> &pathElements, const std::string &sRequest, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processPutRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode deleteInformation(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processDeleteRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);
};


#endif	/* INFORMATIONRESOURCE_H */

