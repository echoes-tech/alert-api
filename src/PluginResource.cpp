/* 
 * API PluginResource
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

#include "PluginResource.h"



using namespace std;

PluginResource::PluginResource(){
}

Wt::Dbo::ptr<Plugin> PluginResource::pluginIsAccessible(unsigned short& res, std::string& responseMsg) const
{
    Wt::Dbo::ptr<Plugin>  plgPtr;
    try
    {
     
        Wt::Dbo::Transaction transaction(*this->session);
        
        std::string queryStr = "SELECT plg FROM \"T_PLUGIN_PLG\" plg "
                                "WHERE \"PLG_ID\" IN "
                                "("
                                   "SELECT \"T_PLUGIN_PLG_PLG_ID\" FROM \"TJ_AST_PLG\" "
                                   "wHERE \"T_ASSET_AST_AST_ID\" IN "
                                        "("
                                        "SELECT \"AST_ID\" FROM \"T_ASSET_AST\" "
                                        "WHERE \"AST_ORG_ORG_ID\" = " + boost::lexical_cast<std::string>(session->user().get()->currentOrganization.id()) +
                                        " AND \"AST_DELETE\" IS NULL "
                                        ")"
                                    "AND \"T_PLUGIN_PLG_PLG_ID\" = " + boost::lexical_cast<std::string>(vPathElements[1]) + 
                                " )"
                                " AND \"PLG_DELETE\" IS NULL";
        
         Wt::Dbo::Query<Wt::Dbo::ptr<Plugin> > queryRes = session->query<Wt::Dbo::ptr<Plugin> >(queryStr);

         plgPtr = queryRes;
         res = 200;
         transaction.commit();
         if(!plgPtr)
         {
             responseMsg = "{\"message\":\" Not found\"}";
             res = 404;
         }
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
         return plgPtr;
}

unsigned short PluginResource::getPluginJSON(std::string& responseMsg) const
{
    unsigned short res = 500;
    int idx = 0, idx1 = 0, idx3 = 0;
    try
    {
        Wt::Dbo::Transaction transaction(*this->session);


        // on regarde si le plugin appartient à l'organisation
        std::string queryStr = "SELECT plg FROM \"T_PLUGIN_PLG\" plg "
                                "WHERE \"PLG_ID\" IN "
                                "("
                                   "SELECT \"T_PLUGIN_PLG_PLG_ID\" FROM \"TJ_AST_PLG\" "
                                   "wHERE \"T_ASSET_AST_AST_ID\" IN "
                                        "("
                                        "SELECT \"AST_ID\" FROM \"T_ASSET_AST\" "
                                        "WHERE \"AST_ORG_ORG_ID\" = " + boost::lexical_cast<std::string>(session->user().get()->currentOrganization.id()) +
                                        " AND \"AST_DELETE\" IS NULL "
                                        ")"
                                    "AND \"T_PLUGIN_PLG_PLG_ID\" = " + boost::lexical_cast<std::string>(vPathElements[1]) + 
                                " )" ;
        
         Wt::Dbo::Query<Wt::Dbo::ptr<Plugin> > queryRes = session->query<Wt::Dbo::ptr<Plugin> >(queryStr);

         Wt::Dbo::ptr<Plugin>  plgPtr = queryRes;
         
        if(plgPtr)
        {
            responseMsg = "{\n\t\"id\" : " + boost::lexical_cast<std::string>(vPathElements[1]) + ",\n";
            responseMsg += "\t\"name\" : \"" + plgPtr->name.toUTF8() + "\",\n";
            responseMsg += "\t\"desc\" : \"" + plgPtr->desc.toUTF8() + "\",\n";
            responseMsg += "\t\"idAsset\" : 0,\n";
            responseMsg += "\t\"source\" : [\n"; 
    
            Wt::Dbo::collection<Wt::Dbo::ptr<Source>> srcCollec = session->find<Source>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                                         .orderBy("\"SRC_ID\"");

            for (Wt::Dbo::collection<Wt::Dbo::ptr<Source> >::const_iterator i = srcCollec.begin(); i != srcCollec.end(); i++) 
            {
                responseMsg += "\t{\n";
                responseMsg += "\t\t\"id\" : " + boost::lexical_cast<std::string>(i->get()->pk.id) +",\n";
                responseMsg += "\t\t\"idAddon\" : " + boost::lexical_cast<std::string>(i->get()->addon.id()) +",\n";
                responseMsg += "\t\t\"params\" : {\n";
                
                Wt::Dbo::collection<Wt::Dbo::ptr<SourceParameterValue>> srcParamCollec = session->find<SourceParameterValue>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                                                                      .where("\"SRC_ID\" = ?").bind(boost::lexical_cast<std::string>(i->get()->pk.id));

                for (Wt::Dbo::collection<Wt::Dbo::ptr<SourceParameterValue> >::const_iterator i1 = srcParamCollec.begin(); i1 != srcParamCollec.end(); i1++) 
                {
                    Wt::Dbo::ptr<SourceParameter> srcParam = session->find<SourceParameter>().where("\"SRP_ID\" = ?").bind(i1->get()->pk.sourceParameter.id());
                    responseMsg += "\t\t\t\"" + boost::lexical_cast<std::string>(srcParam.get()->name) + "\": \""+ boost::lexical_cast<std::string>(i1->get()->value) +"\",\n";              
                }
                if(srcParamCollec.size() != 0)
                {
                    responseMsg.replace(responseMsg.size()-2, 1, "");
                }
                responseMsg += "\t\t},\n";
                responseMsg += "\t\t\"searches\": [\n";  
      
                Wt::Dbo::collection<Wt::Dbo::ptr<Search>> seaCollec = session->find<Search>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                                             .where("\"SRC_ID\" = ?").bind(boost::lexical_cast<std::string>(i->get()->pk.id))
                                                                                             .orderBy("\"SEA_ID\"");

                for (Wt::Dbo::collection<Wt::Dbo::ptr<Search> >::const_iterator i2 = seaCollec.begin(); i2 != seaCollec.end(); i2++) 
                {
                    responseMsg += "\t\t{\n";
                    responseMsg += "\t\t\t\"id\" : " + boost::lexical_cast<std::string>(i2->get()->pk.id) +",\n";
                    responseMsg += "\t\t\t\"idType\" : " + boost::lexical_cast<std::string>(i2->get()->searchType.id()) +",\n";
                    responseMsg += "\t\t\t\"params\" : {\n";

                    Wt::Dbo::collection<Wt::Dbo::ptr<SearchParameterValue>> seaParamCollec = session->find<SearchParameterValue>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                                                                          .where("\"SRC_ID\" = ?").bind(boost::lexical_cast<std::string>(i->get()->pk.id))
                                                                                                                          .where("\"SEA_ID\" = ?").bind(boost::lexical_cast<std::string>(i2->get()->pk.id));

                    for (Wt::Dbo::collection<Wt::Dbo::ptr<SearchParameterValue> >::const_iterator i3 = seaParamCollec.begin(); i3 != seaParamCollec.end(); i3++) 
                    {
                        Wt::Dbo::ptr<SearchParameter> seaParam = session->find<SearchParameter>().where("\"SEP_ID\" = ?").bind(i3->get()->searchParameterValueId.searchParameter.id());
                        responseMsg += "\t\t\t\t\"" + boost::lexical_cast<std::string>(seaParam.get()->name) + "\": \""+ boost::lexical_cast<std::string>(i3->get()->value) +"\",\n";           
                    }
                    if(seaParamCollec.size() != 0)
                    {
                        responseMsg.replace(responseMsg.size()-2, 1, "");
                    }   
                    responseMsg += "\t\t\t},\n";
                    responseMsg += "\t\t\t\"period\" : " + boost::lexical_cast<std::string>(i2->get()->period) + ",\n";

                    responseMsg += "\t\t\t\"staticValues\" : " + boost::lexical_cast<std::string>(i2->get()->searchIsStatic) +"\n"; 
                    responseMsg += "\t\t},\n";
                }
                if(seaCollec.size() != 0)
                {
                    responseMsg.replace(responseMsg.size()-2, 1, "");
                }
                responseMsg += "\t\t]\n\t},\n";   
            } 
            if(srcCollec.size() != 0)
            {
                responseMsg.replace(responseMsg.size()-2, 1, "");
            }
            responseMsg += "\t]\n}";               
            res = 200;
            transaction.commit();
        }
        else
        {
            res = 404;
            responseMsg = "{\"message\":\"Plugin not found\"}";
        }
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;  
}

unsigned short PluginResource::getPlugin(std::string& responseMsg) const
{
    unsigned short res = 500;
    int idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(*this->session);

        //Wt::Dbo::collection<Wt::Dbo::ptr<Plugin> > plgPtr = session->find<Plugin>();
        // on liste les plugins rattachés à des assets détenu par l'organization
        std::string queryStr = "SELECT plg FROM \"T_PLUGIN_PLG\" plg "
                                "WHERE \"PLG_ID\" IN "
                                "("
                                   "SELECT \"T_PLUGIN_PLG_PLG_ID\" FROM \"TJ_AST_PLG\" "
                                   "wHERE \"T_ASSET_AST_AST_ID\" IN "
                                        "("
                                        "SELECT \"AST_ID\" FROM \"T_ASSET_AST\" "
                                        "WHERE \"AST_ORG_ORG_ID\" = " + boost::lexical_cast<std::string>(session->user().get()->currentOrganization.id()) +
                                        " AND \"AST_DELETE\" IS NULL "
                                        ")"
                                ") "
                                " AND \"PLG_DELETE\" IS NULL"
                                " ORDER BY \"PLG_ID\" ";
        
         Wt::Dbo::Query<Wt::Dbo::ptr<Plugin> > queryRes = session->query<Wt::Dbo::ptr<Plugin> >(queryStr);

         Wt::Dbo::collection<Wt::Dbo::ptr<Plugin> > plgPtr = queryRes.resultList();

        
        if(plgPtr.size() > 0 )
        {
            responseMsg = "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Plugin> >::const_iterator i = plgPtr.begin(); i != plgPtr.end(); i++) 
            {
                i->modify()->setId(i->id());
                responseMsg +=  i->modify()->toJSON();
                idx++;
                if(plgPtr.size()-idx > 0)
                {
                    responseMsg += ",\n";
                }
            }
            responseMsg += "\n]\n";
            res = 200;
            transaction.commit();
        }
        else
        {
            res = 404;
            responseMsg = "{\"message\":\"Plugin not found\"}";
        }
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << "[PluginResource]" << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;  
}

unsigned short PluginResource::getInformationListForPlugin(std::string &responseMsg) const
{
    unsigned short res = 500;
    int idx = 0;
    if(!pluginIsAccessible(res, responseMsg))
    {
        return res;
    }
    try
    {
        
        Wt::Dbo::Transaction transaction(*this->session);

        Wt::Dbo::collection < Wt::Dbo::ptr<Information2> > information = session->find<Information2>()
                .where("\"PLG_ID_PLG_ID\" = ? AND \"INF_DELETE\" IS NULL")
                .bind(this->vPathElements[1])
               .orderBy("\"SRC_ID\", \"SEA_ID\", \"INF_VALUE_NUM\"");

        if(information.size() > 0 )
        {
            responseMsg = "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Information2> >::const_iterator i = information.begin(); i != information.end(); i++) 
            { 
                Information2 info(*i->get());
                responseMsg +=  info.toJSON();
                idx++;
                if(information.size()-idx > 0)
                {
                    responseMsg += ",\n";
                }
            }
            responseMsg += "\n]\n";
            res = 200;
            
        }
        else
        {
            res = 404;
            responseMsg = "{\"message\":\"Information not found\"}";
        }
        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;  
}

unsigned short PluginResource::getSearchForSourceAndPlugin(std::string& responseMsg) const
{
    unsigned short res = 500;
    if(!pluginIsAccessible(res, responseMsg))
    {
        return res;
    }
    int idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(*session);
        Wt::Dbo::collection<Wt::Dbo::ptr<Search>> seaCollec = session->find<Search>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                                     .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                                                                                     .where("\"SEA_DELETE\" IS NULL")
                                                                                     .orderBy("\"SEA_ID\"");           
        if (seaCollec.size() > 0)
        {
            responseMsg += "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Search> >::const_iterator i = seaCollec.begin(); i != seaCollec.end(); i++) 
            {
                responseMsg += "\t" + i->modify()->toJSON();
                ++idx;
                if(seaCollec.size()-idx > 0)
                {
                    responseMsg += ",\n";
                }
            }
            responseMsg += "\n]\n";               

            res = 200;
        }
        else 
        {
            res = 404;
            responseMsg = "{\"message\":\"Search not found\"}";
        }
        transaction.commit();
    }
    catch (Wt::Dbo::Exception const &e)
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;
}

unsigned short PluginResource::getParameterValueForSearch(std::string &responseMsg) const
{
    unsigned short res = 500;
    if(!pluginIsAccessible(res, responseMsg))
    {
        return res;
    }
    int idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(*session);
        Wt::Dbo::collection<Wt::Dbo::ptr<SearchParameterValue>> seaParamCollec = session->find<SearchParameterValue>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                                                                      .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                                                                                                                      .where("\"SEA_ID\" = ?").bind(this->vPathElements[5]);
        if (seaParamCollec.size() > 0)
        {
            responseMsg += "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<SearchParameterValue> >::const_iterator i = seaParamCollec.begin(); i != seaParamCollec.end(); i++) 
            {
                responseMsg += "\t" + i->modify()->toJSON();
                ++idx;
                if(seaParamCollec.size()-idx > 0)
                {
                    responseMsg += ",\n";
                }
            }
            responseMsg += "]\n";               

            res = 200;
        }
        else 
        {
            res = 404;
            responseMsg = "{\"message\":\"Parameter not found\"}";
        }
        transaction.commit();
    }
    catch (Wt::Dbo::Exception const &e)
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;
}

unsigned short PluginResource::getInformationForSeaSrcAndPlg(std::string& responseMsg) const
{
    unsigned short res = 500;
    if(!pluginIsAccessible(res, responseMsg))
    {
        return res;
    }
    int idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(*session);
        Wt::Dbo::collection<Wt::Dbo::ptr<Information2>> infCollec = session->find<Information2>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                                                 .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                                                                                                 .where("\"SEA_ID\" = ?").bind(this->vPathElements[5])
                                                                                                 .where("\"INF_DELETE\" IS NULL")
                                                                                                 .orderBy("\"INF_VALUE_NUM\"");                                                                                       
        if (infCollec.size() > 0)
        {
            responseMsg += "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Information2> >::const_iterator i = infCollec.begin(); i != infCollec.end(); i++) 
            {
                Information2 info(*i->get());
                responseMsg += "\t" + info.toJSON();
                ++idx;
                if(infCollec.size()-idx > 0)
                {
                    responseMsg += ",\n";
                }
            }
            responseMsg += "\n]\n";               

            res = 200;
        }
        else 
        {
            res = 404;
            responseMsg = "{\"message\":\"Information not found\"}";
        }
        transaction.commit();
    }
    catch (Wt::Dbo::Exception const &e)
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;
}

unsigned short PluginResource::getSourceForPlugin(std::string& responseMsg) const
{
    unsigned short res = 500;
    if(!pluginIsAccessible(res, responseMsg))
    {
        return res;
    }
    int idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(*session);
        Wt::Dbo::collection<Wt::Dbo::ptr<Source>> srcCollec = session->find<Source>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                                     .where("\"SRC_DELETE\" IS NULL")
                                                                                     .orderBy("\"SRC_ID\"");

        if (srcCollec.size() > 0)
        {
            responseMsg += "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Source> >::const_iterator i = srcCollec.begin(); i != srcCollec.end(); i++) 
            {
                responseMsg += "\t" + i->modify()->toJSON();
                ++idx;
                if(srcCollec.size()-idx > 0)
                {
                    responseMsg += ",\n";
                }
            }
            responseMsg += "\n]\n";               

            res = 200;
        }
        else 
        {
            res = 404;
            responseMsg = "{\"message\":\"Source not found\"}";
        }
        transaction.commit();
    }
    catch (Wt::Dbo::Exception const &e)
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;
}

unsigned short PluginResource::getParameterValueForSource(std::string& responseMsg) const
{
    unsigned short res = 500;
    if(!pluginIsAccessible(res, responseMsg))
    {
        return res;
    }
    int idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(*session);
        Wt::Dbo::collection<Wt::Dbo::ptr<SourceParameterValue>> srcParamCollec = session->find<SourceParameterValue>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                                                                      .where("\"SRC_ID\" = ?").bind(this->vPathElements[3]);

        if (srcParamCollec.size() > 0)
        {
            responseMsg += "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<SourceParameterValue> >::const_iterator i = srcParamCollec.begin(); i != srcParamCollec.end(); i++) 
            {
                responseMsg += "\t" + i->modify()->toJSON();
                ++idx;
                if(srcParamCollec.size()-idx > 0)
                {
                    responseMsg += ",\n";
                }
            }
            responseMsg += "\n]\n";               

            res = 200;
        }
        else 
        {
            res = 404;
            responseMsg = "{\"message\":\"Parameter not found\"}";
        }
        transaction.commit();
    }
    catch (Wt::Dbo::Exception const &e)
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;
}

void PluginResource::processGetRequest(Wt::Http::Response &response)
{
    std::string responseMsg = "", nextElement = "";
    
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->statusCode = getPlugin(responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned int>(nextElement);

            nextElement = getNextElementFromPath();

            if(!nextElement.compare(""))
            {
                this->statusCode = getPluginJSON(responseMsg);
            }
            else if(!nextElement.compare("informations"))
            {
                nextElement = getNextElementFromPath();
                if(!nextElement.compare(""))
                {
                    this->statusCode = getInformationListForPlugin(responseMsg);
                }
                else
                {
                    this->statusCode = 400;
                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                }
            }
            else if (!nextElement.compare("sources"))
            {
                nextElement = getNextElementFromPath();
                if(!nextElement.compare(""))
                {
                    this->statusCode = getSourceForPlugin(responseMsg);
                }
                else
                {
                    boost::lexical_cast<unsigned int>(nextElement);
                    nextElement = getNextElementFromPath();
                    if (!nextElement.compare("parameters"))
                    {
                        this->statusCode = getParameterValueForSource(responseMsg);
                    }
                    else if (!nextElement.compare("searches"))
                    {
                        nextElement = getNextElementFromPath();
                        if(!nextElement.compare(""))
                        {
                            this->statusCode = getSearchForSourceAndPlugin(responseMsg);
                        }
                        else
                        {
                            boost::lexical_cast<unsigned int>(nextElement);
                            nextElement = getNextElementFromPath();
                            if(!nextElement.compare("parameters"))
                            {
                                this->statusCode = getParameterValueForSearch(responseMsg);
                            }
                            else if(!nextElement.compare("informations"))
                            {
                                nextElement = getNextElementFromPath();
                                if(!nextElement.compare(""))
                                {
                                    this->statusCode = getInformationForSeaSrcAndPlg(responseMsg);
                                }
                                else
                                {
                                    this->statusCode = 400;
                                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                                }
                                
                            }
                            else
                            {
                                this->statusCode = 400;
                                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                            }
                        }
                    }
                    else
                    {
                        this->statusCode = 400;
                        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                    }
                }
            }
            else
            {
                this->statusCode = 400;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            this->statusCode = 400;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(this->statusCode);
    response.out() << responseMsg;
    return;
}


unsigned short PluginResource::postPlugin(std::string& responseMsg, const std::string& sRequest)
{
    unsigned short res = 500;
    Wt::WString plgName = "", plgDesc = "";
    try
    {
        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);
        //descriptif
        plgName = result.get("plg_name");
        plgDesc = result.get("plg_desc");
    }
    catch (Wt::Json::ParseError const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON:" << sRequest;
        return res;
    }
    catch (Wt::Json::TypeException const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON.:" << sRequest;
        return res;
    }   
    try 
    {
        Wt::Dbo::Transaction transaction(*session);
        Plugin *plugin = new Plugin();
        plugin->name = plgName;
        plugin->desc = plgDesc;
        
        Wt::Dbo::ptr<Plugin> plgPtr = session->add<Plugin>(plugin);
       
        plgPtr.flush();
        plgPtr.modify()->setId(plgPtr.id());
        responseMsg = plgPtr.modify()->toJSON();
        
        transaction.commit();
        res = 200;
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;
}

unsigned short PluginResource::postSourceForPlugin(std::string& responseMsg, const std::string& sRequest)
{
    unsigned short res = 500;
    if(!pluginIsAccessible(res, responseMsg))
    {
        return res;
    }
    Wt::WString addonId = "";
    
    try
    {
        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);
        int plgIdInt, addonIdInt;
        //descriptif

        addonIdInt = result.get("addon_id");
        addonId = boost::lexical_cast<std::string>(addonIdInt);

        
        try
        {
            Wt::Dbo::Transaction transaction(*session);
            
            // creer l'id de la source
            Wt::Dbo::collection<Wt::Dbo::ptr<Source>> srcCollec = session->find<Source>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1]);
            long long srcId = 1;
            if(srcCollec.size()> 0)
            {
                std::string queryStr = "SELECT MAX(\"SRC_ID\") FROM \"T_SOURCE_SRC\" src "
                                   " WHERE \"PLG_ID_PLG_ID\" = " + boost::lexical_cast<std::string > (this->vPathElements[1]);

                Wt::Dbo::Query<long long> queryResult = session->query<long long>(queryStr);
                srcId = queryResult + 1;
            }

            //creation de la source
            Wt::Dbo::ptr<Plugin> plgPtr = session->find<Plugin>().where("\"PLG_ID\" = ?").bind(this->vPathElements[1]);
            Wt::Dbo::ptr<Addon> addonPtr = session->find<Addon>().where("\"ADO_ID\" = ?").bind(addonId);
            Source *source = new Source;
            source->pk.id = srcId;
            source->pk.plugin = plgPtr;
            source->addon = addonPtr;
            Wt::Dbo::ptr<Source> srcPtr = session->add<Source>(source);
            srcPtr.flush();


            Wt::Dbo::collection<Wt::Dbo::ptr<SourceParameter> > sourceParameterPtr = addonPtr.get()->sourceParameters;

            //creations des parametres
            for (Wt::Dbo::collection<Wt::Dbo::ptr<SourceParameter> >::const_iterator i = sourceParameterPtr.begin(); i != sourceParameterPtr.end(); i++)
            {
                SourceParameterValue *sourceParameterValue = new SourceParameterValue;
                
                //recuperation des "source_parameters" dans le JSON
                Wt::WString tmp = result.get(boost::lexical_cast<std::string>((*i).get()->name));
                
                sourceParameterValue->value = tmp;
                sourceParameterValue->name = (*i).get()->name;
                sourceParameterValue->pk.sourceParameter = *i;
                sourceParameterValue->pk.source = srcPtr;
                session->add<SourceParameterValue>(sourceParameterValue);
            }
            res = 200;
            responseMsg = srcPtr.modify()->toJSON();
            transaction.commit();
        }
        catch (Wt::Dbo::Exception const& e) 
        {
            Wt::log("error") << e.what();
            res = 503;
            responseMsg = "{\"message\":\"Service Unavailable\"}";
        }
    }
    catch (Wt::Json::ParseError const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON:" << sRequest;
    }
    catch (Wt::Json::TypeException const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON.:" << sRequest;
    }   

    return res;
}

unsigned short PluginResource::postSearchForSourceAndPlugin(std::string& responseMsg, const std::string& sRequest)
{
    unsigned short res = 500;
    if(!pluginIsAccessible(res, responseMsg))
    {
        return res;
    }
    long long styId;
    bool seaIsStatic;
    int posKeyValue, nbValue, seaPeriod;
    Wt::Json::Array& units = Wt::Json::Array::Empty;
    
    try
    {
        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);

        //search
        seaPeriod = result.get("sea_period");
        styId = result.get("sty_id");
        seaIsStatic = result.get("sea_is_static");
        posKeyValue = result.get("pos_key_value");
        nbValue = result.get("nb_value");
        units = result.get("units");
        if(units.size() <= nbValue)
        {
            try
            {
                Wt::Dbo::Transaction transaction(*session);
                Wt::Dbo::ptr<Search> seaPtr;
                //verif si le search type va avec l'addon 
                
                std::string queryStr = "SELECT set FROM \"T_SEARCH_TYPE_STY\" set "
                                       " WHERE \"STY_ID\" IN"
                                        "("
                                            "SELECT \"T_SEARCH_TYPE_STY_STY_ID\" FROM \"TJ_ADO_STY\" "
                                            "WHERE \"T_ADDON_ADO_ADO_ID\" = " 
                                            "("
                                                "SELECT \"SRC_ADO_ADO_ID\" FROM \"T_SOURCE_SRC\" "
                                                "WHERE \"PLG_ID_PLG_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[1]) +
                                                "AND \"SRC_ID\" =  " + boost::lexical_cast<std::string>(this->vPathElements[3]) + 
                                            ")"
                                        ")"
                                        "AND \"STY_ID\" = " + boost::lexical_cast<std::string>(styId);
                
     
                //std::cerr << queryStr << std::endl;
                Wt::Dbo::Query<Wt::Dbo::ptr<SearchType> > queryRes = session->query<Wt::Dbo::ptr<SearchType> >(queryStr);

                Wt::Dbo::collection<Wt::Dbo::ptr<SearchType> > seaTypePtr = queryRes.resultList();

                if(seaTypePtr.size() == 0)
                {
                        res = 404;
                        responseMsg = "{\"message\":\"Search Type not found.\"}";
                        return res;
                }
                // creer l'id de la search
                Wt::Dbo::collection<Wt::Dbo::ptr<Search>> seaCollec = session->find<Search>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                     .where("\"SRC_ID\" = ?").bind(this->vPathElements[3]);
                long long seaId = 1;
                if (seaCollec.size() > 0)
                {
                    std::string queryStr = "SELECT MAX(\"SEA_ID\") FROM \"T_SEARCH_SEA\" sea "
                                           " WHERE \"PLG_ID_PLG_ID\" = " + boost::lexical_cast<std::string > (this->vPathElements[1]) +
                                           " AND \"SRC_ID\" = " + boost::lexical_cast<std::string > (this->vPathElements[3]) ;

                    Wt::Dbo::Query<long long> queryResult = session->query<long long>(queryStr);
                    seaId = queryResult + 1;    
                }

                //creation de la search
                Wt::Dbo::ptr<Source> srcPtr = session->find<Source>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                     .where("\"SRC_ID\" = ?").bind(this->vPathElements[3]);
                if(srcPtr)
                {
                    Wt::Dbo::ptr<SearchType> seaTypPtr = session->find<SearchType>().where("\"STY_ID\" = ?").bind(styId);
                    if(seaTypPtr)
                    {
                        Search *search = new Search;
                        search->pk.id = seaId;
                        search->pk.source = srcPtr;
                        search->searchType = seaTypPtr;
                        search->period = seaPeriod;
                        search->nbValue = nbValue;
                        search->pos_key_value = posKeyValue;
                        search->searchIsStatic = seaIsStatic;
                        seaPtr = session->add<Search>(search);
                        seaPtr.flush();


                        Wt::Dbo::collection<Wt::Dbo::ptr<SearchParameter> > searchParameterPtr = seaTypPtr.get()->searchParameters;
                        //creations des parametres
                        for (Wt::Dbo::collection<Wt::Dbo::ptr<SearchParameter> >::const_iterator i = searchParameterPtr.begin(); i != searchParameterPtr.end(); i++)
                        {
                            SearchParameterValue *searchParameterValue = new SearchParameterValue;

                            //recuperation des "search_parameters" dans le JSON
                            Wt::WString tmp = result.get(boost::lexical_cast<std::string>((*i).get()->name));

                            searchParameterValue->value = tmp;
                            searchParameterValue->name = (*i).get()->name;
                            searchParameterValue->searchParameterValueId.searchParameter = *i;
                            searchParameterValue->searchParameterValueId.search = seaPtr;
                            session->add<SearchParameterValue>(searchParameterValue);
                        }

                        //liée aux valeurs recherchées une unité
                        for (Wt::Json::Array::const_iterator idx1 = units.begin() ; idx1 < units.end(); idx1++)
                        {
                            Wt::Json::Object tmp = (*idx1);
                            int valNumUnit = tmp.get("val_num");
                            int unitId = tmp.get("unit_id");
                            Wt::Dbo::ptr<InformationUnit> informationUnitPtr = session->find<InformationUnit>().where("\"INU_ID\" = ?").bind(unitId);
                            if (informationUnitPtr && valNumUnit <= nbValue)
                            {
                                SearchUnit *searchUnit = new SearchUnit;
                                searchUnit->pk.infValueNum = valNumUnit;
                                searchUnit->pk.search = seaPtr;
                                searchUnit->informationUnit = informationUnitPtr;
                                session->add<SearchUnit>(searchUnit);
                            }
                            else
                            {
                                std::cerr << "info non trouvée ou valNum non autorisée" << std::endl;
                                res = 400;
                                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}"; 
                                return res;
                            }
                        }
                        responseMsg = seaPtr.modify()->toJSON();
                        res = 200;
                    }
                    else
                    {
                        res = 404;
                        responseMsg = "{\"message\":\"Search Type not found\"}";
                    }
                }
                else
                {
                    res = 404;
                    responseMsg = "{\"message\":\"Source not found\"}";
                }
                transaction.commit();
            }
            catch (Wt::Dbo::Exception const& e) 
            {
                Wt::log("error") << e.what();
                res = 503;
                responseMsg = "{\"message\":\"Service Unavailable\"}";
            }
        }
        else
        {
             std::cerr << "unité sur val_num non declarée" << std::endl;
             this->statusCode = 400;
             responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";  
        }
    }
    catch (Wt::Json::ParseError const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON:" << sRequest;
    }
    catch (Wt::Json::TypeException const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON.:" << sRequest;
    }   
    return res;
}

unsigned short PluginResource::postInformationForSeaSrcAndPlg(std::string& responseMsg, const std::string& sRequest)
{
    unsigned short res = 500;
    if(!pluginIsAccessible(res, responseMsg))
    {
        return res;
    }
    Wt::WString infName, infCalculate;
    bool infDisplay;
    int valueNum;
         
    try
    {
        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);
        
        //information
        infName = result.get("inf_name");
        infDisplay = result.get("inf_display");
        valueNum = result.get("inf_value_num");
        
        try
        {
            Wt::Dbo::Transaction transaction(*session);
            
            //search exist ?
            Wt::Dbo::ptr<Search> seaPtr = session->find<Search>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                 .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                                                                 .where("\"SEA_ID\" = ?").bind(this->vPathElements[5]);
            
            if(seaPtr)
            {
                //Relier une unité à l'info
                // unit exist?
                Wt::Dbo::ptr<SearchUnit> seaUnitPtr = session->find<SearchUnit>().where("\"INF_VALUE_NUM\" = ?").bind(valueNum)
                                                                                 .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                                 .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                                                                                 .where("\"SEA_ID\" = ?").bind(this->vPathElements[5]);
                if(seaUnitPtr)
                {
                    //creation info
                    Information2 *information = new Information2;
                    information->pk.search = seaPtr;
                    information->pk.unit = seaUnitPtr.get()->informationUnit;
                    information->pk.subSearchNumber = valueNum;
                    information->name = infName;
                    information->display = infDisplay;
                    
                    if(result.contains("inf_calculate"))
                    {
                        infCalculate = result.get("inf_calculate");
                        information->calculate = infCalculate;
                    }
                    Wt::Dbo::ptr<Information2> infPtr = session->add<Information2>(information);
                    infPtr.flush();
                    responseMsg = infPtr.modify()->toJSON();
                    res = 200;
                }
                else
                {
                    res = 404;
                    responseMsg = "{\"message\":\"Information not found\"}";
                }
            }
            else
            {
                res = 404;
                responseMsg = "{\"message\":\"Search not found\"}";
            }
            transaction.commit();
        }
        catch (Wt::Dbo::Exception const& e) 
        {
            Wt::log("error") << e.what();
            res = 503;
            responseMsg = "{\"message\":\"Service Unavailable\"}";
        }
    }
    catch (Wt::Json::ParseError const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON:" << sRequest;
    }
    catch (Wt::Json::TypeException const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON.:" << sRequest;
    }   
    return res; 
}

void PluginResource::processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    
// std::string responseMsg = "", nextElement = "", sRequest = "";
//
//    nextElement = getNextElementFromPath();
//    if(!nextElement.compare(""))
//    {
//        this->statusCode = 400;
//        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}"; 
//    }
//    else
//    {
//        try
//        {
//            boost::lexical_cast<unsigned int>(nextElement);
//            nextElement = getNextElementFromPath();
//            if(!nextElement.compare(""))
//            {
//                this->statusCode = deletePlugin(responseMsg);
//            }
//            else if(!nextElement.compare("sources"))
//            {
//                nextElement = getNextElementFromPath();
//                boost::lexical_cast<unsigned int>(nextElement);
//
//                nextElement = getNextElementFromPath();
//                if(!nextElement.compare(""))
//                {
//                    this->statusCode = deleteSourceForPlugin(responseMsg);
//                }
//                else if(!nextElement.compare("searches"))
//                {
//                    
//                    nextElement = getNextElementFromPath();
//                    boost::lexical_cast<unsigned int>(nextElement);
//
//                    nextElement = getNextElementFromPath();
//
//                    if(!nextElement.compare(""))
//                    {
//                        this->statusCode = deleteSearchForSourceAndPlugin(responseMsg);
//                    }
//                    else if (!nextElement.compare("inf_values"))
//                    {
//                        nextElement = getNextElementFromPath();
//                        boost::lexical_cast<unsigned int>(nextElement);
//                        nextElement = getNextElementFromPath();
//                        if (!nextElement.compare("units"))
//                        {
//                            nextElement = getNextElementFromPath();
//                            boost::lexical_cast<unsigned int>(nextElement);
//                            nextElement = getNextElementFromPath();
//                            if (!nextElement.compare("informations"))
//                            {
//                                this->statusCode = deleteInformationForSeaSrcAndPlg(responseMsg);
//                            }
//                            else
//                            {
//                                this->statusCode = 400;
//                                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                            }
//                        }
//                        else
//                        {
//                            this->statusCode = 400;
//                            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                        }
//                    }
//                    else
//                    {
//                        this->statusCode = 400;
//                        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                    }
//                }
//                else
//                {
//                    this->statusCode = 400;
//                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                }
//            }
//            else
//            {
//                this->statusCode = 400;
//                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//            }
//        }
//        catch(boost::bad_lexical_cast &)
//        {
//            this->statusCode = 400;
//            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//        }
//    }
//
//    response.setStatus(this->statusCode);
//    response.out() << responseMsg;
//    return;


    std::string responseMsg = "", nextElement = "", sRequest = "";

    sRequest = request2string(request);
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->statusCode = postPlugin(responseMsg, sRequest);
    }
    else
    {       
        try
        {
            boost::lexical_cast<unsigned int>(nextElement);

            nextElement = getNextElementFromPath();
///////////////////////////////////////
            if(!nextElement.compare(""))
            {
                this->statusCode = deletePlugin(responseMsg);
            }
 ///////////////////////////////////////         
            else if(!nextElement.compare("sources"))
            {
                nextElement = getNextElementFromPath();
                if (!nextElement.compare(""))
                {
                    this->statusCode = postSourceForPlugin(responseMsg, sRequest);
                }
                else
                {
                    boost::lexical_cast<unsigned int>(nextElement);

                    nextElement = getNextElementFromPath();
 ///////////////////////////////////////
                    if(!nextElement.compare(""))
                    {
                        this->statusCode = deleteSourceForPlugin(responseMsg);
                        //this->statusCode = patchParametersSourceForPlugin(responseMsg, sRequest);
                    }
                    else if(!nextElement.compare("patch"))
                    {
                        this->statusCode = patchParametersSourceForPlugin(responseMsg, sRequest);
                    }
 ///////////////////////////////////////                    
                    else if(!nextElement.compare("searches"))
                    {
                        nextElement = getNextElementFromPath();
                        if (!nextElement.compare(""))
                        {
                            this->statusCode = postSearchForSourceAndPlugin(responseMsg, sRequest);
                        }
                        else
                        {
                            boost::lexical_cast<unsigned int>(nextElement);
                            nextElement = getNextElementFromPath();
 ///////////////////////////////////////
                            if(!nextElement.compare(""))
                            {
                                this->statusCode = deleteSearchForSourceAndPlugin(responseMsg);
                                //this->statusCode = patchSearchForSourceAndPlugin(responseMsg, sRequest);
                            }
                            else if(!nextElement.compare("patch"))
                            {
                                this->statusCode = patchSearchForSourceAndPlugin(responseMsg, sRequest);
                            }
 ///////////////////////////////////////
                            else if (!nextElement.compare("inf_values"))
                            {
                                nextElement = getNextElementFromPath();
                                boost::lexical_cast<unsigned int>(nextElement);
                                nextElement = getNextElementFromPath();
                                if (!nextElement.compare("units"))
                                {
                                    nextElement = getNextElementFromPath();
                                    boost::lexical_cast<unsigned int>(nextElement);
                                    nextElement = getNextElementFromPath();
                                    if (!nextElement.compare("informations"))
                                    {
                                        this->statusCode = deleteInformationForSeaSrcAndPlg(responseMsg);
                                        //this->statusCode = patchInformationForSeaSrcAndPlg(responseMsg, sRequest);
                                    }
                                    else
                                    {
                                        this->statusCode = 400;
                                        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                                    }
                                }
                                else
                                {
                                    this->statusCode = 400;
                                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                                }
                            }
 ///////////////////////////////////////       
                            else if(!nextElement.compare("informations"))
                            {
                                nextElement = getNextElementFromPath();
                                if (!nextElement.compare(""))
                                {
                                    this->statusCode = postInformationForSeaSrcAndPlg(responseMsg, sRequest);
                                }
                                else
                                {
                                    this->statusCode = 400;
                                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                                }
                            }
                            else
                            {
                                this->statusCode = 400;
                                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                            }
                        }
                    }
                    else
                    {
                        this->statusCode = 400;
                        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                    }
                }
            }
            else
            {  
                this->statusCode = 400;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            this->statusCode = 400;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }             
    }

    response.setStatus(this->statusCode);
    response.out() << responseMsg;
    return ;
}


void PluginResource::processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}

unsigned short PluginResource::patchInformationForSeaSrcAndPlg(std::string &responseMsg, const std::string &sRequest)
{
     unsigned short res = 500;
    if(!pluginIsAccessible(res, responseMsg))
    {
        return res;
    }
    Wt::WString infName, infCalculate;
    bool infDisplay;
    int valueNum;
         
    try
    {
        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);
        
        //information
        infName = result.get("inf_name");
        infDisplay = result.get("inf_display");
        infCalculate = result.get("inf_calculate");
        
        try
        {
            Wt::Dbo::Transaction transaction(*session);
            
            // Information exist?
            Wt::Dbo::ptr<Information2> infPtr = session->find<Information2>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                             .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                                                                             .where("\"SEA_ID\" = ?").bind(this->vPathElements[5])
                                                                             .where("\"INF_VALUE_NUM\" = ?").bind(this->vPathElements[7])
                                                                             .where("\"INU_ID_INU_ID\" = ?").bind(this->vPathElements[9]);
            if(infPtr)
            {
                //Relier une unité à l'info
                // unit exist?
                Wt::Dbo::ptr<SearchUnit> seaUnitPtr = session->find<SearchUnit>().where("\"INF_VALUE_NUM\" = ?").bind(this->vPathElements[7])
                                                                                 .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                                 .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                                                                                 .where("\"SEA_ID\" = ?").bind(this->vPathElements[5]);
                if(seaUnitPtr)
                {

                    //creation info
                    infPtr.modify()->name = infName;
                    infPtr.modify()->display = infDisplay;
                    infPtr.modify()->calculate = infCalculate;
                    responseMsg = infPtr.modify()->toJSON();
                    res = 200;
                }
                else
                {
                    res = 404;
                    responseMsg = "{\"message\":\"Unit not found\"}";
                }
            }
            else
            {
                res = 404;
                responseMsg = "{\"message\":\"Information not found\"}";
            }

            transaction.commit();
        }
        catch (Wt::Dbo::Exception const& e) 
        {
            Wt::log("error") << e.what();
            res = 503;
            responseMsg = "{\"message\":\"Service Unavailable\"}";
        }
    }
    catch (Wt::Json::ParseError const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON:" << sRequest;
    }
    catch (Wt::Json::TypeException const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON.:" << sRequest;
    }   
    return res; 
}

unsigned short PluginResource::patchSearchForSourceAndPlugin(std::string& responseMsg, const std::string& sRequest)
{
    unsigned short res = 500;
    if(!pluginIsAccessible(res, responseMsg))
    {
        return res;
    }
    long long styId;
    bool seaIsStatic;
    int posKeyValue, nbValue, seaPeriod;
    
    try
    {
        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);

        //search
        seaPeriod = result.get("sea_period");
        seaIsStatic = result.get("sea_is_static");
        posKeyValue = result.get("pos_key_value");
            try
            {
                Wt::Dbo::Transaction transaction(*session);
                Wt::Dbo::ptr<Search> seaPtr;

                //modification de la search si elle exist
                seaPtr = session->find<Search>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                                                .where("\"SEA_ID\" = ?").bind(this->vPathElements[5]);
                if(seaPtr)
                {
                    Wt::Dbo::ptr<SearchType> seaTypPtr = session->find<SearchType>().where("\"STY_ID\" = ?").bind(seaPtr.get()->searchType.id());
                    if(seaTypPtr)
                    {
                        
                        seaPtr.modify()->period = seaPeriod;
                        seaPtr.modify()->pos_key_value = posKeyValue;
                        seaPtr.modify()->searchIsStatic = seaIsStatic;
          
                        Wt::Dbo::collection<Wt::Dbo::ptr<SearchParameter> > searchParameterPtr = seaTypPtr.get()->searchParameters;
                        //modification des parametres
                        for (Wt::Dbo::collection<Wt::Dbo::ptr<SearchParameter> >::const_iterator i = searchParameterPtr.begin(); i != searchParameterPtr.end(); i++)
                        {
                            Wt::Dbo::ptr<SearchParameterValue> searchParameterValuePtr = session->find<SearchParameterValue>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                                                                              .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                                                                                                                              .where("\"SEA_ID\" = ?").bind(this->vPathElements[5])
                                                                                                                              .where("\"SEP_ID_SEP_ID\" = ?").bind((*i).id());

                            //recuperation des "search_parameters" dans le JSON
                            Wt::WString tmp = result.get(boost::lexical_cast<std::string>((*i).get()->name));

                            searchParameterValuePtr.modify()->value = tmp;
                        }
                    }
                    else
                    {
                        res = 404;
                        responseMsg = "{\"message\":\"Search Type not found\"}";
                        return res;
                    }
                }
                else
                {
                    res = 404;
                    responseMsg = "{\"message\":\"Search not found\"}";
                    return res;
                }
                res = 200;
                responseMsg = seaPtr.modify()->toJSON();
                transaction.commit();
            }
            catch (Wt::Dbo::Exception const& e) 
            {
                Wt::log("error") << e.what();
                res = 503;
                responseMsg = "{\"message\":\"Service Unavailable\"}";
            }
    }
    catch (Wt::Json::ParseError const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON:" << sRequest;
    }
    catch (Wt::Json::TypeException const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON.:" << sRequest;
    }   
    return res;
}

unsigned short PluginResource::patchParametersSourceForPlugin(std::string &responseMsg, const std::string &sRequest)
{
    unsigned short res = 500;
    if(!pluginIsAccessible(res, responseMsg))
    {
        return res;
    }
    Wt::Json::Array& parameters = Wt::Json::Array::Empty;
    
    try
    {
        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);
        parameters = result.get("parameters");
        try
        {
            Wt::Dbo::Transaction transaction(*session);
            Wt::Dbo::ptr<Source> srcPtr = session->find<Source>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                 .where("\"SRC_ID\" = ?").bind(this->vPathElements[3]);

            if(srcPtr)
            {
                //modification des parametres
                Wt::Dbo::ptr<SourceParameterValue> srcParamValPtr;
                for (Wt::Json::Array::const_iterator idx1 = parameters.begin() ; idx1 < parameters.end(); idx1++)
                {
                    Wt::Json::Object tmp = (*idx1);
                    int srpIdParam = tmp.get("srp_id");
                    Wt::WString valueParam = tmp.get("value");


                    srcParamValPtr = session->find<SourceParameterValue>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                          .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                                                                          .where("\"SRP_ID_SRP_ID\" = ?").bind(srpIdParam);
                    if (srcParamValPtr)
                    {
                        srcParamValPtr.modify()->value = valueParam;
                    }
                    else
                    {
                        res = 404;
                        responseMsg = "{\"message\":\"Parameter not found\"}";
                        return res;
                    }
                    responseMsg += srcParamValPtr.modify()->toJSON();
                    res = 200;
                }
            }
            else
            {
                res = 404;
                responseMsg = "{\"message\":\"Source not found\"}";
            }
            transaction.commit();
        }
        catch (Wt::Dbo::Exception const& e) 
        {
            Wt::log("error") << e.what();
            res = 503;
            responseMsg = "{\"message\":\"Service Unavailable\"}";
        }
    }
    catch (Wt::Json::ParseError const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON:" << sRequest;
    }
    catch (Wt::Json::TypeException const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON.:" << sRequest;
    }   

    return res;
}

void PluginResource::processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
   std::string responseMsg = "", nextElement = "", sRequest = "";
    
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->statusCode = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned int>(nextElement);

            nextElement = getNextElementFromPath();

            if (!nextElement.compare("sources"))
            {
                nextElement = getNextElementFromPath();
                if(!nextElement.compare(""))
                {
                    this->statusCode = 400;
                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                }
                else
                {
                    boost::lexical_cast<unsigned int>(nextElement);
                    nextElement = getNextElementFromPath();
                    if (!nextElement.compare("parameters"))
                    {
                        this->statusCode = patchParametersSourceForPlugin(responseMsg, sRequest);
                    }
                    else
                    {
                        this->statusCode = 400;
                        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                    }
                }
            }
            else
            {
                this->statusCode = 400;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            this->statusCode = 400;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(this->statusCode);
    response.out() << responseMsg;
    return;
}

unsigned short PluginResource::deleteInformationForSeaSrcAndPlg(std::string& responseMsg)
{
    unsigned short res = 500;
    if(!pluginIsAccessible(res, responseMsg))
    {
        return res;
    }
    try 
    {  
        Wt::Dbo::Transaction transaction(*this->session);
           
        Wt::Dbo::ptr<Information2> informationPtr = session->find<Information2>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                             .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                                                             .where("\"SEA_ID\" = ?").bind(this->vPathElements[5])
                                                             .where("\"INF_VALUE_NUM\" = ?").bind(this->vPathElements[7])
                                                             .where("\"INU_ID_INU_ID\" = ?").bind(this->vPathElements[9]);
        //information exist ?
        if(informationPtr)
        {
            Wt::Dbo::collection<Wt::Dbo::ptr<AlertValue>> avaCollec = session->find<AlertValue>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                                                 .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                                                                                                 .where("\"SEA_ID\" = ?").bind(this->vPathElements[5])
                                                                                                 .where("\"INF_VALUE_NUM\" = ?").bind(this->vPathElements[7])
                                                                                                 .where("\"INU_ID_INU_ID\" = ?").bind(this->vPathElements[9])
                                                                                                 .where("\"AVA_DELETE\" IS NULL");
            //verif si l'info n'est pas utilisée                                                                
            if (avaCollec.size() == 0)
            {                
                //supprime l'info

                informationPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
//                std::string executeString1 = "DELETE FROM \"T_INFORMATION_INF\" "
//                                             "WHERE \"PLG_ID_PLG_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[1]) +
//                                                " AND \"SRC_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[3]) +
//                                                " AND \"SEA_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[5]) +
//                                                " AND \"INF_VALUE_NUM\" = " + boost::lexical_cast<std::string>(this->vPathElements[7]) +
//                                                " AND \"INU_ID_INU_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[9]);
//                                                            
//                session->execute(executeString1);
                //session->execute(executeString2);
               res = 204;
            }
            else
            {
                res = 409;
                responseMsg = "{\"message\":\"Conflict, an alert use this information\"}";
                return res;
            }
            
        }
        else
        {
            responseMsg = "{\"message\":\"Information Not Found\"}";
            res = 404;
            return res;
        }

        transaction.commit();               
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    
    return res;
}

unsigned short PluginResource::deleteSearchForSourceAndPlugin(std::string& responseMsg)
{
    unsigned short res = 500;
    if(!pluginIsAccessible(res, responseMsg))
    {
        return res;
    }
    try 
    {  
        Wt::Dbo::Transaction transaction(*this->session);
           
        Wt::Dbo::ptr<Search> seaPtr = session->find<Search>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                             .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                                                             .where("\"SEA_ID\" = ?").bind(this->vPathElements[5]);
        //search exist ?
        if(seaPtr)
        {
            Wt::Dbo::collection<Wt::Dbo::ptr<Information2>> infCollec = session->find<Information2>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                                                 .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                                                                                                 .where("\"SEA_ID\" = ?").bind(this->vPathElements[5]);

            for (Wt::Dbo::collection<Wt::Dbo::ptr<Information2> >::const_iterator i = infCollec.begin(); i != infCollec.end(); i++) 
            {
                std::string path = "/" + vPathElements[1] + "/sources/" + vPathElements[3] + "/searches/" + 
                                         vPathElements[5] + "/inf_values/" + boost::lexical_cast<std::string>(i->get()->pk.subSearchNumber) +
                                        "/units/" + boost::lexical_cast<std::string>(i->get()->pk.unit.id());
                boost::split(this->vPathElements, path, boost::is_any_of("/"), boost::token_compress_on);

                res = deleteInformationForSeaSrcAndPlg(responseMsg);
                if( res != 204)
                {
                    res = 409;
                    responseMsg = "{\"message\":\"Conflict, an alert use this search\"}";
                    return res;
                }
            }           
            
            seaPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
            res = 204;   
        }
        else
        {
            responseMsg = "{\"message\":\"Search Not Found\"}";
            res = 404;
        }
        transaction.commit();               
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    
    return res;
    
}

unsigned short PluginResource::deletePlugin(std::string& responseMsg)
{
    unsigned short res = 500;
    try 
    {  
        if(!pluginIsAccessible(res, responseMsg))
        {
            return res;
        }
        Wt::Dbo::Transaction transaction(*this->session);
        Wt::Dbo::ptr<Plugin> plgPtr = session->find<Plugin>().where("\"PLG_ID\" = ?").bind(this->vPathElements[1]);
        //verif si le plugin existe
        if(plgPtr)
        {
            Wt::Dbo::collection<Wt::Dbo::ptr<Source>> srcCollec = session->find<Source>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1]);
            //verif si le plugin n'est pas utilisé
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Source> >::const_iterator i = srcCollec.begin(); i != srcCollec.end(); i++) 
            {
                std::string path = "/" + vPathElements[1] + "/sources/" + boost::lexical_cast<std::string>(i->get()->pk.id);
                boost::split(this->vPathElements, path, boost::is_any_of("/"), boost::token_compress_on);

                res = deleteSourceForPlugin(responseMsg);
                if( res != 204)
                {
                    res = 409;
                    responseMsg = "{\"message\":\"Conflict, an alerts use this plugin\"}";
                    return res;
                }
            }

            plgPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
//            std::string queryString = "DELETE FROM \"TJ_AST_PLG\" "
//                             "WHERE \"T_PLUGIN_PLG_PLG_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[1]);
//            session->execute(queryString);
//            
//            std::string queryString1 = "DELETE FROM \"T_PLUGIN_PLG\" "
//                             "WHERE \"PLG_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[1]);
//            session->execute(queryString1);
        }
        else
        {
            res = 404;
            responseMsg = "{\"message\":\"Plugin not found\"}";
            return res;
        }
        res = 204;
        transaction.commit();               
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    
    return res;
}

unsigned short PluginResource::deleteSourceForPlugin(std::string& responseMsg)
{
    unsigned short res = 500;
    if(!pluginIsAccessible(res, responseMsg))
    {
        return res;
    }
    try 
    {  
        Wt::Dbo::Transaction transaction(*this->session);
        Wt::Dbo::ptr<Source> srcPtr = session->find<Source>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                             .where("\"SRC_ID\" = ?").bind(this->vPathElements[3]);
        //verif si la source existe
        if(srcPtr)
        {
            Wt::Dbo::collection<Wt::Dbo::ptr<Search>> seaCollec = session->find<Search>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                 .where("\"SRC_ID\" = ?").bind(this->vPathElements[3]);
            
                for (Wt::Dbo::collection<Wt::Dbo::ptr<Search> >::const_iterator i = seaCollec.begin(); i != seaCollec.end(); i++) 
                {
                    std::string path = "/" + vPathElements[1] + "/sources/" + vPathElements[3] + "/searches/" + 
                                             boost::lexical_cast<std::string>(i->get()->pk.id);
                    boost::split(this->vPathElements, path, boost::is_any_of("/"), boost::token_compress_on);

                    res = deleteSearchForSourceAndPlugin(responseMsg);
                    if( res != 204)
                    {
                        res = 409;
                        responseMsg = "{\"message\":\"Conflict, an alerts use this source\"}";
                        return res;
                    }
                }
            srcPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
//                std::string queryString = "DELETE FROM \"T_SOURCE_PARAMETER_VALUE_SPV\" "
//                                 "WHERE \"PLG_ID_PLG_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[1]) +
//                                 "AND \"SRC_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[3]) +
//                                 "AND \"SRP_ID_SRP_ID\" IN "
//                                 "("
//                                        "SELECT \"T_SOURCE_PARAMETER_SRP_SRP_ID\" FROM \"TJ_ADO_SRP\" "
//                                        "WHERE \"T_ADDON_ADO_ADO_ID\" = " + boost::lexical_cast<std::string>(srcPtr.get()->addon.id()) +
//                                 ")";
//                session->execute(queryString);
//                std::string queryString2 = "DELETE FROM \"T_SOURCE_SRC\" "
//                                           "WHERE \"PLG_ID_PLG_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[1]) +
//                                           "AND \"SRC_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[3]);
//                session->execute(queryString2);

        }
        else
        {
            res = 404;
            responseMsg = "{\"message\":\"Source not found\"}";
            return res;
        }
        res = 204;
        transaction.commit();               
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    
    return res;
    
}

void PluginResource::processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    std::string responseMsg = "", nextElement = "", sRequest = "";

    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->statusCode = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}"; 
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned int>(nextElement);
            nextElement = getNextElementFromPath();
            if(!nextElement.compare(""))
            {
                this->statusCode = deletePlugin(responseMsg);
            }
            else if(!nextElement.compare("sources"))
            {
                nextElement = getNextElementFromPath();
                boost::lexical_cast<unsigned int>(nextElement);

                nextElement = getNextElementFromPath();
                if(!nextElement.compare(""))
                {
                    this->statusCode = deleteSourceForPlugin(responseMsg);
                }
                else if(!nextElement.compare("searches"))
                {
                    
                    nextElement = getNextElementFromPath();
                    boost::lexical_cast<unsigned int>(nextElement);

                    nextElement = getNextElementFromPath();

                    if(!nextElement.compare(""))
                    {
                        this->statusCode = deleteSearchForSourceAndPlugin(responseMsg);
                    }
                    else if (!nextElement.compare("inf_values"))
                    {
                        nextElement = getNextElementFromPath();
                        boost::lexical_cast<unsigned int>(nextElement);
                        nextElement = getNextElementFromPath();
                        if (!nextElement.compare("units"))
                        {
                            nextElement = getNextElementFromPath();
                            boost::lexical_cast<unsigned int>(nextElement);
                            nextElement = getNextElementFromPath();
                            if (!nextElement.compare("informations"))
                            {
                                this->statusCode = deleteInformationForSeaSrcAndPlg(responseMsg);
                            }
                            else
                            {
                                this->statusCode = 400;
                                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                            }
                        }
                        else
                        {
                            this->statusCode = 400;
                            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                        }
                    }
                    else
                    {
                        this->statusCode = 400;
                        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                    }
                }
                else
                {
                    this->statusCode = 400;
                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                }
            }
            else
            {
                this->statusCode = 400;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            this->statusCode = 400;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(this->statusCode);
    response.out() << responseMsg;
    return;
}

PluginResource::PluginResource(const PluginResource& orig) {
}

PluginResource::~PluginResource(){
    beingDeleted();
}


//unsigned short PluginResource::getPlugin(string &responseMsg) const
//{
//    unsigned short res = 500;
//
//    try
//    {
//        if (boost::lexical_cast<unsigned int>(this->vPathElements[1]) == 1)
//        {
//            ifstream ifs("/var/www/wt/probe/plugins/Linux-System.json");
//            string content((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
//            responseMsg = content;
//
//            res = 200;
//        } 
//        else 
//        {
//            responseMsg = "{\n\t\"message\":\"Plugin not found\"\n}";
//            res = 404;
//        }
//    }
//    catch(boost::bad_lexical_cast &)
//    {
//        res = 400;
//        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//    }
//
//    return res;
//}













void PluginResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);
    return;
}

