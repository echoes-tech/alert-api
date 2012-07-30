#include <Wt/WServer>
#include <Wt/Http/Client>
#include <Wt/WLogger>
#include <Wt/Utils>
#include <resources/RestSrReception.h>
//#include <EchoesHome.h>

#include <tools/Session.h>
#include <alert/Alert.h>
#include <alert/AlertTracking.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

//---------------------------------------------------------------------------

typedef std::vector<std::string> ParameterValues;
typedef std::map<std::string, ParameterValues> ParameterMap;

class RestSrReception : public Wt::WResource
{
    public:
        virtual ~RestSrReception()
        {
            beingDeleted();
        }

    protected:
        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
        {
            Wt::log("notice") << "[SR] Client address : " << request.clientAddress();
            Wt::log("notice") << "[SR] Content type : " << request.contentType();
            Wt::log("notice") << "[SR] Content length : " << request.contentLength();
            Wt::log("notice") << "[SR] Url scheme : " << request.urlScheme();
            Wt::log("notice") << "[SR] Query string : " << request.queryString();
            Wt::log("notice") << "[SR] X-Forwarded-For : " << request.headerValue("X-Forwarded-For");
            
//            try
//            {
//                Wt::log("notice") << "[SR] coucou : " << request.getParameterValues("coucou")[0];
//            }
//            catch(std::exception const& e)
//            {
//                Wt::log("notice") << "[SR] no paramater";
//            }
            
            /** string used to get the xml file sent in the request */
            std::string requestBodyString = "";
                       
            /** getting the input stream for the request char by char */
            char c;
            c = request.in().get();
            while (request.in())
            {
                requestBodyString.append(1,c);
                c = request.in().get();
            }
            
            Wt::log("notice") << "[SR] " << requestBodyString;
            
            /** stream : mandatory for boost ptree */
            std::istringstream requestBodyStream(requestBodyString);
            
            /** publishing status=0 on the web page as result : netsize compliant */
            response.out() << "status=0";
                        
            /** boost ptree to parse the xml */
            boost::property_tree::ptree ptree;
            boost::property_tree::xml_parser::read_xml(requestBodyStream, ptree, boost::property_tree::xml_parser::trim_whitespace);
            
            /** everything we need in the xml */
            std::string gateway = "";
            std::string messageId = "";
            std::string messageStatus = "";
            std::string eventReason = "";
            std::string notificationDate = "";
            std::string port = "";
            
            /** try catch to handle a corrupted file */
            try
            {
                gateway = ptree.get<std::string>("SR.GW");
                messageId = ptree.get<std::string>("SR.P1");
                messageStatus = ptree.get<std::string>("SR.P3");
                eventReason = ptree.get<std::string>("SR.P4");
                notificationDate = ptree.get<std::string>("SR.P5");
                port = ptree.get<std::string>("SR.PORT");
            }
            catch(std::exception const& e)
            {
                std::cout << e.what() << std::endl;
                //TODO : behaviour in error case
                return;
            }
            
            
            Wt::log("notice") << "[SR] " << "gateway : " << gateway;
            Wt::log("notice") << "[SR] " << "messageId : " << messageId;
            Wt::log("notice") << "[SR] " << "messageStatus : " << messageStatus;
            Wt::log("notice") << "[SR] " << "eventReason : " << eventReason;
            Wt::log("notice") << "[SR] " << "notificationDate : " << notificationDate;
            Wt::log("notice") << "[SR] " << "port : " << port;
            
        }
};

class SendSMS : public Wt::WResource
{
    public:
        virtual ~SendSMS()
        {
            beingDeleted();
        }

    protected:
        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
        {
//            std::map<std::string,std::ParameterValues>::iterator itRequest;
//            
//            for(itRequest = request.getParameterMap().begin(); itRequest != request.getParameterMap().end(); ++itRequest)
//            {
//                
//            }
            
            Wt::Http::Client *client = new Wt::Http::Client(this);
            
            client->done().connect(boost::bind(&SendSMS::handle, this, _1, _2));
            
            //          curl_easy_setopt(curl, CURLOPT_URL, "https://Fr.netsizeonline.com:8443/Request.aspx");
            //          curl_easy_setopt(curl, CURLOPT_URL, "http://Fr.netsizeonline.com:8080/Request.aspx");
            //          curl_easy_setopt(curl, CURLOPT_URL, "https://sms.echoes-tech.com/netsize/sr");
            //          curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:8080/sr");
            
            
            

            std::string bodyText = "sEndpointName=NSGClientMT%5EEchoesteMO-test-0132892&sExtensionName="
                            "AGSMSExt%5ENetsize&sLogin=echoesteMO-test-test&sPassword=lngA8ylp&sTarget=%2B33687693700&sMessage="
                            "test_sr_xml&nNotification=7&Output=XMLLongTags";
            
            
            /** Message containing the http parameters and the body of the post request */
            Wt::Http::Message message;
            message.addBodyText(bodyText);
            message.setHeader("Content-Type", "application/x-www-form-urlencoded");

            
            
//            std::string apiAddress = "https://Fr.netsizeonline.com:8443/Request.aspx";
            std::string apiAddress = "http://127.0.0.1:8080/fake";
            
            if (client->post(apiAddress,message)) 
            {
                Wt::log("info") << "[SMS] Message sent to API. Address : " << apiAddress;
            }
            else
            {
                 Wt::log("error") << "[SMS] Failed to send message to API. Address : " << apiAddress;
            }
            
            
        }
        
