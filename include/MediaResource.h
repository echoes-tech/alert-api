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

#include "includeFile.h"

class MediaResource : public PublicApiResource
{
    public :
        MediaResource();
        virtual ~MediaResource();
        
    protected :
        
        Wt::WString mediaId;
      
        std::string getMedia();
        std::string getListValueForMedia();
        virtual void processGetRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        std::string postMediaSpecialization(std::string sRequest);
        std::string postMedia(std::string sRequest);
        virtual void processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
        
        virtual void processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        std::string deleteMediaSpecialization();
        std::string deleteMedia();
        virtual void processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response);

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
};

#endif	/* MEDIARESOURCE_H */

