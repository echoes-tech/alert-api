/* 
 * Header of Itooki SMS Acknowledgement Receiver
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

#ifndef ACKRECEIVER_H
#define	ACKRECEIVER_H

#include "PublicItookiResource.h"

class ItookiAckReceiver   : public PublicItookiResource
{
    public:
        ItookiAckReceiver(Echoes::Dbo::Session& session);
        virtual ~ItookiAckReceiver();

    protected:

        std::string m_number;
        std::string m_message;
        
        EReturnCode postAck(map<string, long long> parameters, const vector<string> &pathElements, const string &sRequest, string &responseMsg);
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, std::string &responseMsg);
};



#endif	/* ACKRECEIVER_H */

