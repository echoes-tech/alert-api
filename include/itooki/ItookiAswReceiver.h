/* 
 * Header of Itooki SMS Answer Receiver
 * @author ECHOES Technologies (TSA)
 * @date 08/08/2012
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2012-2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef ASWRECEIVER_H
#define	ASWRECEIVER_H

#include "PublicItookiResource.h"

class ItookiAswReceiver : public PublicItookiResource
{
    public:
        ItookiAswReceiver(Echoes::Dbo::Session& session);
        virtual ~ItookiAswReceiver();

    protected:

        std::string m_number;
        std::string m_message;
        
        EReturnCode postAsw(map<string, long long> parameters, const vector<string> &pathElements, const string &sRequest, string &responseMsg);
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, std::string &responseMsg);
        void operationOnAsw(Wt::Dbo::ptr<Echoes::Dbo::MessageTrackingEvent> MsgTrEv);

};


#endif	/* ASWRECEIVER_H */

