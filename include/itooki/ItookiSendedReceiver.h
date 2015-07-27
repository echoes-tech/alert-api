/* 
 * File:   ItookiSendedReceiver.h
 * Author: cde
 *
 * Created on 10 juillet 2015, 10:35
 */

#ifndef ITOOKISENDEDRECEIVER_H
#define	ITOOKISENDEDRECEIVER_H

#include "PublicItookiResource.h"


class ItookiSendedReceiver   : public PublicItookiResource
{
    public:
        ItookiSendedReceiver(Echoes::Dbo::Session& session);
        virtual ~ItookiSendedReceiver();

    protected:

        std::string m_number;
        std::string m_message;
        
        EReturnCode postSended(map<string, long long> parameters, const vector<string> &pathElements, const string &sRequest, string &responseMsg);
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, std::string &responseMsg);

};

#endif	/* ITOOKISENDEDRECEIVER_H */

