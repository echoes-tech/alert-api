/* 
 * HEADER OF API CriterionResource
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
        EReturnCode getCriteriaList(std::string &responseMsg);
        EReturnCode getCriterion(std::string &responseMsg);
        virtual void processGetRequest(Wt::Http::Response &response);
};

#endif	/* CRITERIONRESOURCE_H */

