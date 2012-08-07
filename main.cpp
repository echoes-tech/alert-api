#include <Wt/WServer>
#include <Wt/Http/Client>
#include <Wt/WLogger>
#include <Wt/Utils>
#include <Wt/WEnvironment>
#include <resources/RestSrReception.h>
//#include <EchoesHome.h>

#include <tools/Session.h>
#include <alert/Alert.h>
#include <alert/AlertTracking.h>

#include <user/User.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include <unistd.h>

//---------------------------------------------------------------------------

static Session session("hostaddr=127.0.0.1 port=5432 dbname=echoes user=echoes password=toto");

class Utils 
{
    public:
        template <class T>
        static bool checkId(Wt::Dbo::ptr<T> ptr)
        {
            bool res = false;
            if (ptr.id() != -1)
            {
                res = true;
            }
            return res;
        };
        
};

typedef std::vector<std::string> ParameterValues;
typedef std::map<std::string, ParameterValues> ParameterMap;

class PublicApiResource : public Wt::WResource
{
    public:
        virtual ~PublicApiResource()
        {
            beingDeleted();
        }

    protected:
        std::string login;
        Wt::WString password;
        const Wt::Auth::AuthService *authService;
        Wt::Auth::PasswordService *passService;
        Wt::Auth::PasswordVerifier *verifier;
        bool authentified;
        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
        {
            authentified = false;
//            Session::configureAuth();
            
            this->authService = new Wt::Auth::AuthService();
            
            this->passService = new Wt::Auth::PasswordService(*this->authService);
            
            this->verifier = new Wt::Auth::PasswordVerifier();
            this->verifier->addHashFunction(new Wt::Auth::BCryptHashFunction(7));

            
            passService->setAttemptThrottlingEnabled(true);
            
            passService->setVerifier(this->verifier);
            
            
            
            
            if (!request.getParameterValues("login").empty())
            {
                this->login = request.getParameterValues("login")[0];
            }
            if (!request.getParameterValues("password").empty())
            {
                this->password = request.getParameterValues("password")[0];
            }
            
            const Wt::WString pass = this->password;
            const Wt::Auth::User *usr;
            // new transaction
            {
                try
                {
                    Wt::Dbo::Transaction transaction(session);
                    Wt::Dbo::ptr<AuthInfo::AuthIdentityType> authIdType = session.find<AuthInfo::AuthIdentityType>().where("\"identity\" = ?").bind(this->login);
                    if (Utils::checkId<AuthInfo::AuthIdentityType>(authIdType))
                    {
                        
                        const std::string userId = boost::lexical_cast<std::string>(authIdType.get()->authInfo().get()->user().id());
                        usr = new Wt::Auth::User(userId, UserDatabase(session));
                        
                        Wt::Auth::PasswordHash *hash = new Wt::Auth::PasswordHash(authIdType.get()->authInfo().get()->passwordMethod(), 
                                                                                 authIdType.get()->authInfo().get()->passwordSalt(),
                                                                                 authIdType.get()->authInfo().get()->passwordHash());
                        
                        if (this->passService->verifier()->verify(pass,*hash))
                        {
                            usr->setAuthenticated(true);
                            this->authentified = true;
                            Wt::log("info") << usr->id() << " logged.";
                        }
                        else
                        {
                            usr->setAuthenticated(false);
                            Wt::log("info") << usr->id() << " failure number : " << usr->failedLoginAttempts();
                        }
                    }
                    else
                    {
                        Wt::log("error") << "[PUBLIC API] User not found";
                        //TODO error behavior
                    }
                }
                catch(Wt::Dbo::Exception const& e)
                {
                    Wt::log("error") << e.what();
                    //TODO : behaviour in error case
                }
//                Wt::log("info") << myPasswordServiceResource.verifyPassword(*usr,pass);
            }

            
       
        }
};



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
            
            // new transaction
            {
                try
                {
                    Wt::Dbo::Transaction transaction(session);
                    Wt::Dbo::ptr<AlertTracking> at = session.find<AlertTracking>().where("\"ATR_ACK_ID\" = ?").bind(messageId);
                    if (Utils::checkId<AlertTracking>(at))
                    {
                        at.modify()->receiverSrv = gateway;

                        AlertTrackingEvent *ate = new AlertTrackingEvent();
                        ate->alertTracking = at;
                        ate->value = eventReason;
                        ate->date = Wt::WDateTime::currentDateTime();

                        Wt::Dbo::ptr<AlertTrackingEvent> ptrAte = session.add(ate);
                    }
                    else
                    {
                        Wt::log("error") << "[SR] Alert tracking not found";
                        //TODO error behavior
                    }
                }
                catch(Wt::Dbo::Exception const& e)
                {
                    Wt::log("error") << e.what();
                    //TODO : behaviour in error case
                }
            }
            
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
        std::string alertId;
        std::string alertTrackingId;
        std::string number;
        std::string messageText;
        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
        {
            this->alertId = "";
            this->alertTrackingId = "";
            
            if (!request.getParameterValues("alertid").empty())
            {
                this->alertId = request.getParameterValues("alertid")[0];
            }
            if (!request.getParameterValues("alerttrackingid").empty())
            {
                this->alertTrackingId = request.getParameterValues("alerttrackingid")[0];
            }
            if (!request.getParameterValues("number").empty())
            {
                this->number = Wt::Utils::urlEncode(request.getParameterValues("number")[0]);
            }
            if (!request.getParameterValues("messageText").empty())
            {
                this->messageText = Wt::Utils::urlEncode(request.getParameterValues("messageText")[0]);
            }
            
//            this->alertTrackingId = request.getParameterValues("alerttrackingid")[0];
            
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
//            std::string apiAddress = "http://127.0.0.1:8080/fake";
            
            if (client->post(apiAddress,message)) 
            {
                Wt::log("info") << "[SMS] Message sent to API. Address : " << apiAddress;
                try
                {
                    Wt::Dbo::Transaction transaction(session);
                    Wt::Dbo::ptr<AlertTracking> at = session.find<AlertTracking>().where("\"ATR_ID\" = ?").bind(this->alertTrackingId);
                    if (Utils::checkId<AlertTracking>(at))
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
                        //TODO error behavior
                    }
                }
                catch(Wt::Dbo::Exception const& e)
                {
                    Wt::log("error") << e.what();
                    //TODO : behaviour in error case
                }
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
                    std::string ackId = "";
                    ackId = ptree.get<std::string>("NMGSMSMTResponse.TicketList.Ticket.IdTicket");
                    
                    Wt::log("info") << "[SMS][ACK] result code : " << resultCode;
                    Wt::log("info") << "[SMS][ACK] response : " << response.body();
                    
                    try
                    {
                        Wt::Dbo::Transaction transaction(session);
                        Wt::Dbo::ptr<AlertTracking> at = session.find<AlertTracking>().where("\"ATR_ID\" = ?").bind(this->alertTrackingId);
                        if (Utils::checkId<AlertTracking>(at))
                        {
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
                        }
                        else
                        {
                            Wt::log("error") << "[SMS] Alert tracking not found";
                            //TODO error behavior
                        }
                    }
                    catch(Wt::Dbo::Exception const& e)
                    {
                        Wt::log("error") << e.what();
                        //TODO : behaviour in error case
                    }
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

            Wt::Http::Client *client = new Wt::Http::Client(this);
            
            client->done().connect(boost::bind(&TestSrAPI::handle, this, _1, _2));
            
            
            Wt::Http::Message message;

            std::string bodyText = "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?><SR><GW>fr.netsizeonline.com</GW><P1>722564613</P1><P2></P2><P3>3</P3><P4>800</P4><P5>2012-07-27T14:06:44</P5><P6>echoesteMO-test-test</P6><PORT>38000</PORT></SR>";


            message.addBodyText(bodyText);
            message.setHeader("Content-Type", "text/plain");
            
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
                    Wt::log("error") << "Http::Client error: " << response.status();
                }
            }
        }
};


class FakeNetsize : public PublicApiResource
{
    public:
        virtual ~FakeNetsize()
        {
            beingDeleted();
        }

    protected:
        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
        {
            PublicApiResource::handleRequest(request, response);
            if (this->authentified)
            {
                response.out() << "papuche";
            }
            else
            {
                response.out() << "pas papuche";
            }
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
        PublicApiResource par;
        
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