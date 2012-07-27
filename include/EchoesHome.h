#ifndef ECHOESHOME_H_
#define ECHOESHOME_H_

#include <Wt/WApplication>
#include <Wt/WContainerWidget>
#include <Wt/WServer>

#include <Wt/Auth/AuthModel>
#include <Wt/Auth/AuthWidget>
#include <Wt/Auth/PasswordService>
#include <Wt/WAnchor>

#include <tools/Session.h>

typedef std::vector<std::string> ParameterValues;
typedef std::map<std::string, ParameterValues> ParameterMap;

class EchoesHome;

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


class EchoesHome : public Wt::WApplication 
{
public:

    EchoesHome(const Wt::WEnvironment& env)
    : Wt::WApplication(env),
    session_() 
    {
        session_.login().changed().connect(this, &EchoesHome::authEvent);

        useStyleSheet("css/style.css");


        Wt::Auth::AuthWidget *authWidget
                = new Wt::Auth::AuthWidget(Session::auth(), session_.users(),
                session_.login());

        authWidget->model()->addPasswordAuth(&Session::passwordAuth());
        authWidget->model()->addOAuth(Session::oAuth());
        authWidget->setRegistrationEnabled(true);

        authWidget->processEnvironment();
        
        Wt::WResource *sr = new RestSrReception();
        sr->setInternalPath(appRoot() + "sr");
        Wt::WAnchor *a;



        a = new Wt::WAnchor( sr , "Doc", root());
        
        a->setTarget(Wt::TargetSelf);

      
        
        
        Wt::log("notice") << "\nURL : " << sr->url() << "";
        
        //makeAbsoluteUrl(sr->url());
        
        
        

        root()->addWidget(authWidget);
    }

    void authEvent() {
        if (session_.login().loggedIn()) 
        {
            Wt::log("notice") << "User " << session_.login().user().id()
                    << " logged in.";
            setCookie("test", "value",600);
        }
        else 
        {
            Wt::log("notice") << "User logged out.";
        }
    }
    Session session_;
private:

};

//using namespace Wt;
//
//class EchoesHome : public WApplication {
//public:
//    EchoesHome(const WEnvironment& env);
//protected:
//    WString tr(const char *key);
//    WTabWidget *tabList;
//    //    PostgresConnector *pgc;
//    void handleInternalPath(const std::string &internalPath);
//
//    void authEvent();
//private:
//    Session session_;
//
//    //    WWidget *homePage_;
//    Wt::Auth::AuthWidget *authWidget;
//
//    Wt::Auth::AuthModel *authModel;
//    //    Wt::Auth::AuthWidget *authWidget;
//
//    WWidget *initHome();
//    void *initAuthWidget();
//    WWidget *initTabs();
//    WWidget *createTab(const char *textKey, const char *sourceDir);
//
//};

Wt::WApplication *createEchoesHomeApplication(const Wt::WEnvironment& env) 
{
    // On instancie la classe EchoesHome qui permet d'afficher le site.
    return new EchoesHome(env);
}

#endif // ECHOESHOME_H_
