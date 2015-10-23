/* 
 * Header of Itooki SMS Sender
 * @author ECHOES Technologies (FPO)
 * @date 15/11/2012
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2012-2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef ITOOKISMSSENDER_H
#define	ITOOKISMSSENDER_H

#include "PublicItookiResource.h"

class ItookiSMSSender : public PublicItookiResource
{
    public:
        ItookiSMSSender(Echoes::Dbo::Session& session);
        virtual ~ItookiSMSSender();

        Wt::WObject* getParent() const;

        int send(const std::string &number, const std::string &message, const long long alertID, Wt::Dbo::ptr<Echoes::Dbo::Message> atrPtr);

        int send(const std::string &number, const std::string &message, Wt::Dbo::ptr<Echoes::Dbo::Message> atrPtr);
        std::string urlEncodeMessage(const std::string);
        
    private:
        Wt::WObject *m_parent;

        void setParent(Wt::WObject* parent);

        void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response, const long long atrId);
};

#endif	/* ITOOKISMSSENDER_H */

