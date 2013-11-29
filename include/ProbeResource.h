/* 
 * Header of API Probe Resource
 * @author ECHOES Technologies (FPO)
 * @date 26/09/2012
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2012 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef PROBERESOURCE_H
#define	PROBERESOURCE_H

#include "PublicApiResource.h"

class ProbeResource : public PublicApiResource
{
    public:
        ProbeResource();
        virtual ~ProbeResource();

    protected:
        EReturnCode getProbesList(std::string &responseMsg);
        EReturnCode getProbe(std::string &responseMsg);
        virtual void processGetRequest(Wt::Http::Response &response);

        EReturnCode postProbe(std::string &responseMsg, const std::string &sRequest);
        virtual void processPostRequest(Wt::Http::Response &response);

        virtual void processPutRequest(Wt::Http::Response &response);

        virtual void processDeleteRequest(Wt::Http::Response &response);

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
};

#endif	/* PROBERESOURCE_H */

