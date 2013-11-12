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

#ifndef main

#include <signal.h>

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
#include "RoleResource.h"
#include "AlertResource.h"
#include "OrganizationResource.h"

#include "ItookiAckReceiver.h"
#include "ItookiAswReceiver.h"

#endif

using namespace std;

/**
Point d'entrée du programme.
@param argc : TBC
@param argv : TBC
*/
int main(int argc, char **argv)
{
    #ifndef main
    int res = EXIT_FAILURE;
    
    try
    {
        // use argv[0] as the application name to match a suitable entry
        // in the Wt configuration file
        Wt::WServer server(argv[0]);

        // On définit la configuration du serveur en lui passant les paramètres d'entrée et son fichier de configuration
        server.setServerConfiguration(argc, argv);

        if (conf.readProperties(server))
        {
            AddonResource        addonResource;
            AlertResource        alertResource;
            AssetResource        assetResource;
            CriterionResource    criteriaResource;
            InformationResource  informationRessource;
            MediaResource        mediaResource;
            OrganizationResource organizationRessource;
            PluginResource       pluginResource;
            ProbeResource        probeResource;
            SearchTypeResource   searchTypeResource;
            UnitResource         unitResource;
            UserResource         userResource;
            RoleResource         roleResource;
            ItookiAckReceiver    itookiAckReceiver;
            ItookiAswReceiver    itookiAswReceiver;

            server.addResource(&searchTypeResource, "/search_types");
            server.addResource(&addonResource, "/addons");
            server.addResource(&criteriaResource, "/criteria");
            server.addResource(&unitResource, "/units");
            server.addResource(&organizationRessource, "/organizations");
            server.addResource(&informationRessource, "/informations");
            server.addResource(&mediaResource, "/medias");
            server.addResource(&userResource, "/users");
            server.addResource(&roleResource, "/roles");
            server.addResource(&alertResource, "/alerts");
            server.addResource(&assetResource, "/assets");
            server.addResource(&probeResource, "/probes");
            server.addResource(&pluginResource, "/plugins");
            server.addResource(&itookiAckReceiver, "/itooki/ack");
            server.addResource(&itookiAswReceiver, "/itooki/asw");

            Echoes::Dbo::Session::configureAuth();

            // démarrage du serveur en arrière plan
            if (server.start())
            {
                // méthode qui bloque le thread courant en attendant le signal d'exctinction
                int sig = Wt::WServer::waitForShutdown(argv[0]);

                Wt::log("info") << "[Main] Shutdown (signal = " << sig << ")";

                server.stop();

                if (sig == SIGHUP)
                    Wt::WServer::restart(argc, argv, environ);

                res = EXIT_SUCCESS;
            }
        }
        else
            Wt::log("fatal") << "[Main] Every properties are not correctly set in " << WT_CONFIG_XML;
    }
    catch (Wt::WServer::Exception& e)
    {
        cerr << e.what() << endl;
    }
    catch (exception& e)
    {
        cerr << "exception: " << e.what() << endl;
    }
    return res;
    
    #else
    return 0;
    #endif
}

