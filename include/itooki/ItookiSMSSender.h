/* 
 * Header of Itooki SMS Sender
 * @author ECHOES Technologies (FPO)
 * @date 15/11/2012
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2012 BY ECHOES TECHNOLGIES SAS
 * 
 */

#ifndef ITOOKISMSSENDER_H
#define	ITOOKISMSSENDER_H

#include <Wt/Http/Client>
#include <Wt/Http/Request>
#include <Wt/Http/Response>
#include <Wt/Utils>
#include <Wt/WObject>
#include <Wt/WLogger>

#include <boost/algorithm/string.hpp>

#include <tools/SessionPool.h>

#include "Utils.h"

class ItookiSMSSender
{
    public:
        ItookiSMSSender(const std::string &number, const std::string &message, Wt::WObject *parent = 0);
        virtual ~ItookiSMSSender();

        void setAlertTrackingPtr(Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> alertTrackingPtr);

        std::string getMessage() const;
        std::string getNumber() const;
        Wt::WObject* getParent() const;
        Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> getAlertTrackingPtr() const;

        int send();

    private:
        std::string _number, _message;
        Wt::WObject *_parent;
        Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> _alertTrackingPtr;

        void setMessage(std::string message);
        void setNumber(std::string number);
        void setParent(Wt::WObject* parent);

        void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response);
};

#endif	/* ITOOKISMSSENDER_H */

