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
        SourceResource();
        virtual ~SourceResource();

        /**
         * Select a Source
         * @param srcId Identifier of Source
         * @return Source Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Source> selectSource(const long long &srcId, Echoes::Dbo::Session &session);
        /**
         * Select a Source with a ID string
         * @param srcId String of Identifier of Source
         * @return Source Wt Dbo Pointer
         */
        static Wt::Dbo::ptr<Echoes::Dbo::Source> selectSource(const std::string &srcId, Echoes::Dbo::Session &session);

    private:
        EReturnCode getSourcesList(std::string &responseMsg);
        EReturnCode getSource(std::string &responseMsg);
        virtual void processGetRequest(Wt::Http::Response &response);

        EReturnCode postSource(std::string &responseMsg, const std::string &sRequest);
        virtual void processPostRequest(Wt::Http::Response &response);

        EReturnCode putSource(std::string &responseMsg, const std::string &sRequest);
        virtual void processPutRequest(Wt::Http::Response &response);

        EReturnCode deleteSource(std::string &responseMsg);
        virtual void processDeleteRequest(Wt::Http::Response &response);
};

#endif	/* SOURCERESOURCE_H */

