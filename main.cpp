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

#include <boost/thread/thread.hpp>
#include <Wt/WServer>
#include <tools/SessionPool.h>

#include "Conf.h"

#include "PublicApiResource.h"

#include "SearchTypeResource.h"
#include "AddonResource.h"
#include "CriterionResource.h"
#include "UnitResource.h"
#include "AssetResource.h"
#include "ProbeResource.h"
#include "InformationResource.h"
#include "MediaResource.h"
#include "PluginResource.h"
#include "UserResource.h"
#include "AlertResource.h"
#include "OrganizationResource.h"

#include "ItookiAckReceiver.h"
#include "ItookiAswReceiver.h"

#include "SendSMS.h"
#include "SrReception.h"

using namespace std;

string Utils::connection;

SessionPool* SessionPool::instance = 0;
string SessionPool::credentials = "";
boost::mutex SessionPool::mutex;

/**
Point d'entrée du programme.
@param argc : TBC
@param argv : TBC
*/
int main(int argc, char **argv)
{
    Conf *conf = new Conf();

    Utils::connection = conf->getSessConnectParams();

    try
    {
        SearchTypeResource searchTypeResource;
        AddonResource addonResource;
        CriterionResource criteriaResource;
        UnitResource unitResource;
        OrganizationResource organizationRessource;
        InformationResource informationRessource;
        MediaResource  mediaResource;
        PluginResource pluginResource;
        UserResource   userResource;
        AlertResource  alertResource;
        AssetResource  assetResource;
        ProbeResource  probeResource;
        // NetSize
//        SrReception     receiveSr;
//        SendSMS         sendSMS;
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
        
        server.addResource(&searchTypeResource, "/search_types");
        server.addResource(&addonResource, "/addons");
        server.addResource(&criteriaResource, "/criteria");
        server.addResource(&unitResource, "/units");
        server.addResource(&organizationRessource, "/organizations");
        server.addResource(&informationRessource, "/informations");
        server.addResource(&mediaResource, "/medias");
        server.addResource(&userResource, "/users");
        server.addResource(&alertResource, "/alerts");
        server.addResource(&assetResource, "/assets");
        server.addResource(&probeResource, "/probes");
        server.addResource(&pluginResource, "/plugins");
//        server.addResource(&receiveSr, "/netsize/sr");
//        server.addResource(&sendSMS, "/netsize/send");
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
