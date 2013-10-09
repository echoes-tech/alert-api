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

PluginResource::PluginResource() : PublicApiResource::PublicApiResource()
{
}

PluginResource::PluginResource(const PluginResource& orig) : PublicApiResource::PublicApiResource(orig)
{
}

PluginResource::~PluginResource(){
}

 unsigned short PluginResource::pluginIsAccessible(string& responseMsg)
{
     
    unsigned short res = 500;
    Wt::Dbo::ptr<Plugin> plgPtr;
    try
    {

        Wt::Dbo::Transaction transaction(_session);

        string queryStr = "SELECT plg FROM \"T_PLUGIN_PLG\" plg "
                                "WHERE \"PLG_ID\" IN "
                                "("
                                   "SELECT \"T_PLUGIN_PLG_PLG_ID\" FROM \"TJ_AST_PLG\" "
                                   "wHERE \"T_ASSET_AST_AST_ID\" IN "
                                        "("
                                        "SELECT \"AST_ID\" FROM \"T_ASSET_AST\" "
                                        "WHERE \"AST_ORG_ORG_ID\" = " + boost::lexical_cast<string>(_session.user().get()->currentOrganization.id()) +
                                        " AND \"AST_DELETE\" IS NULL "
                                        ")"
                                    "AND \"T_PLUGIN_PLG_PLG_ID\" = " + boost::lexical_cast<string>(vPathElements[1]) + 
                                " )"
                                " AND \"PLG_DELETE\" IS NULL";

         Wt::Dbo::Query<Wt::Dbo::ptr<Plugin>> queryRes = _session.query<Wt::Dbo::ptr<Plugin>>(queryStr);

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
    return res;
}

unsigned short PluginResource::getPluginJSON(string& responseMsg)
{
    unsigned short res = 500;
    try
    {
        Wt::Dbo::Transaction transaction(_session);

        // on regarde si le plugin appartient à l'organisation
        string queryStr = "SELECT plg FROM \"T_PLUGIN_PLG\" plg "
                                "WHERE \"PLG_ID\" IN "
                                "("
                                   "SELECT \"T_PLUGIN_PLG_PLG_ID\" FROM \"TJ_AST_PLG\" "
                                   "wHERE \"T_ASSET_AST_AST_ID\" IN "
                                        "("
                                        "SELECT \"AST_ID\" FROM \"T_ASSET_AST\" "
                                        "WHERE \"AST_ORG_ORG_ID\" = " + boost::lexical_cast<string>(_session.user().get()->currentOrganization.id()) +
                                        " AND \"AST_DELETE\" IS NULL "
                                        ")"
                                    "AND \"T_PLUGIN_PLG_PLG_ID\" = " + boost::lexical_cast<string>(vPathElements[1]) + 
                                " )" 
                                " AND \"PLG_DELETE\" IS NULL";
        
         Wt::Dbo::Query<Wt::Dbo::ptr<Plugin> > queryRes = _session.query<Wt::Dbo::ptr<Plugin> >(queryStr);

         Wt::Dbo::ptr<Plugin> plgPtr = queryRes;
         
        if(plgPtr)
        {
            responseMsg = "{\n\t\"id\" : " + boost::lexical_cast<string>(vPathElements[1]) + ",\n";
            responseMsg += "\t\"name\" : \"" + plgPtr->name.toUTF8() + "\",\n";
            responseMsg += "\t\"desc\" : \"" + plgPtr->desc.toUTF8() + "\",\n";
            responseMsg += "\t\"idAsset\" : 0,\n";
            responseMsg += "\t\"sources\" : [\n"; 
    
            Wt::Dbo::collection<Wt::Dbo::ptr<Source>> srcCollec = _session.find<Source>()
                    .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                    .where("\"SRC_DELETE\" IS NULL")
                    .orderBy("\"SRC_ID\"");

            for (Wt::Dbo::collection<Wt::Dbo::ptr<Source> >::const_iterator i = srcCollec.begin(); i != srcCollec.end(); i++) 
            {
                responseMsg += "\t{\n";
                responseMsg += "\t\t\"id\" : " + boost::lexical_cast<string>(i->get()->pk.id) +",\n";
                responseMsg += "\t\t\"idAddon\" : " + boost::lexical_cast<string>(i->get()->addon.id()) +",\n";
                responseMsg += "\t\t\"params\" : {\n";
                
                Wt::Dbo::collection<Wt::Dbo::ptr<SourceParameterValue>> srcParamCollec = _session.find<SourceParameterValue>()
                        .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                        .where("\"SRC_ID\" = ?").bind(boost::lexical_cast<string>(i->get()->pk.id));

                for (Wt::Dbo::collection<Wt::Dbo::ptr<SourceParameterValue> >::const_iterator i1 = srcParamCollec.begin(); i1 != srcParamCollec.end(); i1++) 
                {
                    Wt::Dbo::ptr<SourceParameter> srcParam = _session.find<SourceParameter>().where("\"SRP_ID\" = ?").bind(i1->get()->pk.sourceParameter.id());
                    responseMsg += "\t\t\t\"" + boost::lexical_cast<string>(srcParam.get()->name) + "\": \""+ boost::lexical_cast<string>(i1->get()->value) +"\",\n";              
                }
                if(srcParamCollec.size() != 0)
                {
                    responseMsg.replace(responseMsg.size()-2, 1, "");
                }
                responseMsg += "\t\t},\n";
                responseMsg += "\t\t\"searches\": [\n";  
      
                Wt::Dbo::collection<Wt::Dbo::ptr<Search>> seaCollec = _session.find<Search>()
                        .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                        .where("\"SRC_ID\" = ?").bind(boost::lexical_cast<string>(i->get()->pk.id))
                        .where("\"SEA_DELETE\" IS NULL")
                        .orderBy("\"SEA_ID\"");

                for (Wt::Dbo::collection<Wt::Dbo::ptr<Search> >::const_iterator i2 = seaCollec.begin(); i2 != seaCollec.end(); i2++) 
                {
                    responseMsg += "\t\t{\n";
                    responseMsg += "\t\t\t\"id\" : " + boost::lexical_cast<string>(i2->get()->pk.id) +",\n";
                    responseMsg += "\t\t\t\"idType\" : " + boost::lexical_cast<string>(i2->get()->searchType.id()) +",\n";
                    responseMsg += "\t\t\t\"params\" : {\n";

                    Wt::Dbo::collection<Wt::Dbo::ptr<SearchParameterValue>> seaParamCollec = _session.find<SearchParameterValue>()
                            .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                            .where("\"SRC_ID\" = ?").bind(boost::lexical_cast<string>(i->get()->pk.id))
                            .where("\"SEA_ID\" = ?").bind(boost::lexical_cast<string>(i2->get()->pk.id));

                    for (Wt::Dbo::collection<Wt::Dbo::ptr<SearchParameterValue> >::const_iterator i3 = seaParamCollec.begin(); i3 != seaParamCollec.end(); i3++) 
                    {
                        Wt::Dbo::ptr<SearchParameter> seaParam = _session.find<SearchParameter>().where("\"SEP_ID\" = ?").bind(i3->get()->searchParameterValueId.searchParameter.id());
                        responseMsg += "\t\t\t\t\"" + boost::lexical_cast<string>(seaParam.get()->name) + "\": \""+ boost::lexical_cast<string>(i3->get()->value) +"\",\n";           
                    }
                    if(seaParamCollec.size() != 0)
                    {
                        responseMsg.replace(responseMsg.size()-2, 1, "");
                    }   
                    responseMsg += "\t\t\t},\n";
                    responseMsg += "\t\t\t\"period\" : " + boost::lexical_cast<string>(i2->get()->period) + ",\n";

                    responseMsg += "\t\t\t\"staticValues\" : " + boost::lexical_cast<string>(i2->get()->searchIsStatic) +"\n"; 
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

unsigned short PluginResource::getPlugin(string& responseMsg)
{
    unsigned short res = 500;
    unsigned idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(_session);

        // on liste les plugins rattachés à des assets détenu par l'organization
        string queryStr = "SELECT plg FROM \"T_PLUGIN_PLG\" plg "
                                "WHERE \"PLG_ID\" IN "
                                "("
                                   "SELECT \"T_PLUGIN_PLG_PLG_ID\" FROM \"TJ_AST_PLG\" "
                                   "wHERE \"T_ASSET_AST_AST_ID\" IN "
                                        "("
                                        "SELECT \"AST_ID\" FROM \"T_ASSET_AST\" "
                                        "WHERE \"AST_ORG_ORG_ID\" = " + boost::lexical_cast<string>(_session.user().get()->currentOrganization.id()) +
                                        " AND \"AST_DELETE\" IS NULL "
                                        ")"
                                ") "
                                " AND \"PLG_DELETE\" IS NULL"
                                " ORDER BY \"PLG_ID\" ";
        
         Wt::Dbo::Query<Wt::Dbo::ptr<Plugin> > queryRes = _session.query<Wt::Dbo::ptr<Plugin> >(queryStr);

         Wt::Dbo::collection<Wt::Dbo::ptr<Plugin> > plgPtr = queryRes.resultList();

        
        if(plgPtr.size() > 0 )
        {
            responseMsg = "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Plugin> >::const_iterator i = plgPtr.begin(); i != plgPtr.end(); i++) 
            {
                i->modify()->setId(i->id());
                responseMsg +=  i->get()->toJSON();
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


unsigned short PluginResource::getAliasForInformation(string &responseMsg)
{
    unsigned short res = 500;
    if (this->role.empty())
    {
        res = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        return res;
    }

    if (this->media.empty())
    {
        res = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        return res;
    }
    
    try 
    {
        Wt::Dbo::Transaction transaction(_session);
        Wt::Dbo::ptr<AlertMessageAliasInformation> aliasInformation = this->_session.find<AlertMessageAliasInformation>()
                .where("\"AAI_DELETE\" IS NULL")
                .where("\"URO_ID_URO_ID\" = ?").bind(this->role)
                .where("\"MED_ID_MED_ID\" = ?").bind(this->media)
                .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                .where("\"SEA_ID\" = ?").bind(this->vPathElements[5])
                .where("\"INF_VALUE_NUM\" = ?").bind(this->vPathElements[7])
                .where("\"INU_ID_INU_ID\" = ?").bind(this->vPathElements[9]);
        if (aliasInformation)
        {
            responseMsg = aliasInformation->toJSON();
            res = 200;
            transaction.commit();
        }
        else
        {
            res = 404;
            responseMsg = "{\n\t\"message\":\"Alias not found\"\n}";
        }
        
        
    } 
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << "[Plugin Ressource] " << e.what();
        res = 503;
        responseMsg = "{\n\t\"message\": \"Service Unavailable\"\n}";
    }
    
    
    return res;
}

unsigned short PluginResource::getAliasForCriteria(string &responseMsg)
{
    unsigned short res = 500;
    if (this->role.empty())
    {
        res = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        return res;
    }

    if (this->media.empty())
    {
        res = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        return res;
    }
    
    try 
    {
        Wt::Dbo::Transaction transaction(_session);
        Wt::Dbo::ptr<AlertMessageAliasInformationCriteria> aliasCriteria = this->_session.find<AlertMessageAliasInformationCriteria>()
                .where("\"AIC_DELETE\" IS NULL")
                .where("\"URO_ID_URO_ID\" = ?").bind(this->role)
                .where("\"MED_ID_MED_ID\" = ?").bind(this->media)
                .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                .where("\"SEA_ID\" = ?").bind(this->vPathElements[5])
                .where("\"INF_VALUE_NUM\" = ?").bind(this->vPathElements[7])
                .where("\"INU_ID_INU_ID\" = ?").bind(this->vPathElements[9])
                .where("\"ACR_ID_ACR_ID\" = ?").bind(this->vPathElements[11]);
        if (aliasCriteria)
        {
            responseMsg = aliasCriteria->toJSON();
            res = 200;
            transaction.commit();
        }
        else
        {
            res = 404;
            responseMsg = "{\n\t\"message\":\"Alias not found\"\n}";
        }
        
        
    } 
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << "[Plugin Ressource] " << e.what();
        res = 503;
        responseMsg = "{\n\t\"message\": \"Service Unavailable\"\n}";
    }
    
    
    return res;
}

unsigned short PluginResource::getCriteriaForInformation(string &responseMsg)
{
    unsigned short res = 500;
    unsigned idx = 0;
    try 
    {
        Wt::Dbo::Transaction transaction(_session);
        Wt::Dbo::collection<Wt::Dbo::ptr<AlertCriteria>> informationAlertCriteria = this->_session.find<AlertCriteria>()
                .where("\"ACR_DELETE\" IS NULL");

        if(informationAlertCriteria.size() > 0 )
        {
            responseMsg = "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<AlertCriteria> >::const_iterator i = informationAlertCriteria.begin(); i != informationAlertCriteria.end(); i++) 
            { 
                AlertCriteria acr(*i->get());
                acr.setId(i->id());
                responseMsg +=  acr.toJSON();
                idx++;
                if(informationAlertCriteria.size()-idx > 0)
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
            responseMsg = "{\"message\":\"Criteria not found\"}";
        }
        transaction.commit();
    } 
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << "[Plugin Ressource] " << e.what();
        res = 503;
        responseMsg = "{\n\t\"message\": \"Service Unavailable\"\n}";
    }
    return res;
}

unsigned short PluginResource::getAliasForPlugin(string &responseMsg)
{
    unsigned short res = 500;
    if (this->role.empty())
    {
        res = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        return res;
    }

    if (this->media.empty())
    {
        res = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        return res;
    }
    
    try 
    {
        Wt::Dbo::Transaction transaction(_session);
        Wt::Dbo::ptr<AlertMessageAliasPlugin> aliasPlugin = this->_session.find<AlertMessageAliasPlugin>()
                .where("\"AAP_DELETE\" IS NULL")
                .where("\"URO_ID_URO_ID\" = ?").bind(this->role)
                .where("\"MED_ID_MED_ID\" = ?").bind(this->media)
                .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1]);
        if (aliasPlugin)
        {
            responseMsg = aliasPlugin->toJSON();
            res = 200;
            transaction.commit();
        }
        else
        {
            res = 404;
            responseMsg = "{\n\t\"message\":\"Alias not found\"\n}";
        }
        
        
    } 
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << "[Plugin Ressource] " << e.what();
        res = 503;
        responseMsg = "{\n\t\"message\": \"Service Unavailable\"\n}";
    }
    
    
    return res;
}

unsigned short PluginResource::getInformationListForPlugin(string &responseMsg)
{
    unsigned short res = 500;
    unsigned idx = 0;
    if((res = pluginIsAccessible(responseMsg)) == 200)
    {
        try
        {
            Wt::Dbo::Transaction transaction(_session);

            Wt::Dbo::collection <Wt::Dbo::ptr<Information2>> information = _session.find<Information2>()
                    .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                    .where("\"INF_DELETE\" IS NULL")
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
    }
    return res;  
}

unsigned short PluginResource::getSearchForSourceAndPlugin(string& responseMsg)
{
    unsigned short res = 500;
    if((res = pluginIsAccessible(responseMsg)) == 200)
    {
        unsigned idx = 0;
        try
        {
            Wt::Dbo::Transaction transaction(_session);
            Wt::Dbo::collection<Wt::Dbo::ptr<Search>> seaCollec = _session.find<Search>()
                    .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                    .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                    .where("\"SEA_DELETE\" IS NULL")
                    .orderBy("\"SEA_ID\"");           
            if (seaCollec.size() > 0)
            {
                responseMsg += "[\n";
                for (Wt::Dbo::collection<Wt::Dbo::ptr<Search> >::const_iterator i = seaCollec.begin(); i != seaCollec.end(); i++) 
                {
                    responseMsg += "\t" + i->get()->toJSON();
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
    }
    return res;
}

unsigned short PluginResource::getSearchForSearchIdForSourceAndPlugin(string& responseMsg)
{
    unsigned short res = 500;
    if((res = pluginIsAccessible(responseMsg)) == 200)
    {
        try
        {
            Wt::Dbo::Transaction transaction(_session);
            Wt::Dbo::ptr<Search> seaCollec = _session.find<Search>()
                    .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                    .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                    .where("\"SEA_ID\" = ?").bind(this->vPathElements[5])
                    .where("\"SEA_DELETE\" IS NULL")
                    .orderBy("\"SEA_ID\"");           
            if (seaCollec)
            {
                responseMsg = "\t" + seaCollec.modify()->toJSON();             

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
    }
    return res;
}


unsigned short PluginResource::getParameterValueForSearch(string &responseMsg)
{
    unsigned short res = 500;
    if((res = pluginIsAccessible(responseMsg)) == 200)
    {
        unsigned idx = 0;
        try
        {
            Wt::Dbo::Transaction transaction(_session);
            Wt::Dbo::collection<Wt::Dbo::ptr<SearchParameterValue>> seaParamCollec = _session.find<SearchParameterValue>()
                    .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                    .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                    .where("\"SEA_ID\" = ?").bind(this->vPathElements[5]);
            if (seaParamCollec.size() > 0)
            {
                responseMsg += "[\n";
                for (Wt::Dbo::collection<Wt::Dbo::ptr<SearchParameterValue> >::const_iterator i = seaParamCollec.begin(); i != seaParamCollec.end(); i++) 
                {
                    responseMsg += "\t" + i->get()->toJSON();
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
    }
    return res;
}

unsigned short PluginResource::getInformationForSeaSrcAndPlg(string& responseMsg)
{
    unsigned short res = 500;
    if((res = pluginIsAccessible(responseMsg)) == 200)
    {
        unsigned idx = 0;
        try
        {
            Wt::Dbo::Transaction transaction(_session);
            Wt::Dbo::collection < Wt::Dbo::ptr < Information2 >> infCollec = _session.find<Information2>()
                    .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
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
    }
    return res;
}

unsigned short PluginResource::getSourceForPlugin(string& responseMsg)
{
    unsigned short res = 500;
    if((res = pluginIsAccessible(responseMsg)) == 200)
    {
        unsigned idx = 0;
        try
        {
            Wt::Dbo::Transaction transaction(_session);
            Wt::Dbo::collection < Wt::Dbo::ptr < Source >> srcCollec = _session.find<Source>()
                    .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                    .where("\"SRC_DELETE\" IS NULL")
                    .orderBy("\"SRC_ID\"");

            if (srcCollec.size() > 0)
            {
                responseMsg += "[\n";
                for (Wt::Dbo::collection<Wt::Dbo::ptr<Source> >::const_iterator i = srcCollec.begin(); i != srcCollec.end(); i++) 
                {
                    responseMsg += "\t" + i->get()->toJSON();
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
    }
    return res;
}

unsigned short PluginResource::getParameterValueForSource(string& responseMsg)
{
    unsigned short res = 500;
    if((res = pluginIsAccessible(responseMsg)) == 200)
    {
        unsigned idx = 0;
        try
        {
            Wt::Dbo::Transaction transaction(_session);
            Wt::Dbo::collection < Wt::Dbo::ptr < SourceParameterValue >> srcParamCollec = _session.find<SourceParameterValue>()
                    .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                    .where("\"SRC_ID\" = ?").bind(this->vPathElements[3]);

            if (srcParamCollec.size() > 0)
            {
                responseMsg += "[\n";
                for (Wt::Dbo::collection<Wt::Dbo::ptr<SourceParameterValue> >::const_iterator i = srcParamCollec.begin(); i != srcParamCollec.end(); i++) 
                {
                    responseMsg += "\t" + i->get()->toJSON();
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
    }
    return res;
}

void PluginResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";
    
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->statusCode = getPlugin(responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();

            if(!nextElement.compare(""))
            {
                this->statusCode = getPluginJSON(responseMsg);
            }
            else if(!nextElement.compare("alias"))
            {
                this->statusCode = getAliasForPlugin(responseMsg);
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
                    boost::lexical_cast<unsigned long long>(nextElement);
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
                            boost::lexical_cast<unsigned long long>(nextElement);
                            nextElement = getNextElementFromPath();
                            if(!nextElement.compare(""))
                            {
                                this->statusCode = getSearchForSearchIdForSourceAndPlugin(responseMsg);
                            }
                            else if(!nextElement.compare("parameters"))
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
                            else if(!nextElement.compare("inf_values"))
                            {
                                nextElement = getNextElementFromPath();
                                boost::lexical_cast<unsigned long long>(nextElement);
                                nextElement = getNextElementFromPath();
                                if(!nextElement.compare("units"))
                                {
                                    nextElement = getNextElementFromPath();
                                    boost::lexical_cast<unsigned long long>(nextElement);
                                    nextElement = getNextElementFromPath();
                                    if(!nextElement.compare("alias"))
                                    {
                                        this->statusCode = getAliasForInformation(responseMsg);
                                    }
                                    else if (!nextElement.compare("criteria"))
                                    {
                                        nextElement = getNextElementFromPath();
                                        if(!nextElement.compare(""))
                                        {
                                            this->statusCode = getCriteriaForInformation(responseMsg);
                                        }
                                        else
                                        {
                                            boost::lexical_cast<unsigned long long>(nextElement);
                                            nextElement = getNextElementFromPath();
                                            if(!nextElement.compare("alias"))
                                            {
                                                this->statusCode = getAliasForCriteria(responseMsg);
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


unsigned short PluginResource::postPlugin(string& responseMsg, const string& sRequest)
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
        Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON:" << sRequest;
        return res;
    }
    catch (Wt::Json::TypeException const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
        Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON.:" << sRequest;
        return res;
    }   
    try 
    {
        Wt::Dbo::Transaction transaction(_session);
        Plugin *plugin = new Plugin();
        plugin->name = plgName;
        plugin->desc = plgDesc;
        
        Wt::Dbo::ptr<Plugin> plgPtr = _session.add<Plugin>(plugin);
       
        plgPtr.flush();
        plgPtr.modify()->setId(plgPtr.id());
        responseMsg = plgPtr->toJSON();
        
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

unsigned short PluginResource::postSourceForPlugin(string& responseMsg, const string& sRequest)
{
    unsigned short res = 500;
    if((res = pluginIsAccessible(responseMsg)) == 200)
    {
        long long addonId;

        try
        {
            Wt::Json::Object result;                   
            Wt::Json::parse(sRequest, result);

            //descriptif

            addonId = result.get("addon_id");
            try
            {
                Wt::Dbo::Transaction transaction(_session);

                // creer l'id de la source
                Wt::Dbo::collection<Wt::Dbo::ptr<Source>> srcCollec = _session.find<Source>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1]);
                long long srcId = 1;
                if(srcCollec.size()> 0)
                {
                    string queryStr = "SELECT MAX(\"SRC_ID\") FROM \"T_SOURCE_SRC\" src "
                                       " WHERE \"PLG_ID_PLG_ID\" = " + boost::lexical_cast<string > (this->vPathElements[1]);

                    Wt::Dbo::Query<long long> queryResult = _session.query<long long>(queryStr);
                    srcId = queryResult + 1;
                }

                //creation de la source
                Wt::Dbo::ptr<Plugin> plgPtr = _session.find<Plugin>().where("\"PLG_ID\" = ?").bind(this->vPathElements[1]);
                Wt::Dbo::ptr<Addon> addonPtr = _session.find<Addon>().where("\"ADO_ID\" = ?").bind(addonId);
                Source *source = new Source;
                source->pk.id = srcId;
                source->pk.plugin = plgPtr;
                source->addon = addonPtr;
                Wt::Dbo::ptr<Source> srcPtr = _session.add<Source>(source);
                srcPtr.flush();


                Wt::Dbo::collection<Wt::Dbo::ptr<SourceParameter> > sourceParameterPtr = addonPtr.get()->sourceParameters;

                //creations des parametres
                for (Wt::Dbo::collection<Wt::Dbo::ptr<SourceParameter> >::const_iterator i = sourceParameterPtr.begin(); i != sourceParameterPtr.end(); i++)
                {
                    SourceParameterValue *sourceParameterValue = new SourceParameterValue;

                    //recuperation des "source_parameters" dans le JSON
                    Wt::WString tmp = result.get(boost::lexical_cast<string>(i->get()->name));

                    sourceParameterValue->value = tmp;
                    sourceParameterValue->name = i->get()->name;
                    sourceParameterValue->pk.sourceParameter = *i;
                    sourceParameterValue->pk.source = srcPtr;
                    _session.add<SourceParameterValue>(sourceParameterValue);
                }
                res = 200;
                responseMsg = srcPtr->toJSON();
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
            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON:" << sRequest;
        }
        catch (Wt::Json::TypeException const& e)
        {
            res = 400;
            responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON.:" << sRequest;
        }   
    }
    return res;
}

unsigned short PluginResource::postSearchForSourceAndPlugin(string& responseMsg, const string& sRequest)
{
    unsigned short res = 500;
    if((res = pluginIsAccessible(responseMsg)) == 200)
    {
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
            if(units.size() <= (unsigned)nbValue)
            {
                try
                {
                    Wt::Dbo::Transaction transaction(_session);
                    Wt::Dbo::ptr<Search> seaPtr;
                    //verif si le search type va avec l'addon 

                    string queryStr = "SELECT set FROM \"T_SEARCH_TYPE_STY\" set "
                                           " WHERE \"STY_ID\" IN"
                                            "("
                                                "SELECT \"T_SEARCH_TYPE_STY_STY_ID\" FROM \"TJ_ADO_STY\" "
                                                "WHERE \"T_ADDON_ADO_ADO_ID\" = " 
                                                "("
                                                    "SELECT \"SRC_ADO_ADO_ID\" FROM \"T_SOURCE_SRC\" "
                                                    "WHERE \"PLG_ID_PLG_ID\" = " + this->vPathElements[1] +
                                                    "AND \"SRC_ID\" =  " + this->vPathElements[3] + 
                                                ")"
                                            ")"
                                            "AND \"STY_ID\" = " + boost::lexical_cast<string>(styId);

                    Wt::Dbo::Query<Wt::Dbo::ptr<SearchType> > queryRes = _session.query<Wt::Dbo::ptr<SearchType> >(queryStr);

                    Wt::Dbo::collection<Wt::Dbo::ptr<SearchType> > seaTypePtr = queryRes.resultList();

                    if(seaTypePtr.size() == 0)
                    {
                            res = 404;
                            responseMsg = "{\"message\":\"Search Type not found.\"}";
                            return res;
                    }
                    // creer l'id de la search
                    Wt::Dbo::collection <Wt::Dbo::ptr<Search>> seaCollec = _session.find<Search>()
                            .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                            .where("\"SRC_ID\" = ?").bind(this->vPathElements[3]);
                    long long seaId = 1;
                    if (seaCollec.size() > 0)
                    {
                        string queryStr = "SELECT MAX(\"SEA_ID\") FROM \"T_SEARCH_SEA\" sea "
                                               " WHERE \"PLG_ID_PLG_ID\" = " + this->vPathElements[1] +
                                               " AND \"SRC_ID\" = " + this->vPathElements[3];

                        Wt::Dbo::Query<long long> queryResult = _session.query<long long>(queryStr);
                        seaId = queryResult + 1;    
                    }

                    //creation de la search
                    Wt::Dbo::ptr<Source> srcPtr = _session.find<Source>()
                            .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                            .where("\"SRC_ID\" = ?").bind(this->vPathElements[3]);
                    if(srcPtr)
                    {
                        Wt::Dbo::ptr<SearchType> seaTypPtr = _session.find<SearchType>().where("\"STY_ID\" = ?").bind(styId);
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
                            seaPtr = _session.add<Search>(search);
                            seaPtr.flush();


                            Wt::Dbo::collection<Wt::Dbo::ptr<SearchParameter> > searchParameterPtr = seaTypPtr->searchParameters;
                            //creations des parametres
                            for (Wt::Dbo::collection<Wt::Dbo::ptr<SearchParameter>>::const_iterator i = searchParameterPtr.begin(); i != searchParameterPtr.end(); i++)
                            {
                                SearchParameterValue *searchParameterValue = new SearchParameterValue;

                                //recuperation des "search_parameters" dans le JSON
                                Wt::WString tmp = result.get(i->get()->name.toUTF8());

                                searchParameterValue->value = tmp;
                                searchParameterValue->name = i->get()->name;
                                searchParameterValue->searchParameterValueId.searchParameter = *i;
                                searchParameterValue->searchParameterValueId.search = seaPtr;
                                _session.add<SearchParameterValue>(searchParameterValue);
                            }

                            //liée aux valeurs recherchées une unité
                            for (Wt::Json::Array::const_iterator idx1 = units.begin() ; idx1 < units.end(); idx1++)
                            {
                                Wt::Json::Object tmp = *idx1;
                                int valNumUnit = tmp.get("val_num");
                                int unitId = tmp.get("unit_id");
                                Wt::Dbo::ptr<InformationUnit> informationUnitPtr = _session.find<InformationUnit>().where("\"INU_ID\" = ?").bind(unitId);
                                if (informationUnitPtr && valNumUnit <= nbValue)
                                {
                                    SearchUnit *searchUnit = new SearchUnit;
                                    searchUnit->pk.infValueNum = valNumUnit;
                                    searchUnit->pk.search = seaPtr;
                                    searchUnit->informationUnit = informationUnitPtr;
                                    _session.add<SearchUnit>(searchUnit);
                                }
                                else
                                {
                                    Wt::log("info") << "[Plugin Ressource] info non trouvée ou valNum non autorisée";
                                    res = 400;
                                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}"; 
                                    return res;
                                }
                            }
                            responseMsg = seaPtr->toJSON();
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
                 Wt::log("info") << "[Plugin Ressource] unité sur val_num non declarée";
                 this->statusCode = 400;
                 responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";  
            }
        }
        catch (Wt::Json::ParseError const& e)
        {
            res = 400;
            responseMsg = "{\"message\":\"Problems parsing JSON\"}";
            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON:" << sRequest;
        }
        catch (Wt::Json::TypeException const& e)
        {
            res = 400;
            responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
            Wt::log("warning") << "[PluginRessource] Problems parsing JSON.:" << sRequest;
        }   
    }
    return res;
}

unsigned short PluginResource::postInformationForSeaSrcAndPlg(string& responseMsg, const string& sRequest)
{
    unsigned short res = 500;
    if((res = pluginIsAccessible(responseMsg)) == 200)
    {
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
                Wt::Dbo::Transaction transaction(_session);

                //search exist ?
                Wt::Dbo::ptr<Search> seaPtr = _session.find<Search>()
                        .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                        .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                        .where("\"SEA_ID\" = ?").bind(this->vPathElements[5]);

                if(seaPtr)
                {
                    //Relier une unité à l'info
                    // unit exist?
                    Wt::Dbo::ptr<SearchUnit> seaUnitPtr = _session.find<SearchUnit>()
                            .where("\"INF_VALUE_NUM\" = ?").bind(valueNum)
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
                        Wt::Dbo::ptr<Information2> infPtr = _session.add<Information2>(information);
                        infPtr.flush();
                        responseMsg = infPtr->toJSON();
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
            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON:" << sRequest;
        }
        catch (Wt::Json::TypeException const& e)
        {
            res = 400;
            responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON.:" << sRequest;
        }   
    }
    return res; 
}

void PluginResource::processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "", sRequest = "";

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
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();
            if(!nextElement.compare("sources"))
            {
                nextElement = getNextElementFromPath();
                if (!nextElement.compare(""))
                {
                    this->statusCode = postSourceForPlugin(responseMsg, sRequest);
                }
                else
                {
                    boost::lexical_cast<unsigned long long>(nextElement);

                    nextElement = getNextElementFromPath();
                    
                    ///// patch
                    if(!nextElement.compare("parameters"))
                    {
                        this->statusCode = patchParametersSourceForPlugin(responseMsg, sRequest);
                    }
                    
                    ////patch
                    else if(!nextElement.compare("searches"))
                    {
                        nextElement = getNextElementFromPath();
                        if (!nextElement.compare(""))
                        {
                            this->statusCode = postSearchForSourceAndPlugin(responseMsg, sRequest);
                        }
                        else
                        {
                            boost::lexical_cast<unsigned long long>(nextElement);
                            nextElement = getNextElementFromPath();
                            ///patch
                            if(!nextElement.compare(""))
                            {
                                this->statusCode = patchSearchForSourceAndPlugin(responseMsg, sRequest);
                            }
                            
                            ///patch
                            else if(!nextElement.compare("informations"))
                            {
                                nextElement = getNextElementFromPath();
                                if (!nextElement.compare(""))
                                {
                                    this->statusCode = postInformationForSeaSrcAndPlg(responseMsg, sRequest);
                                }
                                else
                                {
                                    ///patch
                                    boost::lexical_cast<unsigned long long>(nextElement);
                                    nextElement = getNextElementFromPath();
                                    
                                    if(!nextElement.compare(""))
                                    {
                                        this->statusCode = patchSearchForSourceAndPlugin(responseMsg, sRequest);
                                    }
                                    else
                                    {
                                        this->statusCode = 400;
                                        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                                    }
                                    ///patch
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
    string responseMsg = "", nextElement = "";
    string sRequest = request2string(request);
    
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
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();

            if (!nextElement.compare("sources"))
            {
                nextElement = getNextElementFromPath();
                boost::lexical_cast<unsigned long long>(nextElement);
                nextElement = getNextElementFromPath();
                if (!nextElement.compare("searches"))
                {
                    nextElement = getNextElementFromPath();
                    boost::lexical_cast<unsigned long long>(nextElement);
                    nextElement = getNextElementFromPath();
                    if(!nextElement.compare("inf_values"))
                    {
                        nextElement = getNextElementFromPath();
                        boost::lexical_cast<unsigned long long>(nextElement);
                        nextElement = getNextElementFromPath();
                        if(!nextElement.compare("units"))
                        {
                            nextElement = getNextElementFromPath();
                            boost::lexical_cast<unsigned long long>(nextElement);
                            nextElement = getNextElementFromPath();
                            if(!nextElement.compare("criteria"))
                            {
                                nextElement = getNextElementFromPath();
                                boost::lexical_cast<unsigned long long>(nextElement);
                                nextElement = getNextElementFromPath();
                                if(!nextElement.compare("alias"))
                                {
                                    this->statusCode = putAliasForCriterion(responseMsg,sRequest);
                                }
                                else
                                {
                                    this->statusCode = 400;
                                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                                }
                            }
                            else if(!nextElement.compare("alias"))
                            {
                                this->statusCode = putAliasForInformation(responseMsg,sRequest);
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
            else if (!nextElement.compare("alias"))
            {
                this->statusCode = putAliasForPlugin(responseMsg,sRequest);
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

unsigned short PluginResource::putAliasForInformation(string &responseMsg, const string &sRequest)
{
    unsigned short res = 500;
    Wt::WString sRole;
    Wt::WString sMedia;
    Wt::WString sValue;
    if((res = pluginIsAccessible(responseMsg)) == 200)
    {
        try
        {
            Wt::Json::Object result;                   
            Wt::Json::parse(sRequest, result);

            //information
            Wt::Json::Object alias = result.get("alias");
            sRole = alias.get("role");
            sMedia = alias.get("media");
            sValue = alias.get("value");

            try
            {
                Wt::Dbo::Transaction transaction(_session);

                // Information exist?
                Wt::Dbo::ptr<Information2> infPtr = _session.find<Information2>()
                        .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                        .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                        .where("\"SEA_ID\" = ?").bind(this->vPathElements[5])
                        .where("\"INF_VALUE_NUM\" = ?").bind(this->vPathElements[7])
                        .where("\"INU_ID_INU_ID\" = ?").bind(this->vPathElements[9])
                        ;
                if(infPtr)
                {
                    //Relier une unité à l'info
                    // unit exist?
                    Wt::Dbo::ptr<UserRole> ptrRole = this->_session.find<UserRole>()
                        .where("\"URO_ID\" = ?").bind(sRole)
                        .where("\"URO_DELETE\" IS NULL");
                
                    if (!ptrRole)
                    {
                        res = 404;
                        responseMsg = "{\n\t\"message\":\"Role not found\"\n}";
                        return res;
                    }

                    Wt::Dbo::ptr<Media> ptrMedia = this->_session.find<Media>()
                            .where("\"MED_ID\" = ?").bind(sMedia)
                            .where("\"MED_DELETE\" IS NULL");

                    if (!ptrMedia)
                    {
                        res = 404;
                        responseMsg = "{\n\t\"message\":\"Media not found\"\n}";
                        return res;
                    }


                    Wt::Dbo::ptr<AlertMessageAliasInformation> ptrInformationAlias = this->_session.find<AlertMessageAliasInformation>()
                            .where("\"URO_ID_URO_ID\" = ?").bind(sRole)
                            .where("\"SEA_ID\" = ?").bind(infPtr->pk.search->pk.id)
                            .where("\"SRC_ID\" = ?").bind(infPtr->pk.search->pk.source->pk.id)
                            .where("\"PLG_ID_PLG_ID\" = ?").bind(infPtr->pk.search->pk.source->pk.plugin.id())
                            .where("\"INF_VALUE_NUM\" = ?").bind(infPtr->pk.subSearchNumber)
                            .where("\"INU_ID_INU_ID\" = ?").bind(infPtr->pk.unit.id())
                            .where("\"MED_ID_MED_ID\" = ?").bind(sMedia);
                    if (ptrInformationAlias) 
                    {
                        ptrInformationAlias.modify()->alias = sValue;
                    }
                    else
                    {
                        AlertMessageAliasInformation *newInformationAlias = new AlertMessageAliasInformation();
                        newInformationAlias->pk.information = infPtr;
                        newInformationAlias->pk.userRole = ptrRole;
                        newInformationAlias->pk.media = ptrMedia;
                        newInformationAlias->alias = sValue;
                        ptrInformationAlias = this->_session.add<AlertMessageAliasInformation>(newInformationAlias);
                    }
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
        }
        catch (Wt::Json::ParseError const& e)
        {
            res = 400;
            responseMsg = "{\"message\":\"Problems parsing JSON\"}";
            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON:" << sRequest;
        }
        catch (Wt::Json::TypeException const& e)
        {
            res = 400;
            responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON.:" << sRequest;
        }   
    }
    return res; 
}

unsigned short PluginResource::putAliasForCriterion(string &responseMsg, const string &sRequest)
{
    unsigned short res = 500;
    Wt::WString sRole;
    Wt::WString sMedia;
    Wt::WString sValue;
    if((res = pluginIsAccessible(responseMsg)) == 200)
    {
        try
        {
            Wt::Json::Object result;                   
            Wt::Json::parse(sRequest, result);

            //information
            Wt::Json::Object alias = result.get("alias");
            sRole = alias.get("role");
            sMedia = alias.get("media");
            sValue = alias.get("value");

            try
            {
                Wt::Dbo::Transaction transaction(_session);

                // Information exist?
                Wt::Dbo::ptr<Information2> infPtr = _session.find<Information2>()
                        .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                        .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                        .where("\"SEA_ID\" = ?").bind(this->vPathElements[5])
                        .where("\"INF_VALUE_NUM\" = ?").bind(this->vPathElements[7])
                        .where("\"INU_ID_INU_ID\" = ?").bind(this->vPathElements[9])
                        ;
                if(infPtr)
                {
                    //Relier une unité à l'info
                    // unit exist?
                    Wt::Dbo::ptr<UserRole> ptrRole = this->_session.find<UserRole>()
                        .where("\"URO_ID\" = ?").bind(sRole)
                        .where("\"URO_DELETE\" IS NULL");
                
                    if (!ptrRole)
                    {
                        res = 404;
                        responseMsg = "{\n\t\"message\":\"Role not found\"\n}";
                        return res;
                    }

                    Wt::Dbo::ptr<Media> ptrMedia = this->_session.find<Media>()
                            .where("\"MED_ID\" = ?").bind(sMedia)
                            .where("\"MED_DELETE\" IS NULL");

                    if (!ptrMedia)
                    {
                        res = 404;
                        responseMsg = "{\n\t\"message\":\"Media not found\"\n}";
                        return res;
                    }
                    
                    Wt::Dbo::ptr<AlertCriteria> ptrCriterion = this->_session.find<AlertCriteria>()
                            .where("\"ACR_ID\" = ?").bind(this->vPathElements[11])
                            .where("\"ACR_DELETE\" IS NULL");

                    if (!ptrCriterion)
                    {
                        res = 404;
                        responseMsg = "{\n\t\"message\":\"Criterion not found\"\n}";
                        return res;
                    }


                    Wt::Dbo::ptr<AlertMessageAliasInformationCriteria> ptrInformationCriteriaAlias = this->_session.find<AlertMessageAliasInformationCriteria>()
                            .where("\"URO_ID_URO_ID\" = ?").bind(sRole)
                            .where("\"SEA_ID\" = ?").bind(infPtr->pk.search->pk.id)
                            .where("\"SRC_ID\" = ?").bind(infPtr->pk.search->pk.source->pk.id)
                            .where("\"PLG_ID_PLG_ID\" = ?").bind(infPtr->pk.search->pk.source->pk.plugin.id())
                            .where("\"INF_VALUE_NUM\" = ?").bind(infPtr->pk.subSearchNumber)
                            .where("\"INU_ID_INU_ID\" = ?").bind(infPtr->pk.unit.id())
                            .where("\"MED_ID_MED_ID\" = ?").bind(sMedia)
                            .where("\"ACR_ID_ACR_ID\" = ?").bind(ptrCriterion.id());
                    if (ptrInformationCriteriaAlias) 
                    {
                        ptrInformationCriteriaAlias.modify()->alias = sValue;
                    }
                    else
                    {
                        AlertMessageAliasInformationCriteria *newInformationCriteriaAlias = new AlertMessageAliasInformationCriteria();
                        newInformationCriteriaAlias->pk.information = infPtr;
                        newInformationCriteriaAlias->pk.userRole = ptrRole;
                        newInformationCriteriaAlias->pk.media = ptrMedia;
                        newInformationCriteriaAlias->pk.alertCriteria = ptrCriterion;
                        newInformationCriteriaAlias->alias = sValue;
                        ptrInformationCriteriaAlias = this->_session.add<AlertMessageAliasInformationCriteria>(newInformationCriteriaAlias);
                    }
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
        }
        catch (Wt::Json::ParseError const& e)
        {
            res = 400;
            responseMsg = "{\"message\":\"Problems parsing JSON\"}";
            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON:" << sRequest;
        }
        catch (Wt::Json::TypeException const& e)
        {
            res = 400;
            responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON.:" << sRequest;
        }   
    }
    return res; 
}

unsigned short PluginResource::putAliasForPlugin(string &responseMsg, const string &sRequest)
{
    unsigned short res = 500;
    Wt::WString sRole;
    Wt::WString sMedia;
    Wt::WString sValue;
    if((res = pluginIsAccessible(responseMsg)) == 200)
    {
        try
        {
            Wt::Json::Object result;                   
            Wt::Json::parse(sRequest, result);

            //information
            Wt::Json::Object alias = result.get("alias");
            sRole = alias.get("role");
            sMedia = alias.get("media");
            sValue = alias.get("value");

            try
            {
                Wt::Dbo::Transaction transaction(_session);

                // Information exist?
                Wt::Dbo::ptr<Plugin> infPlg = _session.find<Plugin>()
                        .where("\"PLG_ID\" = ?").bind(this->vPathElements[1])

                        ;
                if(infPlg)
                {
                    //Relier une unité à l'info
                    // unit exist?
                    Wt::Dbo::ptr<UserRole> ptrRole = this->_session.find<UserRole>()
                        .where("\"URO_ID\" = ?").bind(sRole)
                        .where("\"URO_DELETE\" IS NULL");
                
                    if (!ptrRole)
                    {
                        res = 404;
                        responseMsg = "{\n\t\"message\":\"Role not found\"\n}";
                        return res;
                    }

                    Wt::Dbo::ptr<Media> ptrMedia = this->_session.find<Media>()
                            .where("\"MED_ID\" = ?").bind(sMedia)
                            .where("\"MED_DELETE\" IS NULL");

                    if (!ptrMedia)
                    {
                        res = 404;
                        responseMsg = "{\n\t\"message\":\"Media not found\"\n}";
                        return res;
                    }


                    Wt::Dbo::ptr<AlertMessageAliasPlugin> ptrPluginAlias = this->_session.find<AlertMessageAliasPlugin>()
                            .where("\"URO_ID_URO_ID\" = ?").bind(sRole)
                            .where("\"PLG_ID_PLG_ID\" = ?").bind(infPlg.id())
                            .where("\"MED_ID_MED_ID\" = ?").bind(sMedia);
                    if (ptrPluginAlias) 
                    {
                        ptrPluginAlias.modify()->alias = sValue;
                    }
                    else
                    {
                        AlertMessageAliasPlugin *newPluginAlias = new AlertMessageAliasPlugin();
                        newPluginAlias->pk.plugin = infPlg;
                        newPluginAlias->pk.userRole = ptrRole;
                        newPluginAlias->pk.media = ptrMedia;
                        newPluginAlias->alias = sValue;
                        ptrPluginAlias = this->_session.add<AlertMessageAliasPlugin>(newPluginAlias);
                    }
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
        }
        catch (Wt::Json::ParseError const& e)
        {
            res = 400;
            responseMsg = "{\"message\":\"Problems parsing JSON\"}";
            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON:" << sRequest;
        }
        catch (Wt::Json::TypeException const& e)
        {
            res = 400;
            responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON.:" << sRequest;
        }   
    }
    return res; 
}

unsigned short PluginResource::patchInformationForSeaSrcAndPlg(string &responseMsg, const string &sRequest)
{
    unsigned short res = 500;
    if((res = pluginIsAccessible(responseMsg)) == 200)
    {
        Wt::WString infName, infCalculate;
        bool infDisplay;

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
                Wt::Dbo::Transaction transaction(_session);

                // Information exist?
                Wt::Dbo::ptr<Information2> infPtr = _session.find<Information2>()
                        .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                        .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                        .where("\"SEA_ID\" = ?").bind(this->vPathElements[5])
                        .where("\"INF_VALUE_NUM\" = ?").bind(this->vPathElements[7])
                        .where("\"INU_ID_INU_ID\" = ?").bind(this->vPathElements[9]);
                if(infPtr)
                {
                    //Relier une unité à l'info
                    // unit exist?
                    Wt::Dbo::ptr<SearchUnit> seaUnitPtr = _session.find<SearchUnit>()
                            .where("\"INF_VALUE_NUM\" = ?").bind(this->vPathElements[7])
                            .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                            .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                            .where("\"SEA_ID\" = ?").bind(this->vPathElements[5]);
                    if(seaUnitPtr)
                    {

                        //creation info
                        infPtr.modify()->name = infName;
                        infPtr.modify()->display = infDisplay;
                        infPtr.modify()->calculate = infCalculate;
                        responseMsg = infPtr->toJSON();
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
            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON:" << sRequest;
        }
        catch (Wt::Json::TypeException const& e)
        {
            res = 400;
            responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON.:" << sRequest;
        }   
    }
    return res; 
}

unsigned short PluginResource::patchSearchForSourceAndPlugin(string& responseMsg, const string& sRequest)
{
    unsigned short res = 500;
    if((res = pluginIsAccessible(responseMsg)) == 200)
    {
        bool seaIsStatic;
        int posKeyValue, seaPeriod;

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
                Wt::Dbo::Transaction transaction(_session);
                Wt::Dbo::ptr<Search> seaPtr;

                //modification de la search si elle exist
                seaPtr = _session.find<Search>()
                        .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                        .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                        .where("\"SEA_ID\" = ?").bind(this->vPathElements[5]);
                if(seaPtr)
                {
                    Wt::Dbo::ptr<SearchType> seaTypPtr = _session.find<SearchType>().where("\"STY_ID\" = ?").bind(seaPtr->searchType.id());
                    if(seaTypPtr)
                    {

                        seaPtr.modify()->period = seaPeriod;
                        seaPtr.modify()->pos_key_value = posKeyValue;
                        seaPtr.modify()->searchIsStatic = seaIsStatic;

                        Wt::Dbo::collection<Wt::Dbo::ptr<SearchParameter> > searchParameterPtr = seaTypPtr->searchParameters;
                        //modification des parametres
                        for (Wt::Dbo::collection<Wt::Dbo::ptr<SearchParameter> >::const_iterator i = searchParameterPtr.begin(); i != searchParameterPtr.end(); i++)
                        {
                            Wt::Dbo::ptr<SearchParameterValue> searchParameterValuePtr = _session.find<SearchParameterValue>()
                                    .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                    .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                                    .where("\"SEA_ID\" = ?").bind(this->vPathElements[5])
                                    .where("\"SEP_ID_SEP_ID\" = ?").bind(i->id());

                            //recuperation des "search_parameters" dans le JSON
                            Wt::WString tmp = result.get(i->get()->name.toUTF8());

                            searchParameterValuePtr.modify()->value = tmp;
                        }
                        res = 200;
                        responseMsg = seaPtr->toJSON();
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
            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON:" << sRequest;
        }
        catch (Wt::Json::TypeException const& e)
        {
            res = 400;
            responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON.:" << sRequest;
        }   
    }
    return res;
}

unsigned short PluginResource::patchParametersSourceForPlugin(string &responseMsg, const string &sRequest)
{
    unsigned short res = 500;
    if((res = pluginIsAccessible(responseMsg)) == 200)
    {
        Wt::Json::Array& parameters = Wt::Json::Array::Empty;

        try
        {
            Wt::Json::Object result;                   
            Wt::Json::parse(sRequest, result);
            parameters = result.get("parameters");
            try
            {
                Wt::Dbo::Transaction transaction(_session);
                Wt::Dbo::ptr<Source> srcPtr = _session.find<Source>()
                        .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                        .where("\"SRC_ID\" = ?").bind(this->vPathElements[3]);

                if(srcPtr)
                {
                    //modification des parametres
                    Wt::Dbo::ptr<SourceParameterValue> srcParamValPtr;
                    for (Wt::Json::Array::const_iterator idx1 = parameters.begin() ; idx1 < parameters.end(); idx1++)
                    {
                        Wt::Json::Object tmp = *idx1;
                        int srpIdParam = tmp.get("srp_id");
                        Wt::WString valueParam = tmp.get("value");


                        srcParamValPtr = _session.find<SourceParameterValue>()
                                .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
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
                        responseMsg += srcParamValPtr->toJSON();
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
            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON:" << sRequest;
        }
        catch (Wt::Json::TypeException const& e)
        {
            res = 400;
            responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON.:" << sRequest;
        }   
    }
    return res;
}

void PluginResource::processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
   string responseMsg = "", nextElement = "", sRequest = "";
    
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
            boost::lexical_cast<unsigned long long>(nextElement);

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
                    boost::lexical_cast<unsigned long long>(nextElement);
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

unsigned short PluginResource::deleteInformationForSeaSrcAndPlg(string& responseMsg)
{
    unsigned short res = 500;
    if((res = pluginIsAccessible(responseMsg)) == 200)
    {
        try 
        {  
            Wt::Dbo::Transaction transaction(_session);

            Wt::Dbo::ptr<Information2> informationPtr = _session.find<Information2>()
                    .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                    .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                    .where("\"SEA_ID\" = ?").bind(this->vPathElements[5])
                    .where("\"INF_VALUE_NUM\" = ?").bind(this->vPathElements[7])
                    .where("\"INU_ID_INU_ID\" = ?").bind(this->vPathElements[9]);
            //information exist ?
            if(informationPtr)
            {
                Wt::Dbo::collection<Wt::Dbo::ptr<AlertValue>> avaCollec = _session.find<AlertValue>()
                        .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
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

                    res = 204;
                }
                else
                {
                    res = 409;
                    responseMsg = "{\"message\":\"Conflict, an alert use this information\"}";
                }
            }
            else
            {
                responseMsg = "{\"message\":\"Information Not Found\"}";
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
    }    
    return res;
}

unsigned short PluginResource::deleteSearchForSourceAndPlugin(string& responseMsg)
{
    unsigned short res = 500;
    if((res = pluginIsAccessible(responseMsg)) == 200)
    {
        try 
        {  
            Wt::Dbo::Transaction transaction(_session);

            Wt::Dbo::ptr<Search> seaPtr = _session.find<Search>()
                    .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                    .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                    .where("\"SEA_ID\" = ?").bind(this->vPathElements[5]);
            //search exist ?
            if(seaPtr)
            {
                Wt::Dbo::collection<Wt::Dbo::ptr<Information2>> infCollec = _session.find<Information2>()
                        .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                        .where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                        .where("\"SEA_ID\" = ?").bind(this->vPathElements[5]);

                for (Wt::Dbo::collection<Wt::Dbo::ptr<Information2> >::const_iterator i = infCollec.begin(); i != infCollec.end(); i++) 
                {
                    string path = "/" + vPathElements[1] + "/sources/" + vPathElements[3] + "/searches/" + 
                                             vPathElements[5] + "/inf_values/" + boost::lexical_cast<string>(i->get()->pk.subSearchNumber) +
                                            "/units/" + boost::lexical_cast<string>(i->get()->pk.unit.id());
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
    }    
    return res;
    
}

unsigned short PluginResource::deletePlugin(string& responseMsg)
{
    unsigned short res = 500;
    if((res = pluginIsAccessible(responseMsg)) == 200)
    {
        try 
        {  
            Wt::Dbo::Transaction transaction(_session);
            Wt::Dbo::ptr<Plugin> plgPtr = _session.find<Plugin>().where("\"PLG_ID\" = ?").bind(this->vPathElements[1]);
            //verif si le plugin existe
            if(plgPtr)
            {
                Wt::Dbo::collection<Wt::Dbo::ptr<Source>> srcCollec = _session.find<Source>().where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1]);
                //verif si le plugin n'est pas utilisé
                for (Wt::Dbo::collection<Wt::Dbo::ptr<Source> >::const_iterator i = srcCollec.begin(); i != srcCollec.end(); i++) 
                {
                    string path = "/" + vPathElements[1] + "/sources/" + boost::lexical_cast<string>(i->get()->pk.id);
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
                res = 204;
            }
            else
            {
                res = 404;
                responseMsg = "{\"message\":\"Plugin not found\"}";
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
    return res;
}

unsigned short PluginResource::deleteSourceForPlugin(string& responseMsg)
{
    unsigned short res = 500;
    if((res = pluginIsAccessible(responseMsg)) == 200)
    {
        try 
        {  
            Wt::Dbo::Transaction transaction(_session);
            Wt::Dbo::ptr<Source> srcPtr = _session.find<Source>()
                    .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                    .where("\"SRC_ID\" = ?").bind(this->vPathElements[3]);
            //verif si la source existe
            if(srcPtr)
            {
                Wt::Dbo::collection<Wt::Dbo::ptr<Search>> seaCollec = _session.find<Search>()
                        .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                        .where("\"SRC_ID\" = ?").bind(this->vPathElements[3]);

                    for (Wt::Dbo::collection<Wt::Dbo::ptr<Search> >::const_iterator i = seaCollec.begin(); i != seaCollec.end(); i++) 
                    {
                        string path = "/" + vPathElements[1] + "/sources/" + vPathElements[3] + "/searches/" + 
                                                 boost::lexical_cast<string>(i->get()->pk.id);
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
                    res = 204;
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
    return res; 
}

void PluginResource::processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "", sRequest = "";

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
            boost::lexical_cast<unsigned long long>(nextElement);
            nextElement = getNextElementFromPath();
            if(!nextElement.compare(""))
            {
                this->statusCode = deletePlugin(responseMsg);
            }
            else if(!nextElement.compare("sources"))
            {
                nextElement = getNextElementFromPath();
                boost::lexical_cast<unsigned long long>(nextElement);

                nextElement = getNextElementFromPath();
                if(!nextElement.compare(""))
                {
                    this->statusCode = deleteSourceForPlugin(responseMsg);
                }
                else if(!nextElement.compare("searches"))
                {
                    
                    nextElement = getNextElementFromPath();
                    boost::lexical_cast<unsigned long long>(nextElement);

                    nextElement = getNextElementFromPath();

                    if(!nextElement.compare(""))
                    {
                        this->statusCode = deleteSearchForSourceAndPlugin(responseMsg);
                    }
                    else if (!nextElement.compare("inf_values"))
                    {
                        nextElement = getNextElementFromPath();
                        boost::lexical_cast<unsigned long long>(nextElement);
                        nextElement = getNextElementFromPath();
                        if (!nextElement.compare("units"))
                        {
                            nextElement = getNextElementFromPath();
                            boost::lexical_cast<unsigned long long>(nextElement);
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

void PluginResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    this->role = "";
    this->media = "";
    if (!request.getParameterValues("role").empty())
    {
        this->role = request.getParameterValues("role")[0];
    }
    
    if (!request.getParameterValues("media").empty())
    {
        this->media = request.getParameterValues("media")[0];
    }
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);
    return;
}

