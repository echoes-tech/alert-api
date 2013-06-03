/* 
 * HEADER OF API PluginResource
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
        MediaResource(const MediaResource& orig);
        virtual ~MediaResource();
        
    protected :
        
        Wt::WString mediaId;
      
        unsigned short getMedia(std::string &responseMsg);
        unsigned short getListValueForMedia(std::string &responseMsg);
        virtual void processGetRequest(Wt::Http::Response &response);

        unsigned short postMediaSpecialization(std::string &responseMsg, const std::string &sRequest);
        unsigned short postMedia(std::string &responseMsg, const std::string &sRequest);
        unsigned short postMediaValidation(std::string &responseMsg, const std::string &sRequest);
        virtual void processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        
        virtual void processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        unsigned short deleteMediaSpecialization(std::string &responseMsg);
        unsigned short deleteMedia(std::string &responseMsg);
        virtual void processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
};

#endif	/* MEDIARESOURCE_H */

