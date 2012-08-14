/* 
 * File:   SendSMS.h
 * Author: tsa
 *
 * Created on 8 août 2012, 14:41
 */

#ifndef SENDSMS_H
#define	SENDSMS_H

#include "includeFile.h"

class SendSMS : public PublicApiResource {
public:

    virtual ~SendSMS() {
        beingDeleted();
    }

protected:
    std::string alertId;
    std::string alertTrackingId;
    std::string number;
    std::string messageText;

    virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response) {
        // Check auth
        PublicApiResource::handleRequest(request, response);

        if (!this->authentified) {
            Wt::log("error") << "[SMS] User not identified";
            return;
        }
        
        Wt::log("error") << "[SMS] User identified";
        
        Session session(Utils::connection);

        this->alertId = "";
        this->alertTrackingId = "";

        // if a parameter is missing, we set doRequest = false
        if (!request.getParameterValues("alertid").empty()) 
        {
            this->alertId = request.getParameterValues("alertid")[0];
        } else 
        {
            Wt::log("error") << "[SMS] Missing alert id"; 
            return;
        }
        if (!request.getParameterValues("alerttrackingid").empty()) 
        {
            this->alertTrackingId = request.getParameterValues("alerttrackingid")[0];
        } else 
        {
            Wt::log("error") << "[SMS] Missing alert tracking id"; 
            return;
        }
        if (!request.getParameterValues("number").empty()) 
        {
            this->number = Wt::Utils::urlEncode(request.getParameterValues("number")[0]);
        } else 
        {
            Wt::log("error") << "[SMS] Missing number"; 
            return;
        }
        if (!request.getParameterValues("messageText").empty()) 
        {
            this->messageText = Wt::Utils::urlEncode(request.getParameterValues("messageText")[0]);
        } else 
        {
            Wt::log("error") << "[SMS] Missing message"; 
            return;
        }

        try 
        {
            Wt::Dbo::Transaction transaction(session);
            Wt::Dbo::ptr<AlertTracking> at = session.find<AlertTracking > ().where("\"ATR_ID\" = ?").bind(this->alertTrackingId);
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
        client->done().connect(boost::bind(&SendSMS::handle, this, _1, _2));

        //          curl_easy_setopt(curl, CURLOPT_URL, "https://Fr.netsizeonline.com:8443/Request.aspx");
        //          curl_easy_setopt(curl, CURLOPT_URL, "http://Fr.netsizeonline.com:8080/Request.aspx");
        //          curl_easy_setopt(curl, CURLOPT_URL, "https://sms.echoes-tech.com/netsize/sr");
        //          curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:8080/sr");          


        std::string bodyText = "sEndpointName=NSGClientMT%5EEchoesteMO-test-0132892&sExtensionName="
                "AGSMSExt%5ENetsize&sLogin=echoesteMO-test-test&sPassword=lngA8ylp&sTarget=" + this->number + "&sMessage="
                + this->messageText + "&nNotification=7&Output=XMLLongTags";


        Wt::log("info") << bodyText;

        /** Message containing the http parameters and the body of the post request */
        Wt::Http::Message message;
        message.addBodyText(bodyText);
        message.setHeader("Content-Type", "application/x-www-form-urlencoded");

        std::string apiAddress = "https://Fr.netsizeonline.com:8443/Request.aspx";
        //                std::string apiAddress = "http://127.0.0.1:8080/fake";


        Wt::log("info") << "[SMS] Trying to send request to API. Address : " << apiAddress;
        if (client->post(apiAddress, message)) 
        {
            Wt::log("info") << "[SMS] Message sent to API. Address : " << apiAddress;
            try 
            {
                Wt::Dbo::Transaction transaction(session);
                Wt::Dbo::ptr<AlertTracking> at = session.find<AlertTracking > ().where("\"ATR_ID\" = ?").bind(this->alertTrackingId);
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

    void handle(boost::system::error_code err, const Wt::Http::Message& response) {
        if (!err) {
            if (response.status() == 200) {
                Session session(Utils::connection);
                /** stream : mandatory for boost ptree */
                std::istringstream requestBodyStream(response.body());
                boost::property_tree::ptree ptree;
                boost::property_tree::xml_parser::read_xml(requestBodyStream, ptree, boost::property_tree::xml_parser::trim_whitespace);

                std::string resultCode = "";
                resultCode = ptree.get<std::string > ("NMGSMSMTResponse.Result.Code");
                std::string ackId = "";
                ackId = ptree.get<std::string > ("NMGSMSMTResponse.TicketList.Ticket.IdTicket");

                Wt::log("info") << "[SMS][ACK] result code : " << resultCode;
                Wt::log("info") << "[SMS][ACK] response : " << response.body();

                try {
                    Wt::Dbo::Transaction transaction(session);
                    Wt::Dbo::ptr<AlertTracking> at = session.find<AlertTracking > ().where("\"ATR_ID\" = ?").bind(this->alertTrackingId);
                    if (Utils::checkId<AlertTracking > (at)) {
                        //TODO : hostname cpp way
                        char hostname[255];
                        gethostname(hostname, 255);
                        at.modify()->receiveDate = Wt::WDateTime::currentDateTime();
                        at.modify()->ackId = ackId;
                        //                                AlertTrackingEvent *ate = new AlertTrackingEvent();
                        //                                ate->alertTracking = at;
                        //                                ate->eventValue = resultCode;
                        //                                
                        //                                Wt::Dbo::ptr<AlertTrackingEvent> ptrAte = session.add(ate);
                    } else {
                        Wt::log("error") << "[SMS] Alert tracking not found";
                        //TODO error behavior
                    }
                } catch (Wt::Dbo::Exception const& e) {
                    Wt::log("error") << e.what();
                    //TODO : behaviour in error case
                }
            }
            else {
                Wt::log("error") << "Http::Client error: " << err.message();
            }
        }
    }
};

#endif	/* SENDSMS_H */

