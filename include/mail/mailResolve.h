/* 
 * Header of  mail Resolve Resource
 * @author ECHOES Technologies (CDE)
 * @date 15/09/2015
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2015 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef MAILRESOLVE_H
#define	MAILRESOLVE_H

#include "PublicMailResource.h"

class MailResolve   : public PublicMailResource
{
    public:
        MailResolve(Echoes::Dbo::Session& session);
        virtual ~MailResolve();

    protected:

        std::string m_number;
        std::string m_message;
        
        void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response, EReturnCode *res, string *responseMsg, bool * answered);
        EReturnCode doMailResolve(map<string, string> parameters, const vector<string> &pathElements, const string &sRequest, string &responseMsg, const Wt::Http::Request &request);
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, std::string &responseMsg);
        EReturnCode PostResolve(map<string, string> parameters, const vector<string> &pathElements, const string &sRequest, string &responseMsg, const Wt::Http::Request &request);
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, std::string &responseMsg);
};



#endif	/* MAILRESOLVE_H */

