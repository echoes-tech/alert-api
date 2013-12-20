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

#include <boost/algorithm/string.hpp>

#include <Wt/Http/Client>
#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include <Wt/Utils>
#include <Wt/WObject>
#include <Wt/WLogger>

#include <tools/Session.h>
#include <tools/SafeTransaction.h>

#include "Conf.h"

class ItookiSMSSender
{
    public:
        ItookiSMSSender(Wt::WObject *parent = NULL);
        virtual ~ItookiSMSSender();

        Wt::WObject* getParent() const;

        int send(const std::string &number, const std::string &message, Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> atrPtr);

    private:
        Wt::WObject *m_parent;

        void setParent(Wt::WObject* parent);

        void handleHttpResponse(Wt::Http::Client *client, boost::system::error_code err, const Wt::Http::Message& response, const long long atrId);
};

#endif	/* ITOOKISMSSENDER_H */

