#include <Wt/WServer>
#include <Wt/WResource>
#include <Wt/Http/Request>
#include <Wt/Http/Response>
//#include <Wt/WEnvironment>
//#include <Wt/WApplication>
//---------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <ctime>


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
            int i = 0;
            int nombre_aleatoire = 0;
            srand(time(NULL)); // initialisation de rand
            nombre_aleatoire = rand();

            std::ofstream file;
            std::string test;
            test = "/var/www/wt/post" + boost::lexical_cast<std::string>(nombre_aleatoire);
            
            file.open (test.c_str());
//            const Wt::WEnvironment& env = EchoesHome::instance()->environment();
            ParameterMap pValues = request.getParameterMap();
            std::map<std::string, ParameterValues>::iterator it;
            for (it = pValues.begin(); it != pValues.end(); ++it)
            {
                response.out() << (*it).first << " : " << (*it).second[0] << "\n";
                file << (*it).first << " : " << (*it).second[0] << "\n";
//                Wt::Dbo::field(a, (*itStrings).second, formatColumnName(*this, (*itStrings).first));
            } 
            
            file.close();
            
//            response.out() << env.getCookieValue("test");
            
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
        
//        Session::configureAuth();
        
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