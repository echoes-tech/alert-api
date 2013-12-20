/* 
 * Header of API SourceResource
 * @author ECHOES Technologies (FPO)
 * @date 08/12/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */


#ifndef SOURCERESOURCE_H
#define	SOURCERESOURCE_H

#include "PublicApiResource.h"

class SourceResource : public PublicApiResource
{
    public:
        SourceResource(Echoes::Dbo::Session*);
        virtual ~SourceResource();

        /**
         * Select a Source
         * @param srcId Identifier of Source
         * @param orgId Identifier of Organization
         * @return Source Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Source> selectSource(const long long &srcId, const long long &orgId, Echoes::Dbo::Session &session);
        /**
         * Select an Source with a string of identifier of Source
         * @param srcId String of Identifier of Source
         * @param orgId Identifier of Organization
         * @return Source Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Source> selectSource(const std::string &srcId, const long long &orgId, Echoes::Dbo::Session &session);
        /**
         * Select a Source with a ID string
         * @param srcId String of Identifier of Source
         * @param orgId String of Identifier of Organization
         * @return Source Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Source> selectSource(const std::string &srcId, const std::string &orgId, Echoes::Dbo::Session &session);

    private:
        EReturnCode getSourcesList(std::map<std::string, long long> &parameters, const long long &orgId, std::string &responseMsg);
        EReturnCode getSource(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        EReturnCode getParametersList(std::map<std::string, long long> &parameters, const long long &orgId, std::string &responseMsg);
        EReturnCode getParametersListForSource(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode postSource(const std::string &sRequest,  const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode putSource(const std::string &sRequest,  const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processPutRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode deleteSource(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processDeleteRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);
};

#endif	/* SOURCERESOURCE_H */

