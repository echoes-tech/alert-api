/* 
 * API PluginRessource
 * @author ECHOES Technologies (GDR)
 * @date 10/02/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */


#include "PluginRessource.h"


using namespace std;

PluginRessource::PluginRessource(){
}


void PluginRessource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
     // Create Session and Check auth
   // PublicApiResource::handleRequest(request, response);
    session = new Session(Utils::connection);
    // set Content-Type
    response.setMimeType("application/json; charset=utf-8");
    
  /*  if (!this->authentified) {
        response.setStatus(401);
        response.out() << "{\"message\":\"Authentification Failed\"}";
        return;
    }*/
  
    
  // URL path after /alert
    string path = request.pathInfo();

    vector< string > splitPath;
    boost::split(splitPath, path, boost::is_any_of("/"), boost::token_compress_on);

    try
    {
        this->plgId = boost::lexical_cast<int>(splitPath[1]);
    }
    catch(boost::bad_lexical_cast &)
    {
        response.setStatus(422);
        response.out() << "{\"message\":\"Validation Failed\"}";
        return;
    }
    
    if (this->plgId)
    {
        if (request.method() == "GET")
        {
            switch (splitPath.size()) 
            {
                case 3:
                    if (splitPath[2] == "information") // liste les info que l'on peut surveiller en fonction du plugin
                    {
                        try
                        {
                            Wt::Dbo::Transaction transaction(*this->session);
                            std::string queryStr = " SELECT inf FROM \"T_INFORMATION_INF\" inf "
                                                "WHERE \"INF_DISPLAY\" = TRUE AND \"PLG_ID_PLG_ID\" = " + boost::lexical_cast<std::string>(this->plgId) +
                                                " AND \"INF_DELETE\" IS NULL";

                            Wt::Dbo::Query<Wt::Dbo::ptr<Information2> > queryRes = session->query<Wt::Dbo::ptr<Information2> >(queryStr);

                            Wt::Dbo::collection<Wt::Dbo::ptr<Information2> > information = queryRes.resultList();
                            
                            if(information.size() > 0 )
                            {
                                for (Wt::Dbo::collection<Wt::Dbo::ptr<Information2> >::const_iterator i = information.begin(); i != information.end(); i++) 
                                {
                                    response.out() << "{\n\""
                                                   << "  \"inf_name\" : \"" <<(*i).get()->name << "\"\n\""
                                                   << "}\n";
                                }
                                response.setStatus(200);
                                transaction.commit();
                            }
                            else
                            {
                                response.setStatus(404);
                                response.out() << "{\"message\":\"Information not found\"}";
                                return;
                            }
                        }
                        catch (Wt::Dbo::Exception const& e) 
                        {
                            Wt::log("error") << e.what();
                            response.setStatus(503);
                            response.out() << "{\"message\":\"Service Unavailable\"}";
                            return;
                        }
                    }
                    
                    /// A bouger dans AssetRessource
                    else if(splitPath[2] == "plugin") // liste les plugin d'un asset
                    {
                        try
                        {
                            
                            Wt::Dbo::Transaction transaction(*this->session);
                            std::string queryStr = " SELECT plg FROM \"T_PLUGIN_PLG\" plg "
                                            "WHERE \"PLG_ID\" IN"
                                            "("
                                            "SELECT \"T_PLUGIN_PLG_PLG_ID\" FROM \"TJ_AST_PLG\" "
                                            "WHERE \"T_ASSET_AST_AST_ID\" = " +  boost::lexical_cast<std::string>(this->plgId) +
                                            ")"
                                            "AND \"PLG_DELETE\" IS NULL";

                            Wt::Dbo::Query<Wt::Dbo::ptr<Plugin> > queryRes = session->query<Wt::Dbo::ptr<Plugin> >(queryStr);

                            Wt::Dbo::collection<Wt::Dbo::ptr<Plugin> > plugin = queryRes.resultList();
                            
                            if(plugin.size() > 0 )
                            {
                                for (Wt::Dbo::collection<Wt::Dbo::ptr<Plugin> >::const_iterator i = plugin.begin(); i != plugin.end(); i++) 
                                {
                                    response.out() << "{\n\""
                                                   << "  \"plg_name\" : \"" <<(*i).get()->name << "\"\n\""
                                                   << "}\n";
                                }  
                                response.setStatus(200);
                            }
                            else
                            {
                                response.setStatus(404);
                                response.out() << "{\"message\":\"Plugin not found\"}";
                                return;
                            }
                            transaction.commit();
                            
                        }
                        catch (Wt::Dbo::Exception const& e) 
                        {
                            Wt::log("error") << e.what();
                            response.setStatus(503);
                            response.out() << "{\"message\":\"Service Unavailable\"}";
                            return;
                        }
                    }
                    ////
                    else
                    {
                        response.setStatus(422);
                        response.out() << "{\"message\":\"Validation Failed\"}";
                    }
                    break;
                default:
                    response.setStatus(422);
                    response.out() << "{\"message\":\"Validation Failed\"}";
                    break;
            }
        }
        else 
        {
            response.setStatus(405);
            response.out() << "{\"message\":\"Only GET method is allowed.\"}";
            return;
        }
    }
}


PluginRessource::~PluginRessource()
{
    beingDeleted();
}