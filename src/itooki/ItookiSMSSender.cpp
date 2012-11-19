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

ItookiSMSSender::ItookiSMSSender() {
}

ItookiSMSSender::ItookiSMSSender(const ItookiSMSSender& orig) {
}

void ItookiSMSSender::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);

    // set Content-Type
    response.setMimeType("application/json; charset=utf-8");

    if (!this->authentified) {
        Wt::log("error") << "[SMS] User not identified";
        return;
    }

    Wt::log("info") << "[SMS] User identified";

    this->alertId = "";
    this->alertTrackingId = "";

    // if a parameter is missing, we set doRequest = false
    if (!request.getParameterValues("alertid").empty()) 
    {
        this->alertId = request.getParameterValues("alertid")[0];
    }
    else 
    {
        Wt::log("error") << "[SMS] Missing alert id"; 
        return;
    }
    
    if (!request.getParameterValues("alerttrackingid").empty()) 
    {
        this->alertTrackingId = request.getParameterValues("alerttrackingid")[0];
    }
    else 
    {
        Wt::log("error") << "[SMS] Missing alert tracking id"; 
        return;
    }
    
    // Phone Number
    if (!request.getParameterValues("number").empty()) 
    {
        this->number = Wt::Utils::urlEncode(request.getParameterValues("number")[0]);
    }
    else 
    {
        Wt::log("error") << "[SMS] Missing number"; 
        return;
    }
    
    if (!request.getParameterValues("messageText").empty()) 
    {
        this->messageText = Wt::Utils::urlEncode(request.getParameterValues("messageText")[0]);
    }
    else 
    {
        Wt::log("error") << "[SMS] Missing message"; 
        return;
    }

    try 
    {
        Wt::Dbo::Transaction transaction(*this->session);
        Wt::Dbo::ptr<AlertTracking> at = this->session->find<AlertTracking > ().where("\"ATR_ID\" = ?").bind(this->alertTrackingId);
        if (Utils::checkId<AlertTracking > (at)) 
        {
            if (!at.get()->sendDate.isNull()) 
            {
                Wt::log("error") << "[SMS] Alert tracking already filled, SMS already sent";
                return;
            }
        } 
        else 
        {
            Wt::log("error") << "[SMS] Alert tracking not found";
            return;
        }
    } catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        return;
    }

    // HTTP Client init
    Wt::Http::Client *client = new Wt::Http::Client(this);
    client->done().connect(boost::bind(&ItookiSMSSender::handle, this, _1, _2));

    string apiAddress = "https://www.itooki.fr/http.php?email=contact@echoes-tech.com&pass=00SjmAuiitooki&numero=" + this->number
            + "&message=" + this->messageText
            + "&refaccus=o"
            ;

    Wt::log("info") << "[SMS] Trying to send request to API. Address : " << apiAddress;
    if (client->get(apiAddress)) 
    {
        Wt::log("info") << "[SMS] Message sent to API. Address : " << apiAddress;
        try 
        {
            Wt::Dbo::Transaction transaction(*this->session);
            Wt::Dbo::ptr<AlertTracking> at = this->session->find<AlertTracking > ().where("\"ATR_ID\" = ?").bind(this->alertTrackingId);
            if (Utils::checkId<AlertTracking > (at)) 
            {
                //TODO : hostname cpp way
                char hostname[255];
                gethostname(hostname, 255);
                at.modify()->senderSrv = hostname;
                at.modify()->sendDate = Wt::WDateTime::currentDateTime();
            } 
            else 
            {
                Wt::log("error") << "[SMS] Alert tracking not found";
            }
        } 
        catch (Wt::Dbo::Exception const& e) 
        {
            Wt::log("error") << "[SMS] " << e.what();
            return;
        }
    } 
    else 
    {
        Wt::log("error") << "[SMS] Failed to send message to API. Address : " << apiAddress;
        return;
    }
}

void ItookiSMSSender::handle(boost::system::error_code err, const Wt::Http::Message& response) {
    if (!err)
    {
        if (response.status() == 200)
        {
            Session session(Utils::connection);

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
                try
                {
                    Wt::Dbo::Transaction transaction(session);
                    Wt::Dbo::ptr<AlertTracking> at = session.find<AlertTracking > ().where("\"ATR_ID\" = ?").bind(this->alertTrackingId);
                    if (Utils::checkId<AlertTracking > (at))
                    {
                        at.modify()->ackId = splitResult[1];
                        at.modify()->ackGw = "itooki.fr";
                        at.modify()->receiveDate = Wt::WDateTime::currentDateTime();
                        
                        AlertTrackingEvent *ate = new AlertTrackingEvent();
                        ate->alertTracking = at;
                        ate->date = Wt::WDateTime::currentDateTime();
                        ate->value = splitResult[0];
                        
                    }
                    else
                    {
                        Wt::log("error") << "[SMS] Alert tracking not found";
                        //TODO error behavior
                    }
                }
                catch (Wt::Dbo::Exception const& e)
                {
                    Wt::log("error") << e.what();
                    //TODO : behaviour in error case
                }
            }
        }
        else
        {
            Wt::log("error") << "Http::Client error: " << err.message();
        }
    }
}

ItookiSMSSender::~ItookiSMSSender() {
    beingDeleted();
}

