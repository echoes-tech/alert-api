/* 
 * Header of API Probe Ressource
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

#ifndef PROBERESSOURCE_H
#define	PROBERESSOURCE_H

#include "includeFile.h"

class ProbeRessource : public PublicApiResource
{
    public:
        ProbeRessource();
        ProbeRessource(const ProbeRessource& orig);
        virtual ~ProbeRessource();

    protected:
        int probeId, assetId;
        Wt::WString arch, distribName, distribRelease;

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
};

#endif	/* PROBERESSOURCE_H */

