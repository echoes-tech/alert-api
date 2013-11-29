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
        virtual ~MediaResource();
        
    protected :
        
        Wt::WString m_mediaId;
      
        EReturnCode getMedia(std::string &responseMsg);
        EReturnCode getMediasList(std::string &responseMsg);
        virtual void processGetRequest(Wt::Http::Response &response);

        EReturnCode postMediaSpecialization(std::string &responseMsg, const std::string &sRequest);
        EReturnCode postMedia(std::string &responseMsg, const std::string &sRequest);
        EReturnCode postMediaValidation(std::string &responseMsg, const std::string &sRequest);
        virtual void processPostRequest(Wt::Http::Response &response);

        virtual void processPutRequest(Wt::Http::Response &response);
        
        virtual void processPatchRequest(Wt::Http::Response &response);

        EReturnCode deleteMediaSpecialization(std::string &responseMsg);
        EReturnCode deleteMedia(std::string &responseMsg);
        virtual void processDeleteRequest(Wt::Http::Response &response);

        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response);
};

#endif	/* MEDIARESOURCE_H */

