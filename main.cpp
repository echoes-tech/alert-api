#include <Wt/WServer>
#include <resources/RestSrReception.h>
//#include <EchoesHome.h>

#include <tools/Session.h>

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
        bool debug = false;
        virtual void handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
        {
            int nombre_aleatoire = 0;
            srand(time(NULL)); // initialisation de rand
            nombre_aleatoire = rand();

            std::ofstream file;
            std::string test;
            if (debug)
            {
                test = "/home/tsa/dev/netsize/post" + boost::lexical_cast<std::string>(nombre_aleatoire);
            }
            else
            {
                test = "/var/www/wt/post" + boost::lexical_cast<std::string>(nombre_aleatoire);
            }
            
            file.open (test.c_str());
//            const Wt::WEnvironment& env = EchoesHome::instance()->environment();
            ParameterMap pValues = request.getParameterMap();
            std::map<std::string, ParameterValues>::iterator it;
//            response.out() << "Client address : " << request.clientAddress() << "\n";
//            response.out() << "Content type : " << request.contentType() << "\n";
//            response.out() << "Url scheme : " << request.urlScheme() << "\n";
//            response.out() << "Query string : " << request.queryString() << "\n";
            
            if (debug)
            {
                file << "Client address : " << request.clientAddress() << "\n";
                file << "Content type : " << request.contentType() << "\n";
                file << "Url scheme : " << request.urlScheme() << "\n";
                file << "Query string : " << request.queryString() << "\n";
                file << "X-Forwarded-For : " << request.headerValue("X-Forwarded-For") << "\n";
                file << "buffer : \n";
            }
            
            char c;
            c = request.in().get();
            while (request.in())
            {
                file << c;
                c = request.in().get();
            }            

            response.out() << "status=0";
                        
            file.close();
            
            
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
        
        // On passe le premier paramètre d'entrée au serveur
        Wt::WServer server(argv[0]);
        // On définit la configuration du serveur en lui passant les paramètres d'entrée et son fichier de configuration
        server.setServerConfiguration(argc, argv);
        // On fixe le point d'entrée du programme (type de point d'entée, méthode à appeler, uri, chemin favicon)
        
        
//        server.addEntryPoint(Wt::Application, createEchoesHomeApplication,"", "/favicon.ico");
     
        server.addResource(&receiveSr, "/sr");
        
        Session::configureAuth();
        
//        receiveSr.setInternalPath("/sr");
      
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