//#include "EchoesHome.h"
//
//#include <Wt/WEnvironment>
//#include <Wt/WStackedWidget>
//#include <Wt/WMenuItem>
//#include <Wt/WTabWidget>
//#include <Wt/WText>
//#include <Wt/WTemplate>
//#include <Wt/WViewWidget>
//#include <Wt/WWidget>
//
//#include <Wt/Auth/AuthModel>
//#include <Wt/Auth/AuthWidget>
//
//
//
///**
//Class EchoesHome : étend la classe WApplication.
//@param env : variables d'environnement
//*/
//EchoesHome::EchoesHome(const WEnvironment& env)
//    : WApplication(env),
//    session_()
//{
//    this->session_.login().changed().connect(this, &EchoesHome::authEvent);
//
//    // Définition des ressources (template)
//    messageResourceBundle().use(appRoot() + "echoes-home", false);
//
//    // Initialisation de la page principale du site
//    root()->clear();
//    
//    Wt::Auth::AuthWidget *authWidget
//      = new Wt::Auth::AuthWidget(Session::auth(), session_.users(),
//				 session_.login());
//
//    authWidget->model()->addPasswordAuth(&Session::passwordAuth());
//    authWidget->model()->addOAuth(Session::oAuth());
//    authWidget->setRegistrationEnabled(true);
//
//    authWidget->processEnvironment();
//
//    root()->addWidget(authWidget);
//
//    // homePage_ = initHome();
//    //homePage_ = initTabs();
////    initAuthWidget();
//    
////    WText *title = new WText("<h1>ECHOES Alert</h1>");
////    root()->addWidget(title);
//
//
//    // ajout du widget principal à la racine du site
////    root()->addWidget(this->authWidget);
//}
//
///**
//Initialisation de l'application.
//@return WTemplate
//*/
//WWidget *EchoesHome::initHome()
//{
//    // on utilise un WTemplate qui permet de se baser sur un fichier XML pour générer les pages web. Fichier XML défini plus haut avec messageResourceBundle.
//    WTemplate *result = new WTemplate(tr("template"), root());
//    return result;
//}
//
///**
//Initialisation de l'application.
//@return WTemplate
//*/
//void *EchoesHome::initAuthWidget()
//{
////    Wt::Auth::AuthService *testAuthService = new Wt::Auth::AuthService();
//
////    this->authModel = new Wt::Auth::AuthModel(Session::auth() ,this->session_->users(), this);
//////    this->authModel = new Wt::Auth::AuthModel(Session::auth(),this->session_->users(), this);
////    this->authModel->addPasswordAuth(&Session::passwordAuth());
//////    this->authModel->addOAuth(Session::oAuth());
////
////    this->authWidget = new Wt::Auth::AuthWidget(this->session_->login());
////    this->authWidget->setModel(this->authModel);
////    this->authWidget->setRegistrationEnabled(true);
//    
//}
//
//
///**
//Initialisation de l'appli, test des tabs.
//@return WTabWidget
//*/
//WWidget *EchoesHome::initTabs()
//{
//    // on créée un contenair qui va être utilisé par le WTabWidget
//    WContainerWidget *result = new WContainerWidget();
//    tabList = new WTabWidget(AlignTop | AlignJustify, result);
//
//    // on créée une animation pour animer les transitions entre les tabs
//    WAnimation animation(WAnimation::SlideInFromRight, WAnimation::EaseIn);
//    tabList->contentsStack()->setTransitionAnimation(animation, true);
//
//    // on créée les différents tabs
//    tabList->addTab(createTab("home.examples.hello","hello"),tr("hello-world"))->setPathComponent("");
//    tabList->addTab(createTab("home.examples.chart","charts"),tr("charts"));
//
//    return result;
//}
//
///**
//Création d'un onglet.
//@param textKey la clé qui permet de retrouver le texte de l'onglet dans le template
//@param sourceDir
//@return le container qui contient le texte
//*/
//WWidget *EchoesHome::createTab(const char *textKey, const char *sourceDir)
//{
//    WContainerWidget *result = new WContainerWidget();
//
//    // texte contenu dans l'onglet
//    WText *w = new WText(tr(textKey), result);
//    w->setInternalPathEncoding(true);
//    return result;
//
//}
//
///**
//Méthode utilisée pour transformer une chaîne classique en objet WString. A utiliser en entrée des méthodes WT qui demandent une WString.
//@param key chaîne passée en entrée
//@return objet WString
//*/
//WString EchoesHome::tr(const char *key)
//{
//    return WString::tr(key);
//}
//
///**
//Point d'entrée
//@return EchoesHome
//*/
//WApplication *createEchoesHomeApplication(const WEnvironment& env)
//{
//    // On instancie la classe EchoesHome qui permet d'afficher le site.
//    return new EchoesHome(env);
//}
//
//void EchoesHome::handleInternalPath(const std::string &internalPath)
//{
////  if (session_->login().loggedIn()) {
////    if (internalPath == "/play")
////      doNothing();
//////    else if (internalPath == "/highscores")
//////      showHighScores();
////    else
////      WApplication::instance()->setInternalPath("/play",  true);
////  }
//}
//
//
//void EchoesHome::authEvent()
//{
//    if (session_.login().loggedIn())
//        Wt::log("notice") << "User " << session_.login().user().id()
//        << " logged in.";
//    else
//        Wt::log("notice") << "User logged out.";
//}