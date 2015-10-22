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
        SourceResource(Echoes::Dbo::Session& session);
        virtual ~SourceResource();

        /**
         * Select a Source
         * @param srcId Identifier of Source
         * @param grpId Identifier of Group
         * @return Source Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Source> selectSource(const long long &srcId, const long long &grpId, Echoes::Dbo::Session &session);
        /**
         * Select an Source with a string of identifier of Source
         * @param srcId String of Identifier of Source
         * @param grpId Identifier of Group
         * @return Source Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Source> selectSource(const std::string &srcId, const long long &grpId, Echoes::Dbo::Session &session);
        /**
         * Select a Source with a ID string
         * @param srcId String of Identifier of Source
         * @param grpId String of Identifier of Group
         * @return Source Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Source> selectSource(const std::string &srcId, const std::string &grpId, Echoes::Dbo::Session &session);

    private:
        EReturnCode getSourcesList(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        EReturnCode getSource(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        EReturnCode getParametersList(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        EReturnCode getParametersListForSource(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);

        EReturnCode postSource(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);

        EReturnCode putSource(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        virtual EReturnCode processPutRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);

        EReturnCode deleteSource(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements = std::vector<std::string>(), const std::string &sRequest = "", std::map<string, long long> parameters = std::map<string, long long>());
        virtual EReturnCode processDeleteRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg);
};

#endif	/* SOURCERESOURCE_H */

