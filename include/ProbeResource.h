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
        ProbeResource(Echoes::Dbo::Session& session);
        virtual ~ProbeResource();

        /**
         * Select a Probe
         * @param prbId Identifier of Probe
         * @param grpId Identifier of Group
         * @return Probe Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Probe> selectProbe(const long long &prbId, const long long &grpId, Echoes::Dbo::Session &session);
        /**
         * Select an Probe with a string of identifier of Probe
         * @param prbId String of Identifier of Probe
         * @param grpId Identifier of Group
         * @return Probe Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Probe> selectProbe(const std::string &prbId, const long long &grpId, Echoes::Dbo::Session &session);
        /**
         * Select a Probe with a ID string
         * @param prbId String of Identifier of Probe
         * @param grpId Identifier of Group
         * @return Probe Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Probe> selectProbe(const std::string &prbId, const std::string &grpId, Echoes::Dbo::Session &session);

        /**
         * Select a Probe Package Parameter
         * @param prbId Identifier of Probe
         * @param grpId String of Identifier of Group
         * @return Probe Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::ProbePackageParameter> selectProbePackageParameter(const Wt::Dbo::ptr<Echoes::Dbo::Asset> &astPtr, Echoes::Dbo::Session &session);
        
        static int                      m_defaultTimer;
        
    protected:
        std::map<long long, int>        m_mapTimer;
        
        EReturnCode getProbesList(const long long &grpId, std::string &responseMsg);
        EReturnCode getProbe(const std::vector<std::string> &pathElements, const long long &grpId, std::string &responseMsg);
        EReturnCode getAliveProbe(const std::vector<std::string> &pathElements, const long long &grpId, std::string &responseMsg);
        EReturnCode getJsonForProbe(const std::vector<std::string> &pathElements, const long long &grpId, std::string &responseMsg);
        EReturnCode getPackagesForProbe(const std::vector<std::string> &pathElements, const long long &grpId, std::string &responseMsg);
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);

        EReturnCode postProbe(const std::string &sRequest,  const long long &grpId, std::string &responseMsg);
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);

        EReturnCode putProbe(const std::vector<std::string> &pathElements, const std::string &sRequest, const long long &grpId, std::string &responseMsg);
        virtual EReturnCode processPutRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);
        
        EReturnCode deleteProbe(const std::vector<std::string> &pathElements, const long long &grpId, std::string &responseMsg);
        virtual EReturnCode processDeleteRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);
};

#endif	/* PROBERESOURCE_H */

