/**
 * @file main.cpp
 * @author Thomas Saquet, Florent Poinsaut
 * @date 
 * @brief File containing example of doxygen usage for quick reference.
 *
 * Alert - API is a part of the Alert software
 * Copyright (C) 2013-2017
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 * 
 */

#include <signal.h>

#include "Conf.h"

#include "PublicApiResource.h"

#include "AddonResource.h"
#include "AlertResource.h"
#include "MessageResource.h"
#include "AssetResource.h"
#include "CriterionResource.h"
#include "FilterResource.h"
#include "InformationResource.h"
#include "InformationDataResource.h"
#include "MediaResource.h"
#include "OptionResource.h"
#include "GroupResource.h"
#include "PluginResource.h"
#include "ProbeResource.h"
#include "RoleResource.h"
#include "SearchResource.h"
#include "SourceResource.h"
#include "UnitResource.h"
#include "UserResource.h"

#include "itooki/ItookiAckReceiver.h"
#include "itooki/ItookiAswReceiver.h"
#include "itooki/ItookiSendedReceiver.h"

#include "mail/mailAssign.h"
#include "mail/mailForward.h"
#include "mail/mailResolve.h"

#endif

using namespace std;

/**
 * Main function
 * @param argc Number of arguments passed to the program
 * @param argv Array of arguments passed to the program
 * @return Exit status
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

        conf.initConfFileName(argc, argv);
        if (conf.readProperties(server))
        {
            Echoes::Dbo::Session session(conf.getSessConnectParams());

            AddonResource           addonResource(session);
            AlertResource           alertResource(session);
            MessageResource         messageResource(session);
            AssetResource           assetResource(session);
            CriterionResource       criteriaResource(session);
            FilterResource          filterResource(session);
            InformationResource     informationRessource(session);
            InformationDataResource informationDataRessource(session);
            MediaResource           mediaResource(session);
            OptionResource          optionRessource(session);
            GroupResource           groupRessource(session);
            PluginResource          pluginResource(session);
            ProbeResource           probeResource(session);
            SearchResource          searchResource(session);
            SourceResource          sourceResource(session);
            UnitResource            unitResource(session);
            UserResource            userResource(session);
            RoleResource            roleResource(session);
            ItookiAckReceiver       itookiAckReceiver(session);
            ItookiAswReceiver       itookiAswReceiver(session);
            ItookiSendedReceiver    itookiSendedReceiver(session);
            MailAssign              mailAssign(session);
            MailForward             mailForward(session);
            MailResolve             mailResolve(session);

            server.addResource(&addonResource,            "/addons");
            server.addResource(&alertResource,            "/alerts");
            server.addResource(&messageResource,          "/messages");
            server.addResource(&assetResource,            "/assets");
            server.addResource(&criteriaResource,         "/criteria");
            server.addResource(&filterResource,           "/filters");
            server.addResource(&informationRessource,     "/informations");
            server.addResource(&informationDataRessource, "/information_datas");
            server.addResource(&mediaResource,            "/medias");
            server.addResource(&optionRessource,          "/options");
            server.addResource(&groupRessource,           "/groups");
            server.addResource(&pluginResource,           "/plugins");
            server.addResource(&probeResource,            "/probes");
            server.addResource(&roleResource,             "/roles");
            server.addResource(&searchResource,           "/searches");
            server.addResource(&sourceResource,           "/sources");
            server.addResource(&unitResource,             "/units");
            server.addResource(&userResource,             "/users");
            server.addResource(&itookiAckReceiver,        "/itooki/ack");
            server.addResource(&itookiAswReceiver,        "/itooki/asw");
            server.addResource(&itookiSendedReceiver,     "/itooki/sended");
            server.addResource(&mailAssign,               "/mail/assign");
            server.addResource(&mailForward,               "/mail/forward");
            server.addResource(&mailResolve,               "/mail/resolve");

            Echoes::Dbo::Session::configureAuth();
            
            Wt::WString::setDefaultEncoding(Wt::CharEncoding::UTF8);

            
            // démarrage du serveur en arrière plan
            if (server.start())
            {
                //obligé de setter le port dans la conf à cet endroit car il faut que le server soit start
                conf.setServerPort(server.httpPort()); 
                // méthode qui bloque le thread courant en attendant le signal d'exctinction
                int sig = Wt::WServer::waitForShutdown(argv[0]);

                Wt::log("info") << "[Main] Shutdown (signal = " << sig << ")";

                server.stop();

                if (sig == SIGHUP)
                {
                    Wt::WServer::restart(argc, argv, environ);
                }

                res = EXIT_SUCCESS;
            }
        }
        else
        {
            Wt::log("fatal") << "[Main] Every properties are not correctly set in " << conf.getConfFileName();
        }
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
