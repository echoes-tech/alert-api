/* 
 * Itooki SMS Sender
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

#include "itooki/ItookiSMSSender.h"

using namespace std;

ItookiSMSSender::ItookiSMSSender(const string &number, const string &message, Wt::WObject *parent)
{
    setNumber(number);
    setMessage(message);
    setParent(parent);
}

ItookiSMSSender::ItookiSMSSender(const ItookiSMSSender& orig)
{
    setNumber(orig.getMessage());
    setMessage(orig.getMessage());
    setParent(orig.getParent());
    setAlertTrackingPtr(orig.getAlertTrackingPtr());
}

int ItookiSMSSender::send()
{
    int res = -1;

    // HTTP Client init
    Wt::Http::Client *client = new Wt::Http::Client(_parent);
    client->done().connect(boost::bind(&ItookiSMSSender::handleHttpResponse, this, _1, _2));

    string apiAddress = "https://www.itooki.fr/http.php"
            "?email=contact@echoes-tech.com"
            "&pass=00SjmAuiitooki"
            "&numero=" + Wt::Utils::urlEncode(_number)
            + "&message=" + Wt::Utils::urlEncode(_message)
            + "&refaccus=o";

    Wt::log("info") << "[Itooki SMS Sender] Trying to send request to Itooki API. Address : " << apiAddress;
    if (client->get(apiAddress)) 
    {
        Wt::log("info") << "[Itooki SMS Sender] Message sent to Itooki API. Address : " << apiAddress;

        if(Utils::checkId<AlertTracking>(_alertTrackingPtr))
        {
            try
            {
                Wt::Dbo::Transaction transaction(*_alertTrackingPtr.session());
                //TODO : hostname cpp way
                char hostname[255];
                gethostname(hostname, 255);
                _alertTrackingPtr.modify()->senderSrv = hostname;
                _alertTrackingPtr.modify()->sendDate = Wt::WDateTime::currentDateTime();
                transaction.commit();
            } 
            catch (Wt::Dbo::Exception const& e) 
            {
                Wt::log("error") << "[Itooki SMS Sender] " << e.what();
            }
        }
        res = 0;
    } 
    else 
    {
        Wt::log("error") << "[Itooki SMS Sender] Failed to send message to Itooki API. Address : " << apiAddress;
    }

    return res;
}

void ItookiSMSSender::handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response) {
    if (!err)
    {
        if (response.status() == 200)
        {
            string resultCode = response.body();

            Wt::log("info") << "[SMS][ACK] result code : " << resultCode;
            vector< string > splitResult;
            boost::split(splitResult, resultCode, boost::is_any_of("-"), boost::token_compress_on);
            
            if (splitResult.size() != 2)
            {
                Wt::log("error") << "[SMS] Unexpected answer from itooki.";
            }
            
            if (splitResult.size() == 0)
            {
                Wt::log("error") << "[SMS] Unexpected answer from itooki, no result code.";
            }
            
            if (splitResult.size() == 2)
            {

                if (Utils::checkId<AlertTracking > (_alertTrackingPtr))
                {
                    try
                    {
                        Wt::Dbo::Transaction transaction(*_alertTrackingPtr.session());
                        _alertTrackingPtr.modify()->ackId = splitResult[1];
                        _alertTrackingPtr.modify()->ackGw = "itooki.fr";
                        _alertTrackingPtr.modify()->receiveDate = Wt::WDateTime::currentDateTime();

                        AlertTrackingEvent *ate = new AlertTrackingEvent();
                        ate->alertTracking = _alertTrackingPtr;
                        ate->date = Wt::WDateTime::currentDateTime();
                        ate->value = splitResult[0];
                        transaction.commit();

                    }
                    catch (Wt::Dbo::Exception const& e)
                    {
                        Wt::log("error") << e.what();
                        //TODO : behaviour in error case
                    }
                }
                else
                {
                    Wt::log("error") << "[SMS] Alert tracking not found";
                    //TODO error behavior
                }

            }
        }
        else
        {
            Wt::log("error") << "Http::Client error: " << err.message();
        }
    }
}

void ItookiSMSSender::setMessage(string message)
{
    _message = message;
    return;
}

std::string ItookiSMSSender::getMessage() const
{
    return _message;
}

void ItookiSMSSender::setNumber(string number)
{
    _number = number;
    return;
}

std::string ItookiSMSSender::getNumber() const
{
    return _number;
}

void ItookiSMSSender::setAlertTrackingPtr(Wt::Dbo::ptr<AlertTracking> alertTrackingPtr)
{
    _alertTrackingPtr = alertTrackingPtr;
    return;
}

Wt::Dbo::ptr<AlertTracking> ItookiSMSSender::getAlertTrackingPtr() const
{
    return _alertTrackingPtr;
}

void ItookiSMSSender::setParent(Wt::WObject* parent)
{
    _parent = parent;
    return;
}

Wt::WObject* ItookiSMSSender::getParent() const
{
    return _parent;
}

ItookiSMSSender::~ItookiSMSSender()
{
}

