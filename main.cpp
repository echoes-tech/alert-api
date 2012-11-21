/* 
 * API Asset Ressource
 * @author ECHOES Technologies (TSA)
 * @date 08/08/2012
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2012 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "includeFile.h"

using namespace std;

//class TestSrAPI : public Wt::WResource
//{
//    public:
//        virtual ~TestSrAPI()
//        {
//            beingDeleted();
//        }
//
//    protected:
//        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
//        {
//            Session session(Utils::connection);
//
//            Wt::Http::Client *client = new Wt::Http::Client(this);
//            
//            client->done().connect(boost::bind(&TestSrAPI::handle, this, _1, _2));
//            
//            
//            Wt::Http::Message message;
//
//            std::string bodyText = "<?xml version=\"1.0\" encoding=\"iso-8859-1\"?><SR><GW>fr.netsizeonline.com</GW><P1>722564613</P1><P2></P2><P3>3</P3><P4>800</P4><P5>2012-07-27T14:06:44</P5><P6>echoesteMO-test-test</P6><PORT>38000</PORT></SR>";
//
//
//            message.addBodyText(bodyText);
//            message.setHeader("Content-Type", "text/plain");
//            
//            std::string apiAddress = "http://127.0.0.1:8080/sr";
//            
//            if (client->post(apiAddress,message)) 
//            {
//                Wt::log("info") << "[SMS] Message sent to API. Address : " << apiAddress;
//            }
//            else
//            {
//                 Wt::log("error") << "[SMS] Failed to send message to API. Address : " << apiAddress;
//            }
//        }
//        
//        void handle(boost::system::error_code err, const Wt::Http::Message& response)
//        {
//            if (!err) 
//            {
//                if (response.status() == 200) 
//                {
//                    std::cout << "REPONSE : " << response.body().c_str();
//                } 
//                else 
//                {
//                    Wt::log("error") << "Http::Client error: " << response.status();
//                }
//            }
//        }
//};
//
//
//class FakeNetsize : public PublicApiResource
//{
//    public:
//        virtual ~FakeNetsize()
//        {
//            beingDeleted();
//        }
//
//    protected:
//        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
//        {
//            response.out() << "<?xml version=""1.0"" encoding=""UTF-8""?>"
//                                "<NMGSMSMTResponse>"
//                                "<Result>"
//                                "<Code>800</Code>"
//                                "<Message>Success</Message>"
//                                "</Result>"
//                                "<TicketList>"
//                                "<Ticket>"
//                                "<StatusResultCode>0</StatusResultCode>"
//                                "<StatusResultMessage></StatusResultMessage>"
//                                "<Address>+33687693700</Address>"
//                                "<IdTicket>722564613</IdTicket>"
//                                "<MessageUID>10005001BY72F9</MessageUID>"
//                                "</Ticket>"
//                                "</TicketList>"
//                                "</NMGSMSMTResponse>";
//        }
//};

string Utils::connection;

/**
Point d'entrée du programme.
@param argc : TBC
@param argv : TBC
*/
int main(int argc, char **argv)
{
    Conf *conf = new Conf();

    Utils::connection = "hostaddr=" + conf->getDbHost() + 
                     " port=" + boost::lexical_cast<string>(conf->getDbPort()) + 
                     " dbname=" + conf->getDbName() +
                     " user=" + conf->getDbUser() +
                     " password=" + conf->getDbPassword();

    try
    {
        AssetRessource  assetRessource;
        ProbeRessource  probeRessource;
        SrReception     receiveSr;
        SendSMS         sendSMS;
        ItookiSMSSender itookiSMSSender;
        ItookiAckReceiver itookiAckReceiver;
        ItookiAswReceiver itookiAswReceiver;
//        TestSrAPI       testAPI;
//        FakeNetsize     fakeN;
        
        // On passe le premier paramètre d'entrée au serveur
        Wt::WServer server(argv[0]);
        // On définit la configuration du serveur en lui passant les paramètres d'entrée et son fichier de configuration
        server.setServerConfiguration(argc, argv);
        // On fixe le point d'entrée du programme (type de point d'entée, méthode à appeler, uri, chemin favicon)
//        server.addEntryPoint(Wt::Application, createEchoesHomeApplication,"", "/favicon.ico");

        server.addResource(&assetRessource, "/asset");
        server.addResource(&probeRessource, "/probe");
        server.addResource(&receiveSr, "/netsize/sr");
        server.addResource(&sendSMS, "/netsize/send");
        server.addResource(&itookiSMSSender, "/itooki/sms/sender");
        server.addResource(&itookiAckReceiver, "/itooki/ack");
        server.addResource(&itookiAswReceiver, "/itooki/asw");
//        server.addResource(&testAPI, "/test");
//        server.addResource(&fakeN, "/fake");
        
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
        cerr << e.what() << endl;
    }
    catch (exception &e)
    {
        cerr << "exception: " << e.what() << endl;
    }
    
    delete conf;
}