        void handle(boost::system::error_code err, const Wt::Http::Message& response)
        {
            if (!err) 
            {
                if (response.status() == 200) 
                {
                    /** stream : mandatory for boost ptree */
                    std::istringstream requestBodyStream(response.body());
                    boost::property_tree::ptree ptree;
                    boost::property_tree::xml_parser::read_xml(requestBodyStream, ptree, boost::property_tree::xml_parser::trim_whitespace);
                    
                    std::string resultCode = "";
                    resultCode = ptree.get<std::string>("NMGSMSMTResponse.Result.Code");
                    
                    Wt::log("info") << "[SMS][ACK] result code : " << resultCode;
                    
                    
                } 
                else 
                {
                    Wt::log("error") << "Http::Client error: " << err.message();
                }
            }
        }
};


class TestSrAPI : public Wt::WResource
{
    public:
        virtual ~TestSrAPI()
        {
            beingDeleted();
        }

    protected:
        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
        {
//            std::map<std::string,std::ParameterValues>::iterator itRequest;
//            
//            for(itRequest = request.getParameterMap().begin(); itRequest != request.getParameterMap().end(); ++itRequest)
//            {
//                
//            }
            
            Wt::Http::Client *client = new Wt::Http::Client(this);
            
            client->done().connect(boost::bind(&TestSrAPI::handle, this, _1, _2));
            
            
            Wt::Http::Message message;
//            std::string bodyText = Wt::Utils::urlEncode("coucou=pouet&toto=gni");

            //exemple netsize
            //            "sEndpointName=NSGClientMT%5EEchoesteMO-test-0132892&sExtensionName="
            //                "AGSMSExt%5ENetsize&sLogin=echoesteMO-test-test&sPassword=lngA8ylp&sTarget=%2B33687693700&sMessage="
            //                "test_sr_full_req&nNotification=7&Output=XMLLongTags"
            std::string bodyText = "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?><SR><GW>fr.netsizeonline.com</GW><P1>722069699</P1><P2></P2><P3>3</P3><P4>800</P4><P5>2012-07-27T14:06:44</P5><P6>echoesteMO-test-test</P6><PORT>38000</PORT></SR>";
//            coucou%3dpouet%26toto%3dgni
            

            
//            Wt::log("notice") << bodyLength;
            message.addBodyText(bodyText);
            message.setHeader("Content-Type", "text/plain");

            
//            Wt::log("notice") << "ctype : " << message.getHeader("Content-Type")->c_str();
//            Wt::log("notice") << "clength : " << message.getHeader("Content-Length")->c_str();
            
            std::string apiAddress = "http://127.0.0.1:8080/sr";
            
            if (client->post(apiAddress,message)) 
            {
                Wt::log("info") << "[SMS] Message sent to API. Address : " << apiAddress;
            }
            else
            {
                 Wt::log("error") << "[SMS] Failed to send message to API. Address : " << apiAddress;
            }
            
            
        }
        
        void handle(boost::system::error_code err, const Wt::Http::Message& response)
        {
            if (!err) 
            {
                if (response.status() == 200) 
                {
                    std::cout << "REPONSE : " << response.body().c_str();
                } 
                else 
                {
                    Wt::log("error") << "Http::Client error: " << err.message();
                }
            }
        }
};


class FakeNetsize : public Wt::WResource
{
    public:
        virtual ~FakeNetsize()
        {
            beingDeleted();
        }

    protected:
        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
        {
            response.out() << "<?xml version=""1.0"" encoding=""UTF-8""?>"
                                "<NMGSMSMTResponse>"
                                "<Result>"
                                "<Code>0</Code>"
                                "<Message>Success</Message>"
                                "</Result>"
                                "<TicketList>"
                                "<Ticket>"
                                "<StatusResultCode>0</StatusResultCode>"
                                "<StatusResultMessage></StatusResultMessage>"
                                "<Address>+33687693700</Address>"
                                "<IdTicket>722564613</IdTicket>"
                                "<MessageUID>10005001BY72F9</MessageUID>"
                                "</Ticket>"
                                "</TicketList>"
                                "</NMGSMSMTResponse>";
        }

};

/**
Point d'entrée du programme.
@param argc : TBC
@param argv : TBC
*/
int main(int argc, char **argv)
{
    try
    {
        RestSrReception receiveSr;
        SendSMS         sendSMS;
        TestSrAPI       testAPI;
        FakeNetsize     fakeN;
        
        // On passe le premier paramètre d'entrée au serveur
        Wt::WServer server(argv[0]);
        // On définit la configuration du serveur en lui passant les paramètres d'entrée et son fichier de configuration
        server.setServerConfiguration(argc, argv);
        // On fixe le point d'entrée du programme (type de point d'entée, méthode à appeler, uri, chemin favicon)
//        server.addEntryPoint(Wt::Application, createEchoesHomeApplication,"", "/favicon.ico");
     
        server.addResource(&receiveSr, "/sr");
        server.addResource(&sendSMS, "/send");
        server.addResource(&testAPI, "/test");
        server.addResource(&fakeN, "/fake");
        
        Session::configureAuth();
        
        // démarrage du serveur en arrière plan
        if (server.start())
        {
            // méthode qui bloque le thread courant en attendant le signal d'exctinction
            Wt::WServer::waitForShutdown();
            server.stop();
        }
    }
    catch (Wt::WServer::Exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (std::exception &e)
    {
        std::cerr << "exception: " << e.what() << std::endl;
    }
}