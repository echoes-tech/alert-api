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
        MediaResource();
        virtual ~MediaResource();

        /**
         * Select a Media
         * @param medId Identifier of Media
         * @return Media Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Media> selectMedia(const long long &medId, Echoes::Dbo::Session &session);
        /**
         * Select a Media with a ID string
         * @param medId String of Identifier of Media
         * @return Media Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Media> selectMedia(const std::string &medId, Echoes::Dbo::Session &session);

    protected :
        EReturnCode getMedia(std::string &responseMsg);
        EReturnCode getMediasList(std::string &responseMsg);
        virtual void processGetRequest(Wt::Http::Response &response);

        EReturnCode postMedia(std::string &responseMsg, const std::string &sRequest);
        virtual void processPostRequest(Wt::Http::Response &response);

        EReturnCode putMedia(std::string &responseMsg, const std::string &sRequest);
        virtual void processPutRequest(Wt::Http::Response &response);

        EReturnCode deleteMedia(std::string &responseMsg);
        virtual void processDeleteRequest(Wt::Http::Response &response);
};

#endif	/* MEDIARESOURCE_H */

