/* 
 * Header of API CriterionResource
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

#ifndef CRITERIONRESOURCE_H
#define	CRITERIONRESOURCE_H

#include "PublicApiResource.h"

class CriterionResource : public PublicApiResource
{
    public :
        CriterionResource();
        virtual ~CriterionResource();

    protected :
        EReturnCode getCriteriaList(const long long &orgId, std::string &responseMsg);
        EReturnCode getCriterion(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        EReturnCode getAliasForCriterion(const std::vector<std::string> &pathElements, std::map<std::string, long long> &parameters, const long long &orgId, std::string  &responseMsg);
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode putAliasForCriterion(const std::vector<std::string> &pathElements, const std::string &sRequest, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processPutRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);
};

#endif	/* CRITERIONRESOURCE_H */

