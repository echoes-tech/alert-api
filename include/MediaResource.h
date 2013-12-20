/* 
 * Header of API PluginResource
 * @author ECHOES Technologies (GDR)
 * @date 11/02/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef MEDIARESOURCE_H
#define	MEDIARESOURCE_H

#include "PublicApiResource.h"

class MediaResource : public PublicApiResource
{
    public :
        MediaResource(Echoes::Dbo::Session*);
        virtual ~MediaResource();

        /**
         * Select a Media
         * @param medId Identifier of Media
         * @param orgId Identifier of Organization
         * @return Media Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Media> selectMedia(const long long &medId, const long long &orgId, Echoes::Dbo::Session &session);
        /**
         * Select an Media with a string of identifier of Media
         * @param medId String of Identifier of Media
         * @param orgId Identifier of Organization
         * @return Media Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Media> selectMedia(const std::string &medId, const long long &orgId, Echoes::Dbo::Session &session);
        /**
         * Select a Media with a ID string
         * @param medId String of Identifier of Media
         * @param orgId String of Identifier of Organization
         * @return Media Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Media> selectMedia(const std::string &medId, const std::string &orgId, Echoes::Dbo::Session &session);

    protected :
        EReturnCode getMedia(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        EReturnCode getMediasList(std::map<std::string, long long> &parameters, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode postMedia(const std::string &sRequest,  const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode putMedia(const std::vector<std::string> &pathElements, const std::string &sRequest,  const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processPutRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);

        EReturnCode deleteMedia(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg);
        virtual EReturnCode processDeleteRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg);
};

#endif	/* MEDIARESOURCE_H */

