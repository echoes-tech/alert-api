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
#include "AssetResource.h"

class ProbeResource : public PublicApiResource
{
    public:
        ProbeResource();
        virtual ~ProbeResource();

        /**
         * Select a Probe
         * @param prbId Identifier of Probe
         * @return Probe Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Probe> selectProbe(const long long &prbId, Echoes::Dbo::Session &session);
        /**
         * Select a Probe with a ID string
         * @param prbId String of Identifier of Probe
         * @return Probes Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Probe> selectProbe(const std::string &prbId, Echoes::Dbo::Session &session);

        /**
         * Select a Probe Package Parameter
         * @param astId Identifier of Asset
         * @return Probe Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::ProbePackageParameter> selectProbePackageParameter(const Wt::Dbo::ptr<Echoes::Dbo::Asset> &astPtr, Echoes::Dbo::Session &session);
        
    protected:
        EReturnCode getProbesList(std::string &responseMsg);
        EReturnCode getProbe(std::string &responseMsg);
        virtual void processGetRequest(Wt::Http::Response &response);

        EReturnCode postProbe(std::string &responseMsg, const std::string &sRequest);
        virtual void processPostRequest(Wt::Http::Response &response);

        EReturnCode putProbe(std::string &responseMsg, const std::string &sRequest);
        virtual void processPutRequest(Wt::Http::Response &response);

        EReturnCode deleteProbe(std::string &responseMsg);
        virtual void processDeleteRequest(Wt::Http::Response &response);
};

#endif	/* PROBERESOURCE_H */

