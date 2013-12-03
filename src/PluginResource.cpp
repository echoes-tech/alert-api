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

PluginResource::~PluginResource()
{
}

 EReturnCode PluginResource::pluginIsAccessible(string& responseMsg)
{
     
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    Wt::Dbo::ptr<Echoes::Dbo::Plugin> plgPtr;
    try
    {

        Wt::Dbo::Transaction transaction(m_session);

        string queryStr = "SELECT plg FROM \"T_PLUGIN_PLG\" plg "
                                "WHERE \"PLG_ID\" IN "
                                "("
                                   "SELECT \"T_PLUGIN_PLG_PLG_ID\" FROM \"TJ_PLG_AST\" "
                                   "wHERE \"T_ASSET_AST_AST_ID\" IN "
                                        "("
                                        "SELECT \"AST_ID\" FROM \"T_ASSET_AST\" "
                                        "WHERE \"AST_ORG_ORG_ID\" = " + boost::lexical_cast<string>(m_session.user().get()->organization.id()) +
                                        " AND \"AST_DELETE\" IS NULL "
                                        ")"
                                    "AND \"T_PLUGIN_PLG_PLG_ID\" = " + boost::lexical_cast<string>(m_pathElements[1]) + 
                                " )"
                                " AND \"PLG_DELETE\" IS NULL";

         Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Plugin>> queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::Plugin>>(queryStr);

         plgPtr = queryRes;
         res = EReturnCode::OK;
         transaction.commit();
         if(!plgPtr)
         {
             responseMsg = "{\"message\":\" Not found\"}";
             res = EReturnCode::NOT_FOUND;
         }
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;
}

EReturnCode PluginResource::getPluginJSON(string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        // on regarde si le plugin appartient à l'organisation
        string queryStr = "SELECT plg FROM \"T_PLUGIN_PLG\" plg "
                                "WHERE \"PLG_ID\" IN "
                                "("
                                   "SELECT \"T_PLUGIN_PLG_PLG_ID\" FROM \"TJ_PLG_AST\" "
                                   "wHERE \"T_ASSET_AST_AST_ID\" IN "
                                        "("
                                        "SELECT \"AST_ID\" FROM \"T_ASSET_AST\" "
                                        "WHERE \"AST_ORG_ORG_ID\" = " + boost::lexical_cast<string>(m_session.user().get()->organization.id()) +
                                        " AND \"AST_DELETE\" IS NULL "
                                        ")"
                                    "AND \"T_PLUGIN_PLG_PLG_ID\" = " + boost::lexical_cast<string>(m_pathElements[1]) + 
                                " )" 
                                " AND \"PLG_DELETE\" IS NULL";
        
         Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Plugin> > queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::Plugin> >(queryStr);

         Wt::Dbo::ptr<Echoes::Dbo::Plugin> plgPtr = queryRes;
         
        if(plgPtr)
        {
            responseMsg = "{\n\t\"id\" : " + boost::lexical_cast<string>(m_pathElements[1]) + ",\n";
            responseMsg += "\t\"name\" : \"" + plgPtr->name.toUTF8() + "\",\n";
            responseMsg += "\t\"desc\" : \"" + plgPtr->desc.toUTF8() + "\",\n";
            responseMsg += "\t\"idAsset\" : 0,\n";
            responseMsg += "\t\"sources\" : [\n"; 

            for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Source> >::const_iterator itSrc = plgPtr->sources.begin(); itSrc != plgPtr->sources.end(); itSrc++) 
            {
                responseMsg += "\t{\n";
                responseMsg += "\t\t\"id\" : " + boost::lexical_cast<string>(itSrc->id()) +",\n";
                responseMsg += "\t\t\"idAddon\" : " + boost::lexical_cast<string>(itSrc->get()->addon.id()) +",\n";
                responseMsg += "\t\t\"params\" : {\n";
                
                Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SourceParameterValue>> srcParamCollec = m_session.find<Echoes::Dbo::SourceParameterValue>()
                        .where("\"SRC_ID\" = ?").bind(itSrc->id());

                for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SourceParameterValue> >::const_iterator itSpv = srcParamCollec.begin(); itSpv != srcParamCollec.end(); itSpv++) 
                {
                    responseMsg += "\t\t\t\"" + boost::lexical_cast<string>(itSpv->get()->pk.sourceParameter->name) + "\": \""+ boost::lexical_cast<string>(itSpv->get()->value) +"\",\n";              
                }
                if(srcParamCollec.size() != 0)
                {
                    responseMsg.replace(responseMsg.size()-2, 1, "");
                }
                responseMsg += "\t\t},\n";
                responseMsg += "\t\t\"searches\": [\n";  
      
                Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Search>> seaCollec = m_session.find<Echoes::Dbo::Search>()
                        .where("\"SRC_ID\" = ?").bind(boost::lexical_cast<string>(itSrc->id()))
                        .where("\"SEA_DELETE\" IS NULL")
                        .orderBy("\"SEA_ID\"");

                for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Search> >::const_iterator itSea = seaCollec.begin(); itSea != seaCollec.end(); itSea++) 
                {
                    responseMsg += "\t\t{\n";
                    responseMsg += "\t\t\t\"id\" : " + boost::lexical_cast<string>(itSea->id()) +",\n";
                    responseMsg += "\t\t\t\"idType\" : " + boost::lexical_cast<string>(itSea->get()->searchType.id()) +",\n";
                    responseMsg += "\t\t\t\"params\" : {\n";

                    Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SearchParameterValue>> seaParamCollec = m_session.find<Echoes::Dbo::SearchParameterValue>()
                            .where("\"SEA_ID\" = ?").bind(boost::lexical_cast<string>(itSea->id()));

                    for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SearchParameterValue> >::const_iterator itSev = seaParamCollec.begin(); itSev != seaParamCollec.end(); itSev++) 
                    {
                        responseMsg += "\t\t\t\t\"" + boost::lexical_cast<string>(itSev->get()->searchParameterValueId.searchParameter.get()->name) + "\": \""+ boost::lexical_cast<string>(itSev->get()->value) +"\",\n";           
                    }
                    if(seaParamCollec.size() != 0)
                    {
                        responseMsg.replace(responseMsg.size()-2, 1, "");
                    }   
                    responseMsg += "\t\t\t},\n";
                    responseMsg += "\t\t\t\"period\" : " + boost::lexical_cast<string>(itSea->get()->period) + ",\n";

                    responseMsg += "\t\t\t\"staticValues\" : 0\n"; 
                    responseMsg += "\t\t},\n";
                }
                if(seaCollec.size() != 0)
                {
                    responseMsg.replace(responseMsg.size()-2, 1, "");
                }
                responseMsg += "\t\t]\n\t},\n";   
            } 
            if(plgPtr->sources.size() != 0)
            {
                responseMsg.replace(responseMsg.size()-2, 1, "");
            }
            responseMsg += "\t]\n}";               
            res = EReturnCode::OK;
            transaction.commit();
        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = "{\"message\":\"Plugin not found\"}";
        }
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;  
}

EReturnCode PluginResource::getPlugin(string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    unsigned idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        // on liste les plugins rattachés à des assets détenu par l'organization
        string queryStr = "SELECT plg FROM \"T_PLUGIN_PLG\" plg "
                                "WHERE \"PLG_ID\" IN "
                                "("
                                   "SELECT \"T_PLUGIN_PLG_PLG_ID\" FROM \"TJ_PLG_AST\" "
                                   "wHERE \"T_ASSET_AST_AST_ID\" IN "
                                        "("
                                        "SELECT \"AST_ID\" FROM \"T_ASSET_AST\" "
                                        "WHERE \"AST_ORG_ORG_ID\" = " + boost::lexical_cast<string>(m_session.user().get()->organization.id()) +
                                        " AND \"AST_DELETE\" IS NULL "
                                        ")"
                                ") "
                                " AND \"PLG_DELETE\" IS NULL"
                                " ORDER BY \"PLG_ID\" ";
        
         Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Plugin> > queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::Plugin> >(queryStr);

         Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Plugin> > plgPtr = queryRes.resultList();

        
        if(plgPtr.size() > 0 )
        {
            responseMsg = "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Plugin> >::const_iterator i = plgPtr.begin(); i != plgPtr.end(); i++) 
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
            res = EReturnCode::OK;
            transaction.commit();
        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = "{\"message\":\"Plugin not found\"}";
        }
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << "[PluginResource]" << e.what();
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;  
}

// FIXME : MOVE OUT
//EReturnCode PluginResource::getAliasForInformation(string &responseMsg)
//{
//    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    if (m_role.empty())
//    {
//        res = EReturnCode::BAD_REQUEST;
//        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//        return res;
//    }
//
//    if (m_media.empty())
//    {
//        res = EReturnCode::BAD_REQUEST;
//        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//        return res;
//    }
//    
//    try 
//    {
//        Wt::Dbo::Transaction transaction(m_session);
//        Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasInformation> aliasInformation = m_session.find<Echoes::Dbo::AlertMessageAliasInformation>()
//                .where("\"AAI_DELETE\" IS NULL")
//                .where("\"URO_ID_URO_ID\" = ?").bind(m_role)
//                .where("\"MED_ID_MED_ID\" = ?").bind(m_media)
//                .where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1])
//                .where("\"SRC_ID\" = ?").bind(m_pathElements[3])
//                .where("\"SEA_ID\" = ?").bind(m_pathElements[5])
//                .where("\"INF_VALUE_NUM\" = ?").bind(m_pathElements[7])
//                .where("\"INU_ID_INU_ID\" = ?").bind(m_pathElements[9]);
//        if (aliasInformation)
//        {
//            responseMsg = aliasInformation->toJSON();
//            res = EReturnCode::OK;
//            transaction.commit();
//        }
//        else
//        {
//            res = EReturnCode::NOT_FOUND;
//            responseMsg = "{\n\t\"message\":\"Alias not found\"\n}";
//        }
//        
//        
//    } 
//    catch (Wt::Dbo::Exception const& e) 
//    {
//        Wt::log("error") << "[Plugin Ressource] " << e.what();
//        res = EReturnCode::SERVICE_UNAVAILABLE;
//        responseMsg = "{\n\t\"message\": \"Service Unavailable\"\n}";
//    }
//    
//    
//    return res;
//}
//
//EReturnCode PluginResource::getAliasForCriteria(string &responseMsg)
//{
//    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    if (m_role.empty())
//    {
//        res = EReturnCode::BAD_REQUEST;
//        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//        return res;
//    }
//
//    if (m_media.empty())
//    {
//        res = EReturnCode::BAD_REQUEST;
//        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//        return res;
//    }
//    
//    try 
//    {
//        Wt::Dbo::Transaction transaction(m_session);
//        Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasInformationCriteria> aliasCriteria = m_session.find<Echoes::Dbo::AlertMessageAliasInformationCriteria>()
//                .where("\"AIC_DELETE\" IS NULL")
//                .where("\"URO_ID_URO_ID\" = ?").bind(m_role)
//                .where("\"MED_ID_MED_ID\" = ?").bind(m_media)
//                .where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1])
//                .where("\"SRC_ID\" = ?").bind(m_pathElements[3])
//                .where("\"SEA_ID\" = ?").bind(m_pathElements[5])
//                .where("\"INF_VALUE_NUM\" = ?").bind(m_pathElements[7])
//                .where("\"INU_ID_INU_ID\" = ?").bind(m_pathElements[9])
//                .where("\"ACR_ID_ACR_ID\" = ?").bind(m_pathElements[11]);
//        if (aliasCriteria)
//        {
//            responseMsg = aliasCriteria->toJSON();
//            res = EReturnCode::OK;
//            transaction.commit();
//        }
//        else
//        {
//            res = EReturnCode::NOT_FOUND;
//            responseMsg = "{\n\t\"message\":\"Alias not found\"\n}";
//        }
//        
//        
//    } 
//    catch (Wt::Dbo::Exception const& e) 
//    {
//        Wt::log("error") << "[Plugin Ressource] " << e.what();
//        res = EReturnCode::SERVICE_UNAVAILABLE;
//        responseMsg = "{\n\t\"message\": \"Service Unavailable\"\n}";
//    }
//    
//    
//    return res;
//}
//
//EReturnCode PluginResource::getCriteriaForInformation(string &responseMsg)
//{
//    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    unsigned idx = 0;
//    try 
//    {
//        Wt::Dbo::Transaction transaction(m_session);
//        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertCriteria>> informationAlertCriteria = m_session.find<Echoes::Dbo::AlertCriteria>()
//                .where("\"ACR_DELETE\" IS NULL");
//
//        if(informationAlertCriteria.size() > 0 )
//        {
//            responseMsg = "[\n";
//            for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertCriteria> >::const_iterator i = informationAlertCriteria.begin(); i != informationAlertCriteria.end(); i++) 
//            { 
//                Echoes::Dbo::AlertCriteria acr(*i->get());
//                acr.setId(i->id());
//                responseMsg +=  acr.toJSON();
//                idx++;
//                if(informationAlertCriteria.size()-idx > 0)
//                {
//                    responseMsg += ",\n";
//                }
//            }
//            responseMsg += "\n]\n";
//            res = EReturnCode::OK;
//        }
//        else
//        {
//            res = EReturnCode::NOT_FOUND;
//            responseMsg = "{\"message\":\"Criteria not found\"}";
//        }
//        transaction.commit();
//    } 
//    catch (Wt::Dbo::Exception const& e) 
//    {
//        Wt::log("error") << "[Plugin Ressource] " << e.what();
//        res = EReturnCode::SERVICE_UNAVAILABLE;
//        responseMsg = "{\n\t\"message\": \"Service Unavailable\"\n}";
//    }
//    return res;
//}

EReturnCode PluginResource::getAliasForPlugin(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    if (m_role.empty())
    {
        res = EReturnCode::BAD_REQUEST;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        return res;
    }

    if (m_media.empty())
    {
        res = EReturnCode::BAD_REQUEST;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        return res;
    }
    
    try 
    {
        Wt::Dbo::Transaction transaction(m_session);
        Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasPlugin> aliasPlugin = m_session.find<Echoes::Dbo::AlertMessageAliasPlugin>()
                .where("\"AAP_DELETE\" IS NULL")
                .where("\"URO_ID_URO_ID\" = ?").bind(m_role)
                .where("\"MED_ID_MED_ID\" = ?").bind(m_media)
                .where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1]);
        if (aliasPlugin)
        {
            responseMsg = aliasPlugin->toJSON();
            res = EReturnCode::OK;
            transaction.commit();
        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = "{\n\t\"message\":\"Alias not found\"\n}";
        }
        
        
    } 
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << "[Plugin Ressource] " << e.what();
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = "{\n\t\"message\": \"Service Unavailable\"\n}";
    }
    
    
    return res;
}

EReturnCode PluginResource::getInformationListForPlugin(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    unsigned idx = 0;
    if((res = pluginIsAccessible(responseMsg)) == 200)
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session);

            Wt::Dbo::collection <Wt::Dbo::ptr<Echoes::Dbo::Information>> information = m_session.find<Echoes::Dbo::Information>()
                    .where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1])
                    .where("\"INF_DELETE\" IS NULL")
                    .orderBy("\"INF_ID\"");

            if(information.size() > 0 )
            {
                responseMsg = "[\n";
                for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Information> >::const_iterator i = information.begin(); i != information.end(); i++) 
                { 
                    Echoes::Dbo::Information info(*i->get());
                    responseMsg +=  info.toJSON();
                    idx++;
                    if(information.size()-idx > 0)
                    {
                        responseMsg += ",\n";
                    }
                }
                responseMsg += "\n]\n";
                res = EReturnCode::OK;
            }
            else
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = "{\"message\":\"Information not found\"}";
            }
            transaction.commit();
        }
        catch (Wt::Dbo::Exception const& e) 
        {
            Wt::log("error") << e.what();
            res = EReturnCode::SERVICE_UNAVAILABLE;
            responseMsg = "{\"message\":\"Service Unavailable\"}";
        }
    }
    return res;  
}

//FIXME : MOVE OUT
//EReturnCode PluginResource::getSearchForSourceAndPlugin(string& responseMsg)
//{
//    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    if((res = pluginIsAccessible(responseMsg)) == 200)
//    {
//        unsigned idx = 0;
//        try
//        {
//            Wt::Dbo::Transaction transaction(m_session);
//            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Search>> seaCollec = m_session.find<Echoes::Dbo::Search>()
//                    .where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1])
//                    .where("\"SRC_ID\" = ?").bind(m_pathElements[3])
//                    .where("\"SEA_DELETE\" IS NULL")
//                    .orderBy("\"SEA_ID\"");           
//            if (seaCollec.size() > 0)
//            {
//                responseMsg += "[\n";
//                for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Search> >::const_iterator i = seaCollec.begin(); i != seaCollec.end(); i++) 
//                {
//                    responseMsg += "\t" + i->get()->toJSON();
//                    ++idx;
//                    if(seaCollec.size()-idx > 0)
//                    {
//                        responseMsg += ",\n";
//                    }
//                }
//                responseMsg += "\n]\n";               
//
//                res = EReturnCode::OK;
//            }
//            else 
//            {
//                res = EReturnCode::NOT_FOUND;
//                responseMsg = "{\"message\":\"Search not found\"}";
//            }
//            transaction.commit();
//        }
//        catch (Wt::Dbo::Exception const &e)
//        {
//            Wt::log("error") << e.what();
//            res = EReturnCode::SERVICE_UNAVAILABLE;
//            responseMsg = "{\"message\":\"Service Unavailable\"}";
//        }
//    }
//    return res;
//}
//
//EReturnCode PluginResource::getSearchForSearchIdForSourceAndPlugin(string& responseMsg)
//{
//    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    if((res = pluginIsAccessible(responseMsg)) == 200)
//    {
//        try
//        {
//            Wt::Dbo::Transaction transaction(m_session);
//            Wt::Dbo::ptr<Echoes::Dbo::Search> seaCollec = m_session.find<Echoes::Dbo::Search>()
//                    .where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1])
//                    .where("\"SRC_ID\" = ?").bind(m_pathElements[3])
//                    .where("\"SEA_ID\" = ?").bind(m_pathElements[5])
//                    .where("\"SEA_DELETE\" IS NULL")
//                    .orderBy("\"SEA_ID\"");           
//            if (seaCollec)
//            {
//                responseMsg = "\t" + seaCollec.modify()->toJSON();             
//
//                res = EReturnCode::OK;
//            }
//            else 
//            {
//                res = EReturnCode::NOT_FOUND;
//                responseMsg = "{\"message\":\"Search not found\"}";
//            }
//            transaction.commit();
//        }
//        catch (Wt::Dbo::Exception const &e)
//        {
//            Wt::log("error") << e.what();
//            res = EReturnCode::SERVICE_UNAVAILABLE;
//            responseMsg = "{\"message\":\"Service Unavailable\"}";
//        }
//    }
//    return res;
//}
//
//
//EReturnCode PluginResource::getParameterValueForSearch(string &responseMsg)
//{
//    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    if((res = pluginIsAccessible(responseMsg)) == 200)
//    {
//        unsigned idx = 0;
//        try
//        {
//            Wt::Dbo::Transaction transaction(m_session);
//            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SearchParameterValue>> seaParamCollec = m_session.find<Echoes::Dbo::SearchParameterValue>()
//                    .where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1])
//                    .where("\"SRC_ID\" = ?").bind(m_pathElements[3])
//                    .where("\"SEA_ID\" = ?").bind(m_pathElements[5]);
//            if (seaParamCollec.size() > 0)
//            {
//                responseMsg += "[\n";
//                for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SearchParameterValue> >::const_iterator i = seaParamCollec.begin(); i != seaParamCollec.end(); i++) 
//                {
//                    responseMsg += "\t" + i->get()->toJSON();
//                    ++idx;
//                    if(seaParamCollec.size()-idx > 0)
//                    {
//                        responseMsg += ",\n";
//                    }
//                }
//                responseMsg += "]\n";               
//
//                res = EReturnCode::OK;
//            }
//            else 
//            {
//                res = EReturnCode::NOT_FOUND;
//                responseMsg = "{\"message\":\"Parameter not found\"}";
//            }
//            transaction.commit();
//        }
//        catch (Wt::Dbo::Exception const &e)
//        {
//            Wt::log("error") << e.what();
//            res = EReturnCode::SERVICE_UNAVAILABLE;
//            responseMsg = "{\"message\":\"Service Unavailable\"}";
//        }
//    }
//    return res;
//}
//
//EReturnCode PluginResource::getInformationForSeaSrcAndPlg(string& responseMsg)
//{
//    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    if((res = pluginIsAccessible(responseMsg)) == 200)
//    {
//        unsigned idx = 0;
//        try
//        {
//            Wt::Dbo::Transaction transaction(m_session);
//            Wt::Dbo::collection < Wt::Dbo::ptr < Echoes::Dbo::Information >> infCollec = m_session.find<Echoes::Dbo::Information>()
//                    .where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1])
//                    .where("\"SRC_ID\" = ?").bind(m_pathElements[3])
//                    .where("\"SEA_ID\" = ?").bind(m_pathElements[5])
//                    .where("\"INF_DELETE\" IS NULL")
//                    .orderBy("\"INF_VALUE_NUM\"");                                                                                       
//            if (infCollec.size() > 0)
//            {
//                responseMsg += "[\n";
//                for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Information> >::const_iterator i = infCollec.begin(); i != infCollec.end(); i++) 
//                {
//                    Echoes::Dbo::Information info(*i->get());
//                    responseMsg += "\t" + info.toJSON();
//                    ++idx;
//                    if(infCollec.size()-idx > 0)
//                    {
//                        responseMsg += ",\n";
//                    }
//                }
//                responseMsg += "\n]\n";               
//
//                res = EReturnCode::OK;
//            }
//            else 
//            {
//                res = EReturnCode::NOT_FOUND;
//                responseMsg = "{\"message\":\"Information not found\"}";
//            }
//            transaction.commit();
//        }
//        catch (Wt::Dbo::Exception const &e)
//        {
//            Wt::log("error") << e.what();
//            res = EReturnCode::SERVICE_UNAVAILABLE;
//            responseMsg = "{\"message\":\"Service Unavailable\"}";
//        }
//    }
//    return res;
//}

EReturnCode PluginResource::getSourceForPlugin(string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    if((res = pluginIsAccessible(responseMsg)) == 200)
    {
        unsigned idx = 0;
        try
        {
            Wt::Dbo::Transaction transaction(m_session);
            Wt::Dbo::ptr<Echoes::Dbo::Plugin> plgPtr = m_session.find<Echoes::Dbo::Plugin>()
                    .where("\"PLG_ID\" = ?").bind(m_pathElements[1])
                    .where("\"PLG_DELETE\" IS NULL")
                    .limit(1);
            
            if (plgPtr->sources.size() > 0)
            {
                responseMsg += "[\n";
                for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Source> >::const_iterator i = plgPtr->sources.begin(); i != plgPtr->sources.end(); i++) 
                {
                    responseMsg += "\t" + i->get()->toJSON();
                    ++idx;
                    if(plgPtr->sources.size()-idx > 0)
                    {
                        responseMsg += ",\n";
                    }
                }
                responseMsg += "\n]\n";               

                res = EReturnCode::OK;
            }
            else 
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = "{\"message\":\"Source not found\"}";
            }
            transaction.commit();
        }
        catch (Wt::Dbo::Exception const &e)
        {
            Wt::log("error") << e.what();
            res = EReturnCode::SERVICE_UNAVAILABLE;
            responseMsg = "{\"message\":\"Service Unavailable\"}";
        }
    }
    return res;
}

//FIXME: MOVE OUT
//EReturnCode PluginResource::getParameterValueForSource(string& responseMsg)
//{
//    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    if((res = pluginIsAccessible(responseMsg)) == 200)
//    {
//        unsigned idx = 0;
//        try
//        {
//            Wt::Dbo::Transaction transaction(m_session);
//            Wt::Dbo::collection < Wt::Dbo::ptr < Echoes::Dbo::SourceParameterValue >> srcParamCollec = m_session.find<Echoes::Dbo::SourceParameterValue>()
//                    .where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1])
//                    .where("\"SRC_ID\" = ?").bind(m_pathElements[3]);
//
//            if (srcParamCollec.size() > 0)
//            {
//                responseMsg += "[\n";
//                for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SourceParameterValue> >::const_iterator i = srcParamCollec.begin(); i != srcParamCollec.end(); i++) 
//                {
//                    responseMsg += "\t" + i->get()->toJSON();
//                    ++idx;
//                    if(srcParamCollec.size()-idx > 0)
//                    {
//                        responseMsg += ",\n";
//                    }
//                }
//                responseMsg += "\n]\n";               
//
//                res = EReturnCode::OK;
//            }
//            else 
//            {
//                res = EReturnCode::NOT_FOUND;
//                responseMsg = "{\"message\":\"Parameter not found\"}";
//            }
//            transaction.commit();
//        }
//        catch (Wt::Dbo::Exception const &e)
//        {
//            Wt::log("error") << e.what();
//            res = EReturnCode::SERVICE_UNAVAILABLE;
//            responseMsg = "{\"message\":\"Service Unavailable\"}";
//        }
//    }
//    return res;
//}

void PluginResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";
    
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        m_statusCode = getPlugin(responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();

            if(!nextElement.compare(""))
            {
                m_statusCode = getPluginJSON(responseMsg);
            }
            else if(!nextElement.compare("alias"))
            {
                m_statusCode = getAliasForPlugin(responseMsg);
            }
            else if(!nextElement.compare("informations"))
            {
                nextElement = getNextElementFromPath();
                if(!nextElement.compare(""))
                {
                    m_statusCode = getInformationListForPlugin(responseMsg);
                }
                else
                {
                    m_statusCode = EReturnCode::BAD_REQUEST;
                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                }
            }
            else if (!nextElement.compare("sources"))
            {
                nextElement = getNextElementFromPath();
                if(!nextElement.compare(""))
                {
                    m_statusCode = getSourceForPlugin(responseMsg);
                }
                else
                {
//                    boost::lexical_cast<unsigned long long>(nextElement);
//                    nextElement = getNextElementFromPath();
//                    if (!nextElement.compare("parameters"))
//                    {
//                        m_statusCode = getParameterValueForSource(responseMsg);
//                    }
//                    else if (!nextElement.compare("searches"))
//                    {
//                        nextElement = getNextElementFromPath();
//                        if(!nextElement.compare(""))
//                        {
//                            m_statusCode = getSearchForSourceAndPlugin(responseMsg);
//                        }
//                        else
//                        {
//                            boost::lexical_cast<unsigned long long>(nextElement);
//                            nextElement = getNextElementFromPath();
//                            if(!nextElement.compare(""))
//                            {
//                                m_statusCode = getSearchForSearchIdForSourceAndPlugin(responseMsg);
//                            }
//                            else if(!nextElement.compare("parameters"))
//                            {
//                                m_statusCode = getParameterValueForSearch(responseMsg);
//                            }
//                            else if(!nextElement.compare("informations"))
//                            {
//                                nextElement = getNextElementFromPath();
//                                if(!nextElement.compare(""))
//                                {
//                                    m_statusCode = getInformationForSeaSrcAndPlg(responseMsg);
//                                }
//                                else
//                                {
//                                    m_statusCode = EReturnCode::BAD_REQUEST;
//                                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                                }
//                                
//                            }
//                            else if(!nextElement.compare("inf_values"))
//                            {
//                                nextElement = getNextElementFromPath();
//                                boost::lexical_cast<unsigned long long>(nextElement);
//                                nextElement = getNextElementFromPath();
//                                if(!nextElement.compare("units"))
//                                {
//                                    nextElement = getNextElementFromPath();
//                                    boost::lexical_cast<unsigned long long>(nextElement);
//                                    nextElement = getNextElementFromPath();
//                                    if(!nextElement.compare("alias"))
//                                    {
//                                        m_statusCode = getAliasForInformation(responseMsg);
//                                    }
//                                    else if (!nextElement.compare("criteria"))
//                                    {
//                                        nextElement = getNextElementFromPath();
//                                        if(!nextElement.compare(""))
//                                        {
//                                            m_statusCode = getCriteriaForInformation(responseMsg);
//                                        }
//                                        else
//                                        {
//                                            boost::lexical_cast<unsigned long long>(nextElement);
//                                            nextElement = getNextElementFromPath();
//                                            if(!nextElement.compare("alias"))
//                                            {
//                                                m_statusCode = getAliasForCriteria(responseMsg);
//                                            }
//                                            else
//                                            {
//                                                m_statusCode = EReturnCode::BAD_REQUEST;
//                                                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                                            }
//                                        }
//                                    }
//                                    else
//                                    {
//                                        m_statusCode = EReturnCode::BAD_REQUEST;
//                                        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                                    }
//                                }
//                                else
//                                {
//                                    m_statusCode = EReturnCode::BAD_REQUEST;
//                                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                                }
//                            }   
//                            else
//                            {
//                                m_statusCode = EReturnCode::BAD_REQUEST;
//                                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                            }
//                        }
//                    }
//                    else
//                    {
//                        m_statusCode = EReturnCode::BAD_REQUEST;
//                        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                    }
                    m_statusCode = EReturnCode::BAD_REQUEST;
                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                }
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            m_statusCode = EReturnCode::BAD_REQUEST;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}


EReturnCode PluginResource::postPlugin(string& responseMsg, const string& sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
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
        res = EReturnCode::BAD_REQUEST;
        responseMsg = "{\"message\":\"Problems parsing JSON\"}";
        Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON:" << sRequest;
        return res;
    }
    catch (Wt::Json::TypeException const& e)
    {
        res = EReturnCode::BAD_REQUEST;
        responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
        Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON.:" << sRequest;
        return res;
    }   
    try 
    {
        Wt::Dbo::Transaction transaction(m_session);
        Echoes::Dbo::Plugin *plugin = new Echoes::Dbo::Plugin();
        plugin->name = plgName;
        plugin->desc = plgDesc;
        
        Wt::Dbo::ptr<Echoes::Dbo::Plugin> plgPtr = m_session.add<Echoes::Dbo::Plugin>(plugin);
       
        plgPtr.flush();
        plgPtr.modify()->setId(plgPtr.id());
        responseMsg = plgPtr->toJSON();
        
        transaction.commit();
        res = EReturnCode::OK;
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;
}

EReturnCode PluginResource::postSourceForPlugin(string& responseMsg, const string& sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
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
                Wt::Dbo::Transaction transaction(m_session);

                // creer l'id de la source
                Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Source>> srcCollec = m_session.find<Echoes::Dbo::Source>().where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1]);
//                long long srcId = 1;
                if(srcCollec.size()> 0)
                {
                    string queryStr = "SELECT MAX(\"SRC_ID\") FROM \"T_SOURCE_SRC\" src "
                                       " WHERE \"PLG_ID_PLG_ID\" = " + boost::lexical_cast<string > (m_pathElements[1]);

                    Wt::Dbo::Query<long long> queryResult = m_session.query<long long>(queryStr);
//                    srcId = queryResult + 1;
                }

                //creation de la source
                Wt::Dbo::ptr<Echoes::Dbo::Plugin> plgPtr = m_session.find<Echoes::Dbo::Plugin>().where("\"PLG_ID\" = ?").bind(m_pathElements[1]);
                Wt::Dbo::ptr<Echoes::Dbo::Addon> addonPtr = m_session.find<Echoes::Dbo::Addon>().where("\"ADO_ID\" = ?").bind(addonId);
                Echoes::Dbo::Source *source = new Echoes::Dbo::Source;
                //FIXME
//                source->pk.id = srcId;
//                source->pk.plugin = plgPtr;
                source->addon = addonPtr;
                Wt::Dbo::ptr<Echoes::Dbo::Source> srcPtr = m_session.add<Echoes::Dbo::Source>(source);
                srcPtr.flush();


                Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SourceParameter> > sourceParameterPtr = addonPtr.get()->sourceParameters;

                //creations des parametres
                for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SourceParameter> >::const_iterator i = sourceParameterPtr.begin(); i != sourceParameterPtr.end(); i++)
                {
                    Echoes::Dbo::SourceParameterValue *sourceParameterValue = new Echoes::Dbo::SourceParameterValue;

                    //recuperation des "source_parameters" dans le JSON
                    Wt::WString tmp = result.get(boost::lexical_cast<string>(i->get()->name));

                    sourceParameterValue->value = tmp;
                    sourceParameterValue->name = i->get()->name;
                    sourceParameterValue->pk.sourceParameter = *i;
                    sourceParameterValue->pk.source = srcPtr;
                    m_session.add<Echoes::Dbo::SourceParameterValue>(sourceParameterValue);
                }
                res = EReturnCode::OK;
                responseMsg = srcPtr->toJSON();
                transaction.commit();
            }
            catch (Wt::Dbo::Exception const& e) 
            {
                Wt::log("error") << e.what();
                res = EReturnCode::SERVICE_UNAVAILABLE;
                responseMsg = "{\"message\":\"Service Unavailable\"}";
            }
        }
        catch (Wt::Json::ParseError const& e)
        {
            res = EReturnCode::BAD_REQUEST;
            responseMsg = "{\"message\":\"Problems parsing JSON\"}";
            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON:" << sRequest;
        }
        catch (Wt::Json::TypeException const& e)
        {
            res = EReturnCode::BAD_REQUEST;
            responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON.:" << sRequest;
        }   
    }
    return res;
}

//EReturnCode PluginResource::postSearchForSourceAndPlugin(string& responseMsg, const string& sRequest)
//{
//    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    if((res = pluginIsAccessible(responseMsg)) == 200)
//    {
//        long long styId;
//        bool seaIsStatic;
//        int posKeyValue, nbValue, seaPeriod;
//        Wt::Json::Array& units = Wt::Json::Array::Empty;
//
//        try
//        {
//            Wt::Json::Object result;                   
//            Wt::Json::parse(sRequest, result);
//
//            //search
//            seaPeriod = result.get("sea_period");
//            styId = result.get("sty_id");
//            seaIsStatic = result.get("sea_is_static");
//            posKeyValue = result.get("pos_key_value");
//            nbValue = result.get("nb_value");
//            units = result.get("units");
//            if(units.size() <= (unsigned)nbValue)
//            {
//                try
//                {
//                    Wt::Dbo::Transaction transaction(m_session);
//                    Wt::Dbo::ptr<Echoes::Dbo::Search> seaPtr;
//                    //verif si le search type va avec l'addon 
//
//                    string queryStr = "SELECT set FROM \"T_SEARCH_TYPE_STY\" set "
//                                           " WHERE \"STY_ID\" IN"
//                                            "("
//                                                "SELECT \"T_SEARCH_TYPE_STY_STY_ID\" FROM \"TJ_ADO_STY\" "
//                                                "WHERE \"T_ADDON_ADO_ADO_ID\" = " 
//                                                "("
//                                                    "SELECT \"SRC_ADO_ADO_ID\" FROM \"T_SOURCE_SRC\" "
//                                                    "WHERE \"PLG_ID_PLG_ID\" = " + m_pathElements[1] +
//                                                    "AND \"SRC_ID\" =  " + m_pathElements[3] + 
//                                                ")"
//                                            ")"
//                                            "AND \"STY_ID\" = " + boost::lexical_cast<string>(styId);
//
//                    Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::SearchType> > queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::SearchType> >(queryStr);
//
//                    Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SearchType> > seaTypePtr = queryRes.resultList();
//
//                    if(seaTypePtr.size() == 0)
//                    {
//                            res = EReturnCode::NOT_FOUND;
//                            responseMsg = "{\"message\":\"Search Type not found.\"}";
//                            return res;
//                    }
//                    // creer l'id de la search
//                    Wt::Dbo::collection <Wt::Dbo::ptr<Echoes::Dbo::Search>> seaCollec = m_session.find<Echoes::Dbo::Search>()
//                            .where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1])
//                            .where("\"SRC_ID\" = ?").bind(m_pathElements[3]);
//                    long long seaId = 1;
//                    if (seaCollec.size() > 0)
//                    {
//                        string queryStr = "SELECT MAX(\"SEA_ID\") FROM \"T_SEARCH_SEA\" sea "
//                                               " WHERE \"PLG_ID_PLG_ID\" = " + m_pathElements[1] +
//                                               " AND \"SRC_ID\" = " + m_pathElements[3];
//
//                        Wt::Dbo::Query<long long> queryResult = m_session.query<long long>(queryStr);
//                        seaId = queryResult + 1;    
//                    }
//
//                    //creation de la search
//                    Wt::Dbo::ptr<Echoes::Dbo::Source> srcPtr = m_session.find<Echoes::Dbo::Source>()
//                            .where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1])
//                            .where("\"SRC_ID\" = ?").bind(m_pathElements[3]);
//                    if(srcPtr)
//                    {
//                        Wt::Dbo::ptr<Echoes::Dbo::SearchType> seaTypPtr = m_session.find<Echoes::Dbo::SearchType>().where("\"STY_ID\" = ?").bind(styId);
//                        if(seaTypPtr)
//                        {
//                            Echoes::Dbo::Search *search = new Echoes::Dbo::Search;
//                            //FIXME
////                            search->pk.id = seaId;
////                            search->pk.source = srcPtr;
////                            search->searchType = seaTypPtr;
////                            search->period = seaPeriod;
////                            search->nbValue = nbValue;
////                            search->pos_key_value = posKeyValue;
////                            search->searchIsStatic = seaIsStatic;
//                            seaPtr = m_session.add<Echoes::Dbo::Search>(search);
//                            seaPtr.flush();
//
//
//                            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SearchParameter> > searchParameterPtr = seaTypPtr->searchParameters;
//                            //creations des parametres
//                            for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SearchParameter>>::const_iterator i = searchParameterPtr.begin(); i != searchParameterPtr.end(); i++)
//                            {
//                                Echoes::Dbo::SearchParameterValue *searchParameterValue = new Echoes::Dbo::SearchParameterValue;
//
//                                //recuperation des "search_parameters" dans le JSON
//                                Wt::WString tmp = result.get(i->get()->name.toUTF8());
//
//                                searchParameterValue->value = tmp;
//                                searchParameterValue->name = i->get()->name;
//                                searchParameterValue->searchParameterValueId.searchParameter = *i;
//                                searchParameterValue->searchParameterValueId.search = seaPtr;
//                                m_session.add<Echoes::Dbo::SearchParameterValue>(searchParameterValue);
//                            }
//
//                            //liée aux valeurs recherchées une unité
//                            for (Wt::Json::Array::const_iterator idx1 = units.begin() ; idx1 < units.end(); idx1++)
//                            {
//                                Wt::Json::Object tmp = *idx1;
//                                int valNumUnit = tmp.get("val_num");
//                                int unitId = tmp.get("unit_id");
//                                Wt::Dbo::ptr<Echoes::Dbo::InformationUnit> informationUnitPtr = m_session.find<Echoes::Dbo::InformationUnit>().where("\"INU_ID\" = ?").bind(unitId);
//                                if (informationUnitPtr && valNumUnit <= nbValue)
//                                {
//                                    Echoes::Dbo::SearchUnit *searchUnit = new Echoes::Dbo::SearchUnit;
//                                    searchUnit->pk.infValueNum = valNumUnit;
//                                    searchUnit->pk.search = seaPtr;
//                                    searchUnit->informationUnit = informationUnitPtr;
//                                    m_session.add<Echoes::Dbo::SearchUnit>(searchUnit);
//                                }
//                                else
//                                {
//                                    Wt::log("info") << "[Plugin Ressource] info non trouvée ou valNum non autorisée";
//                                    res = EReturnCode::BAD_REQUEST;
//                                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}"; 
//                                    return res;
//                                }
//                            }
//                            responseMsg = seaPtr->toJSON();
//                            res = EReturnCode::OK;
//                        }
//                        else
//                        {
//                            res = EReturnCode::NOT_FOUND;
//                            responseMsg = "{\"message\":\"Search Type not found\"}";
//                        }
//                    }
//                    else
//                    {
//                        res = EReturnCode::NOT_FOUND;
//                        responseMsg = "{\"message\":\"Source not found\"}";
//                    }
//                    transaction.commit();
//                }
//                catch (Wt::Dbo::Exception const& e) 
//                {
//                    Wt::log("error") << e.what();
//                    res = EReturnCode::SERVICE_UNAVAILABLE;
//                    responseMsg = "{\"message\":\"Service Unavailable\"}";
//                }
//            }
//            else
//            {
//                 Wt::log("info") << "[Plugin Ressource] unité sur val_num non declarée";
//                 m_statusCode = EReturnCode::BAD_REQUEST;
//                 responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";  
//            }
//        }
//        catch (Wt::Json::ParseError const& e)
//        {
//            res = EReturnCode::BAD_REQUEST;
//            responseMsg = "{\"message\":\"Problems parsing JSON\"}";
//            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON:" << sRequest;
//        }
//        catch (Wt::Json::TypeException const& e)
//        {
//            res = EReturnCode::BAD_REQUEST;
//            responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
//            Wt::log("warning") << "[PluginRessource] Problems parsing JSON.:" << sRequest;
//        }   
//    }
//    return res;
//}
//
//EReturnCode PluginResource::postInformationForSeaSrcAndPlg(string& responseMsg, const string& sRequest)
//{
//    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    if((res = pluginIsAccessible(responseMsg)) == 200)
//    {
//        Wt::WString infName, infCalculate;
//        bool infDisplay;
//        int valueNum;
//
//        try
//        {
//            Wt::Json::Object result;                   
//            Wt::Json::parse(sRequest, result);
//
//            //information
//            infName = result.get("inf_name");
//            infDisplay = result.get("inf_display");
//            valueNum = result.get("inf_value_num");
//
//            try
//            {
//                Wt::Dbo::Transaction transaction(m_session);
//
//                //search exist ?
//                Wt::Dbo::ptr<Echoes::Dbo::Search> seaPtr = m_session.find<Echoes::Dbo::Search>()
//                        .where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1])
//                        .where("\"SRC_ID\" = ?").bind(m_pathElements[3])
//                        .where("\"SEA_ID\" = ?").bind(m_pathElements[5]);
//
//                if(seaPtr)
//                {
//                    //Relier une unité à l'info
//                    // unit exist?
//                    Wt::Dbo::ptr<Echoes::Dbo::SearchUnit> seaUnitPtr = m_session.find<Echoes::Dbo::SearchUnit>()
//                            .where("\"INF_VALUE_NUM\" = ?").bind(valueNum)
//                            .where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1])
//                            .where("\"SRC_ID\" = ?").bind(m_pathElements[3])
//                            .where("\"SEA_ID\" = ?").bind(m_pathElements[5]);
//                    if(seaUnitPtr)
//                    {
//                        //creation info
//                        Echoes::Dbo::Information *information = new Echoes::Dbo::Information;
//                        //FIXME
////                        information->pk.search = seaPtr;
////                        information->pk.unit = seaUnitPtr.get()->informationUnit;
////                        information->pk.subSearchNumber = valueNum;
////                        information->name = infName;
////                        information->display = infDisplay;
//
//                        if(result.contains("inf_calculate"))
//                        {
//                            infCalculate = result.get("inf_calculate");
//                            information->calculate = infCalculate;
//                        }
//                        Wt::Dbo::ptr<Echoes::Dbo::Information> infPtr = m_session.add<Echoes::Dbo::Information>(information);
//                        infPtr.flush();
//                        responseMsg = infPtr->toJSON();
//                        res = EReturnCode::OK;
//                    }
//                    else
//                    {
//                        res = EReturnCode::NOT_FOUND;
//                        responseMsg = "{\"message\":\"Information not found\"}";
//                    }
//                }
//                else
//                {
//                    res = EReturnCode::NOT_FOUND;
//                    responseMsg = "{\"message\":\"Search not found\"}";
//                }
//                transaction.commit();
//            }
//            catch (Wt::Dbo::Exception const& e) 
//            {
//                Wt::log("error") << e.what();
//                res = EReturnCode::SERVICE_UNAVAILABLE;
//                responseMsg = "{\"message\":\"Service Unavailable\"}";
//            }
//        }
//        catch (Wt::Json::ParseError const& e)
//        {
//            res = EReturnCode::BAD_REQUEST;
//            responseMsg = "{\"message\":\"Problems parsing JSON\"}";
//            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON:" << sRequest;
//        }
//        catch (Wt::Json::TypeException const& e)
//        {
//            res = EReturnCode::BAD_REQUEST;
//            responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
//            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON.:" << sRequest;
//        }   
//    }
//    return res; 
//}

void PluginResource::processPostRequest(Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";

    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        m_statusCode = postPlugin(responseMsg, m_requestData);
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
                    m_statusCode = postSourceForPlugin(responseMsg, m_requestData);
                }
                else
                {
//                    boost::lexical_cast<unsigned long long>(nextElement);
//
//                    nextElement = getNextElementFromPath();
//                    
//                    ///// patch
//                    if(!nextElement.compare("parameters"))
//                    {
//                        m_statusCode = patchParametersSourceForPlugin(responseMsg, sRequest);
//                    }
//                    
//                    ////patch
//                    else if(!nextElement.compare("searches"))
//                    {
//                        nextElement = getNextElementFromPath();
//                        if (!nextElement.compare(""))
//                        {
//                            m_statusCode = postSearchForSourceAndPlugin(responseMsg, sRequest);
//                        }
//                        else
//                        {
//                            boost::lexical_cast<unsigned long long>(nextElement);
//                            nextElement = getNextElementFromPath();
//                            ///patch
//                            if(!nextElement.compare(""))
//                            {
//                                m_statusCode = patchSearchForSourceAndPlugin(responseMsg, sRequest);
//                            }
//                            
//                            ///patch
//                            else if(!nextElement.compare("informations"))
//                            {
//                                nextElement = getNextElementFromPath();
//                                if (!nextElement.compare(""))
//                                {
//                                    m_statusCode = postInformationForSeaSrcAndPlg(responseMsg, sRequest);
//                                }
//                                else
//                                {
//                                    ///patch
//                                    boost::lexical_cast<unsigned long long>(nextElement);
//                                    nextElement = getNextElementFromPath();
//                                    
//                                    if(!nextElement.compare(""))
//                                    {
//                                        m_statusCode = patchSearchForSourceAndPlugin(responseMsg, sRequest);
//                                    }
//                                    else
//                                    {
//                                        m_statusCode = EReturnCode::BAD_REQUEST;
//                                        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                                    }
//                                    ///patch
//                                }
//                            }
//                            else
//                            {
//                                m_statusCode = EReturnCode::BAD_REQUEST;
//                                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                            }
//                        }
//                    }
//                    else
//                    {
//                        m_statusCode = EReturnCode::BAD_REQUEST;
//                        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                    }
                    m_statusCode = EReturnCode::BAD_REQUEST;
                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                }
            }
            else
            {  
                m_statusCode = EReturnCode::BAD_REQUEST;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            m_statusCode = EReturnCode::BAD_REQUEST;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }             
    }

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return ;
}


void PluginResource::processPutRequest(Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";
    
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();

//            if (!nextElement.compare("sources"))
//            {
//                nextElement = getNextElementFromPath();
//                boost::lexical_cast<unsigned long long>(nextElement);
//                nextElement = getNextElementFromPath();
//                if (!nextElement.compare("searches"))
//                {
//                    nextElement = getNextElementFromPath();
//                    boost::lexical_cast<unsigned long long>(nextElement);
//                    nextElement = getNextElementFromPath();
//                    if(!nextElement.compare("inf_values"))
//                    {
//                        nextElement = getNextElementFromPath();
//                        boost::lexical_cast<unsigned long long>(nextElement);
//                        nextElement = getNextElementFromPath();
//                        if(!nextElement.compare("units"))
//                        {
//                            nextElement = getNextElementFromPath();
//                            boost::lexical_cast<unsigned long long>(nextElement);
//                            nextElement = getNextElementFromPath();
//                            if(!nextElement.compare("criteria"))
//                            {
//                                nextElement = getNextElementFromPath();
//                                boost::lexical_cast<unsigned long long>(nextElement);
//                                nextElement = getNextElementFromPath();
//                                if(!nextElement.compare("alias"))
//                                {
//                                    m_statusCode = putAliasForCriterion(responseMsg,sRequest);
//                                }
//                                else
//                                {
//                                    m_statusCode = EReturnCode::BAD_REQUEST;
//                                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                                }
//                            }
//                            else if(!nextElement.compare("alias"))
//                            {
//                                m_statusCode = putAliasForInformation(responseMsg,sRequest);
//                            }
//                            else
//                            {
//                                m_statusCode = EReturnCode::BAD_REQUEST;
//                                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                            }
//                        }
//                        else
//                        {
//                            m_statusCode = EReturnCode::BAD_REQUEST;
//                            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                        }
//                    }
//                    else
//                    {
//                        m_statusCode = EReturnCode::BAD_REQUEST;
//                        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                    }
//                }
//                else
//                {
//                    m_statusCode = EReturnCode::BAD_REQUEST;
//                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                }
//
//            }
//            else 
            if (!nextElement.compare("alias"))
            {
                m_statusCode = putAliasForPlugin(responseMsg,m_requestData);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            m_statusCode = EReturnCode::BAD_REQUEST;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}

//EReturnCode PluginResource::putAliasForInformation(string &responseMsg, const string &sRequest)
//{
//    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    Wt::WString sRole;
//    Wt::WString sMedia;
//    Wt::WString sValue;
//    if((res = pluginIsAccessible(responseMsg)) == 200)
//    {
//        try
//        {
//            Wt::Json::Object result;                   
//            Wt::Json::parse(sRequest, result);
//
//            //information
//            Wt::Json::Object alias = result.get("alias");
//            sRole = alias.get("role");
//            sMedia = alias.get("media");
//            sValue = alias.get("value");
//
//            try
//            {
//                Wt::Dbo::Transaction transaction(m_session);
//                //FIXME
//                // Information exist?
////                Wt::Dbo::ptr<Echoes::Dbo::Information> infPtr = m_session.find<Echoes::Dbo::Information>()
////                        .where("\"PLG_ID_PLG_ID\" = ?").bind(vPathElements[1])
////                        .where("\"SRC_ID\" = ?").bind(vPathElements[3])
////                        .where("\"SEA_ID\" = ?").bind(vPathElements[5])
////                        .where("\"INF_VALUE_NUM\" = ?").bind(vPathElements[7])
////                        .where("\"INU_ID_INU_ID\" = ?").bind(vPathElements[9])
////                        ;
////                if(infPtr)
////                {
////                    //Relier une unité à l'info
////                    // unit exist?
////                    Wt::Dbo::ptr<Echoes::Dbo::UserRole> ptrRole = _session.find<Echoes::Dbo::UserRole>()
////                        .where("\"URO_ID\" = ?").bind(sRole)
////                        .where("\"URO_DELETE\" IS NULL");
////                
////                    if (!ptrRole)
////                    {
////                        res = EReturnCode::NOT_FOUND;
////                        responseMsg = "{\n\t\"message\":\"Role not found\"\n}";
////                        return res;
////                    }
////
////                    Wt::Dbo::ptr<Echoes::Dbo::MediaType> ptrMedia = _session.find<Echoes::Dbo::MediaType>()
////                            .where("\"MED_ID\" = ?").bind(sMedia)
////                            .where("\"MED_DELETE\" IS NULL");
////
////                    if (!ptrMedia)
////                    {
////                        res = EReturnCode::NOT_FOUND;
////                        responseMsg = "{\n\t\"message\":\"Media not found\"\n}";
////                        return res;
////                    }
////
////
////                    Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasInformation> ptrInformationAlias = _session.find<AlertMessageAliasInformation>()
////                            .where("\"URO_ID_URO_ID\" = ?").bind(sRole)
////                            .where("\"SEA_ID\" = ?").bind(infPtr->pk.search->pk.id)
////                            .where("\"SRC_ID\" = ?").bind(infPtr->pk.search->pk.source->pk.id)
////                            .where("\"PLG_ID_PLG_ID\" = ?").bind(infPtr->pk.search->pk.source->pk.plugin.id())
////                            .where("\"INF_VALUE_NUM\" = ?").bind(infPtr->pk.subSearchNumber)
////                            .where("\"INU_ID_INU_ID\" = ?").bind(infPtr->pk.unit.id())
////                            .where("\"MED_ID_MED_ID\" = ?").bind(sMedia);
////                    if (ptrInformationAlias) 
////                    {
////                        ptrInformationAlias.modify()->alias = sValue;
////                    }
////                    else
////                    {
////                        Echoes::Dbo::AlertMessageAliasInformation *newInformationAlias = new Echoes::Dbo::AlertMessageAliasInformation();
////                        newInformationAlias->pk.information = infPtr;
////                        newInformationAlias->pk.userRole = ptrRole;
////                        newInformationAlias->pk.media = ptrMedia;
////                        newInformationAlias->alias = sValue;
////                        ptrInformationAlias = _session.add<Echoes::Dbo::AlertMessageAliasInformation>(newInformationAlias);
////                    }
////                    res = EReturnCode::OK;
////                }
////                else
////                {
////                    res = EReturnCode::NOT_FOUND;
////                    responseMsg = "{\"message\":\"Information not found\"}";
////                }
//
//                transaction.commit();
//            }
//            catch (Wt::Dbo::Exception const& e) 
//            {
//                Wt::log("error") << e.what();
//                res = EReturnCode::SERVICE_UNAVAILABLE;
//                responseMsg = "{\"message\":\"Service Unavailable\"}";
//            }
//        }
//        catch (Wt::Json::ParseError const& e)
//        {
//            res = EReturnCode::BAD_REQUEST;
//            responseMsg = "{\"message\":\"Problems parsing JSON\"}";
//            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON:" << sRequest;
//        }
//        catch (Wt::Json::TypeException const& e)
//        {
//            res = EReturnCode::BAD_REQUEST;
//            responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
//            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON.:" << sRequest;
//        }   
//    }
//    return res; 
//}
//
//EReturnCode PluginResource::putAliasForCriterion(string &responseMsg, const string &sRequest)
//{
//    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    Wt::WString sRole;
//    Wt::WString sMedia;
//    Wt::WString sValue;
//    if((res = pluginIsAccessible(responseMsg)) == 200)
//    {
//        try
//        {
//            Wt::Json::Object result;                   
//            Wt::Json::parse(sRequest, result);
//
//            //information
//            Wt::Json::Object alias = result.get("alias");
//            sRole = alias.get("role");
//            sMedia = alias.get("media");
//            sValue = alias.get("value");
//
//            try
//            {
//                Wt::Dbo::Transaction transaction(m_session);
//
//                // Information exist?
//                Wt::Dbo::ptr<Echoes::Dbo::Information> infPtr = m_session.find<Echoes::Dbo::Information>()
//                        .where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1])
//                        .where("\"SRC_ID\" = ?").bind(m_pathElements[3])
//                        .where("\"SEA_ID\" = ?").bind(m_pathElements[5])
//                        .where("\"INF_VALUE_NUM\" = ?").bind(m_pathElements[7])
//                        .where("\"INU_ID_INU_ID\" = ?").bind(m_pathElements[9])
//                        ;
//                if(infPtr)
//                {
//                    //Relier une unité à l'info
//                    // unit exist?
//                    Wt::Dbo::ptr<Echoes::Dbo::UserRole> ptrRole = m_session.find<Echoes::Dbo::UserRole>()
//                        .where("\"URO_ID\" = ?").bind(sRole)
//                        .where("\"URO_DELETE\" IS NULL");
//                
//                    if (!ptrRole)
//                    {
//                        res = EReturnCode::NOT_FOUND;
//                        responseMsg = "{\n\t\"message\":\"Role not found\"\n}";
//                        return res;
//                    }
//
//                    Wt::Dbo::ptr<Echoes::Dbo::MediaType> ptrMedia = m_session.find<Echoes::Dbo::MediaType>()
//                            .where("\"MED_ID\" = ?").bind(sMedia)
//                            .where("\"MED_DELETE\" IS NULL");
//
//                    if (!ptrMedia)
//                    {
//                        res = EReturnCode::NOT_FOUND;
//                        responseMsg = "{\n\t\"message\":\"Media not found\"\n}";
//                        return res;
//                    }
//                    
//                    Wt::Dbo::ptr<Echoes::Dbo::AlertCriteria> ptrCriterion = m_session.find<Echoes::Dbo::AlertCriteria>()
//                            .where("\"ACR_ID\" = ?").bind(m_pathElements[11])
//                            .where("\"ACR_DELETE\" IS NULL");
//
//                    if (!ptrCriterion)
//                    {
//                        res = EReturnCode::NOT_FOUND;
//                        responseMsg = "{\n\t\"message\":\"Criterion not found\"\n}";
//                        return res;
//                    }
//
//                    //FIXME
////                    Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasInformationCriteria> ptrInformationCriteriaAlias = _session.find<Echoes::Dbo::AlertMessageAliasInformationCriteria>()
////                            .where("\"URO_ID_URO_ID\" = ?").bind(sRole)
////                            .where("\"SEA_ID\" = ?").bind(infPtr->pk.search->pk.id)
////                            .where("\"SRC_ID\" = ?").bind(infPtr->pk.search->pk.source->pk.id)
////                            .where("\"PLG_ID_PLG_ID\" = ?").bind(infPtr->pk.search->pk.source->pk.plugin.id())
////                            .where("\"INF_VALUE_NUM\" = ?").bind(infPtr->pk.subSearchNumber)
////                            .where("\"INU_ID_INU_ID\" = ?").bind(infPtr->pk.unit.id())
////                            .where("\"MED_ID_MED_ID\" = ?").bind(sMedia)
////                            .where("\"ACR_ID_ACR_ID\" = ?").bind(ptrCriterion.id());
////                    if (ptrInformationCriteriaAlias) 
////                    {
////                        ptrInformationCriteriaAlias.modify()->alias = sValue;
////                    }
////                    else
////                    {
////                        Echoes::Dbo::AlertMessageAliasInformationCriteria *newInformationCriteriaAlias = new Echoes::Dbo::AlertMessageAliasInformationCriteria();
////                        newInformationCriteriaAlias->pk.information = infPtr;
////                        newInformationCriteriaAlias->pk.userRole = ptrRole;
////                        newInformationCriteriaAlias->pk.media = ptrMedia;
////                        newInformationCriteriaAlias->pk.alertCriteria = ptrCriterion;
////                        newInformationCriteriaAlias->alias = sValue;
////                        ptrInformationCriteriaAlias = _session.add<Echoes::Dbo::AlertMessageAliasInformationCriteria>(newInformationCriteriaAlias);
////                    }
//                    res = EReturnCode::OK;
//                }
//                else
//                {
//                    res = EReturnCode::NOT_FOUND;
//                    responseMsg = "{\"message\":\"Information not found\"}";
//                }
//
//                transaction.commit();
//            }
//            catch (Wt::Dbo::Exception const& e) 
//            {
//                Wt::log("error") << e.what();
//                res = EReturnCode::SERVICE_UNAVAILABLE;
//                responseMsg = "{\"message\":\"Service Unavailable\"}";
//            }
//        }
//        catch (Wt::Json::ParseError const& e)
//        {
//            res = EReturnCode::BAD_REQUEST;
//            responseMsg = "{\"message\":\"Problems parsing JSON\"}";
//            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON:" << sRequest;
//        }
//        catch (Wt::Json::TypeException const& e)
//        {
//            res = EReturnCode::BAD_REQUEST;
//            responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
//            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON.:" << sRequest;
//        }   
//    }
//    return res; 
//}

EReturnCode PluginResource::putAliasForPlugin(string &responseMsg, const string &sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
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
                Wt::Dbo::Transaction transaction(m_session);

                // Information exist?
                Wt::Dbo::ptr<Echoes::Dbo::Plugin> infPlg = m_session.find<Echoes::Dbo::Plugin>()
                        .where("\"PLG_ID\" = ?").bind(m_pathElements[1])

                        ;
                if(infPlg)
                {
                    //Relier une unité à l'info
                    // unit exist?
                    Wt::Dbo::ptr<Echoes::Dbo::UserRole> ptrRole = m_session.find<Echoes::Dbo::UserRole>()
                        .where("\"URO_ID\" = ?").bind(sRole)
                        .where("\"URO_DELETE\" IS NULL");
                
                    if (!ptrRole)
                    {
                        res = EReturnCode::NOT_FOUND;
                        responseMsg = "{\n\t\"message\":\"Role not found\"\n}";
                        return res;
                    }

                    Wt::Dbo::ptr<Echoes::Dbo::MediaType> mtyPtr = m_session.find<Echoes::Dbo::MediaType>()
                            .where("\"MED_ID\" = ?").bind(sMedia)
                            .where("\"MED_DELETE\" IS NULL");

                    if (!mtyPtr)
                    {
                        res = EReturnCode::NOT_FOUND;
                        responseMsg = "{\n\t\"message\":\"Media not found\"\n}";
                        return res;
                    }


                    Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasPlugin> ptrPluginAlias = m_session.find<Echoes::Dbo::AlertMessageAliasPlugin>()
                            .where("\"URO_ID_URO_ID\" = ?").bind(sRole)
                            .where("\"PLG_ID_PLG_ID\" = ?").bind(infPlg.id())
                            .where("\"MED_ID_MED_ID\" = ?").bind(sMedia);
                    if (ptrPluginAlias) 
                    {
                        ptrPluginAlias.modify()->alias = sValue;
                    }
                    else
                    {
                        Echoes::Dbo::AlertMessageAliasPlugin *newPluginAlias = new Echoes::Dbo::AlertMessageAliasPlugin();
                        newPluginAlias->pk.plugin = infPlg;
                        newPluginAlias->pk.userRole = ptrRole;
                        newPluginAlias->pk.mediaType = mtyPtr;
                        newPluginAlias->alias = sValue;
                        ptrPluginAlias = m_session.add<Echoes::Dbo::AlertMessageAliasPlugin>(newPluginAlias);
                    }
                    res = EReturnCode::OK;
                }
                else
                {
                    res = EReturnCode::NOT_FOUND;
                    responseMsg = "{\"message\":\"Information not found\"}";
                }

                transaction.commit();
            }
            catch (Wt::Dbo::Exception const& e) 
            {
                Wt::log("error") << e.what();
                res = EReturnCode::SERVICE_UNAVAILABLE;
                responseMsg = "{\"message\":\"Service Unavailable\"}";
            }
        }
        catch (Wt::Json::ParseError const& e)
        {
            res = EReturnCode::BAD_REQUEST;
            responseMsg = "{\"message\":\"Problems parsing JSON\"}";
            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON:" << sRequest;
        }
        catch (Wt::Json::TypeException const& e)
        {
            res = EReturnCode::BAD_REQUEST;
            responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON.:" << sRequest;
        }   
    }
    return res; 
}

//EReturnCode PluginResource::patchInformationForSeaSrcAndPlg(string &responseMsg, const string &sRequest)
//{
//    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    if((res = pluginIsAccessible(responseMsg)) == 200)
//    {
//        Wt::WString infName, infCalculate;
//        bool infDisplay;
//
//        try
//        {
//            Wt::Json::Object result;                   
//            Wt::Json::parse(sRequest, result);
//
//            //information
//            infName = result.get("inf_name");
//            infDisplay = result.get("inf_display");
//            infCalculate = result.get("inf_calculate");
//
//            try
//            {
//                Wt::Dbo::Transaction transaction(m_session);
//
//                // Information exist?
//                Wt::Dbo::ptr<Echoes::Dbo::Information> infPtr = m_session.find<Echoes::Dbo::Information>()
//                        .where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1])
//                        .where("\"SRC_ID\" = ?").bind(m_pathElements[3])
//                        .where("\"SEA_ID\" = ?").bind(m_pathElements[5])
//                        .where("\"INF_VALUE_NUM\" = ?").bind(m_pathElements[7])
//                        .where("\"INU_ID_INU_ID\" = ?").bind(m_pathElements[9]);
//                if(infPtr)
//                {
//                    //Relier une unité à l'info
//                    // unit exist?
//                    Wt::Dbo::ptr<Echoes::Dbo::SearchUnit> seaUnitPtr = m_session.find<Echoes::Dbo::SearchUnit>()
//                            .where("\"INF_VALUE_NUM\" = ?").bind(m_pathElements[7])
//                            .where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1])
//                            .where("\"SRC_ID\" = ?").bind(m_pathElements[3])
//                            .where("\"SEA_ID\" = ?").bind(m_pathElements[5]);
//                    if(seaUnitPtr)
//                    {
//
//                        //creation info
//                        infPtr.modify()->name = infName;
//                        infPtr.modify()->display = infDisplay;
//                        infPtr.modify()->calculate = infCalculate;
//                        responseMsg = infPtr->toJSON();
//                        res = EReturnCode::OK;
//                    }
//                    else
//                    {
//                        res = EReturnCode::NOT_FOUND;
//                        responseMsg = "{\"message\":\"Unit not found\"}";
//                    }
//                }
//                else
//                {
//                    res = EReturnCode::NOT_FOUND;
//                    responseMsg = "{\"message\":\"Information not found\"}";
//                }
//
//                transaction.commit();
//            }
//            catch (Wt::Dbo::Exception const& e) 
//            {
//                Wt::log("error") << e.what();
//                res = EReturnCode::SERVICE_UNAVAILABLE;
//                responseMsg = "{\"message\":\"Service Unavailable\"}";
//            }
//        }
//        catch (Wt::Json::ParseError const& e)
//        {
//            res = EReturnCode::BAD_REQUEST;
//            responseMsg = "{\"message\":\"Problems parsing JSON\"}";
//            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON:" << sRequest;
//        }
//        catch (Wt::Json::TypeException const& e)
//        {
//            res = EReturnCode::BAD_REQUEST;
//            responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
//            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON.:" << sRequest;
//        }   
//    }
//    return res; 
//}
//
//EReturnCode PluginResource::patchSearchForSourceAndPlugin(string& responseMsg, const string& sRequest)
//{
//    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    if((res = pluginIsAccessible(responseMsg)) == 200)
//    {
//        bool seaIsStatic;
//        int posKeyValue, seaPeriod;
//
//        try
//        {
//            Wt::Json::Object result;                   
//            Wt::Json::parse(sRequest, result);
//
//            //search
//            seaPeriod = result.get("sea_period");
//            seaIsStatic = result.get("sea_is_static");
//            posKeyValue = result.get("pos_key_value");
//            try
//            {
//                Wt::Dbo::Transaction transaction(m_session);
//                Wt::Dbo::ptr<Echoes::Dbo::Search> seaPtr;
//
//                //modification de la search si elle exist
//                seaPtr = m_session.find<Echoes::Dbo::Search>()
//                        .where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1])
//                        .where("\"SRC_ID\" = ?").bind(m_pathElements[3])
//                        .where("\"SEA_ID\" = ?").bind(m_pathElements[5]);
//                if(seaPtr)
//                {
//                    Wt::Dbo::ptr<Echoes::Dbo::SearchType> seaTypPtr = m_session.find<Echoes::Dbo::SearchType>().where("\"STY_ID\" = ?").bind(seaPtr->searchType.id());
//                    if(seaTypPtr)
//                    {
//
//                        seaPtr.modify()->period = seaPeriod;
//                        //FIXME
////                        seaPtr.modify()->pos_key_value = posKeyValue;
////                        seaPtr.modify()->searchIsStatic = seaIsStatic;
//
//                        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SearchParameter> > searchParameterPtr = seaTypPtr->searchParameters;
//                        //modification des parametres
//                        for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SearchParameter> >::const_iterator i = searchParameterPtr.begin(); i != searchParameterPtr.end(); i++)
//                        {
//                            Wt::Dbo::ptr<Echoes::Dbo::SearchParameterValue> searchParameterValuePtr = m_session.find<Echoes::Dbo::SearchParameterValue>()
//                                    .where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1])
//                                    .where("\"SRC_ID\" = ?").bind(m_pathElements[3])
//                                    .where("\"SEA_ID\" = ?").bind(m_pathElements[5])
//                                    .where("\"SEP_ID_SEP_ID\" = ?").bind(i->id());
//
//                            //recuperation des "search_parameters" dans le JSON
//                            Wt::WString tmp = result.get(i->get()->name.toUTF8());
//
//                            searchParameterValuePtr.modify()->value = tmp;
//                        }
//                        res = EReturnCode::OK;
//                        responseMsg = seaPtr->toJSON();
//                    }
//                    else
//                    {
//                        res = EReturnCode::NOT_FOUND;
//                        responseMsg = "{\"message\":\"Search Type not found\"}";
//                    }
//                }
//                else
//                {
//                    res = EReturnCode::NOT_FOUND;
//                    responseMsg = "{\"message\":\"Search not found\"}";
//                }
//                transaction.commit();
//            }
//            catch (Wt::Dbo::Exception const& e) 
//            {
//                Wt::log("error") << e.what();
//                res = EReturnCode::SERVICE_UNAVAILABLE;
//                responseMsg = "{\"message\":\"Service Unavailable\"}";
//            }
//        }
//        catch (Wt::Json::ParseError const& e)
//        {
//            res = EReturnCode::BAD_REQUEST;
//            responseMsg = "{\"message\":\"Problems parsing JSON\"}";
//            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON:" << sRequest;
//        }
//        catch (Wt::Json::TypeException const& e)
//        {
//            res = EReturnCode::BAD_REQUEST;
//            responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
//            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON.:" << sRequest;
//        }   
//    }
//    return res;
//}
//
//EReturnCode PluginResource::patchParametersSourceForPlugin(string &responseMsg, const string &sRequest)
//{
//    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    if((res = pluginIsAccessible(responseMsg)) == 200)
//    {
//        Wt::Json::Array& parameters = Wt::Json::Array::Empty;
//
//        try
//        {
//            Wt::Json::Object result;                   
//            Wt::Json::parse(sRequest, result);
//            parameters = result.get("parameters");
//            try
//            {
//                Wt::Dbo::Transaction transaction(m_session);
//                Wt::Dbo::ptr<Echoes::Dbo::Source> srcPtr = m_session.find<Echoes::Dbo::Source>()
//                        .where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1])
//                        .where("\"SRC_ID\" = ?").bind(m_pathElements[3]);
//
//                if(srcPtr)
//                {
//                    //modification des parametres
//                    Wt::Dbo::ptr<Echoes::Dbo::SourceParameterValue> srcParamValPtr;
//                    for (Wt::Json::Array::const_iterator idx1 = parameters.begin() ; idx1 < parameters.end(); idx1++)
//                    {
//                        Wt::Json::Object tmp = *idx1;
//                        int srpIdParam = tmp.get("srp_id");
//                        Wt::WString valueParam = tmp.get("value");
//
//
//                        srcParamValPtr = m_session.find<Echoes::Dbo::SourceParameterValue>()
//                                .where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1])
//                                .where("\"SRC_ID\" = ?").bind(m_pathElements[3])
//                                .where("\"SRP_ID_SRP_ID\" = ?").bind(srpIdParam);
//                        if (srcParamValPtr)
//                        {
//                            srcParamValPtr.modify()->value = valueParam;
//                        }
//                        else
//                        {
//                            res = EReturnCode::NOT_FOUND;
//                            responseMsg = "{\"message\":\"Parameter not found\"}";
//                            return res;
//                        }
//                        responseMsg += srcParamValPtr->toJSON();
//                        res = EReturnCode::OK;
//                    }
//                }
//                else
//                {
//                    res = EReturnCode::NOT_FOUND;
//                    responseMsg = "{\"message\":\"Source not found\"}";
//                }
//                transaction.commit();
//            }
//            catch (Wt::Dbo::Exception const& e) 
//            {
//                Wt::log("error") << e.what();
//                res = EReturnCode::SERVICE_UNAVAILABLE;
//                responseMsg = "{\"message\":\"Service Unavailable\"}";
//            }
//        }
//        catch (Wt::Json::ParseError const& e)
//        {
//            res = EReturnCode::BAD_REQUEST;
//            responseMsg = "{\"message\":\"Problems parsing JSON\"}";
//            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON:" << sRequest;
//        }
//        catch (Wt::Json::TypeException const& e)
//        {
//            res = EReturnCode::BAD_REQUEST;
//            responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
//            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON.:" << sRequest;
//        }   
//    }
//    return res;
//}

//void PluginResource::processPatchRequest(Wt::Http::Response &response)
//{
//   string responseMsg = "", nextElement = "", sRequest = "";
//    
//    nextElement = getNextElementFromPath();
//    if(!nextElement.compare(""))
//    {
//        m_statusCode = EReturnCode::BAD_REQUEST;
//        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//    }
//    else
//    {
//        try
//        {
//            boost::lexical_cast<unsigned long long>(nextElement);
//
//            nextElement = getNextElementFromPath();
//
//            if (!nextElement.compare("sources"))
//            {
//                nextElement = getNextElementFromPath();
//                if(!nextElement.compare(""))
//                {
//                    m_statusCode = EReturnCode::BAD_REQUEST;
//                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                }
//                else
//                {
//                    boost::lexical_cast<unsigned long long>(nextElement);
//                    nextElement = getNextElementFromPath();
//                    if (!nextElement.compare("parameters"))
//                    {
//                        m_statusCode = patchParametersSourceForPlugin(responseMsg, sRequest);
//                    }
//                    else
//                    {
//                        m_statusCode = EReturnCode::BAD_REQUEST;
//                        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                    }
//                }
//            }
//            else
//            {
//                m_statusCode = EReturnCode::BAD_REQUEST;
//                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//            }
//        }
//        catch(boost::bad_lexical_cast &)
//        {
//            m_statusCode = EReturnCode::BAD_REQUEST;
//            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//        }
//    }
//
//    response.setStatus(m_statusCode);
//    response.out() << responseMsg;
//    return;
//}

//EReturnCode PluginResource::deleteInformationForSeaSrcAndPlg(string& responseMsg)
//{
//    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    if((res = pluginIsAccessible(responseMsg)) == 200)
//    {
//        try 
//        {  
//            Wt::Dbo::Transaction transaction(m_session);
//
//            Wt::Dbo::ptr<Echoes::Dbo::Information> informationPtr = m_session.find<Echoes::Dbo::Information>()
//                    .where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1])
//                    .where("\"SRC_ID\" = ?").bind(m_pathElements[3])
//                    .where("\"SEA_ID\" = ?").bind(m_pathElements[5])
//                    .where("\"INF_VALUE_NUM\" = ?").bind(m_pathElements[7])
//                    .where("\"INU_ID_INU_ID\" = ?").bind(m_pathElements[9]);
//            //information exist ?
//            if(informationPtr)
//            {
//                Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertValue>> avaCollec = m_session.find<Echoes::Dbo::AlertValue>()
//                        .where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1])
//                        .where("\"SRC_ID\" = ?").bind(m_pathElements[3])
//                        .where("\"SEA_ID\" = ?").bind(m_pathElements[5])
//                        .where("\"INF_VALUE_NUM\" = ?").bind(m_pathElements[7])
//                        .where("\"INU_ID_INU_ID\" = ?").bind(m_pathElements[9])
//                        .where("\"AVA_DELETE\" IS NULL");
//                //verif si l'info n'est pas utilisée                                                                
//                if (avaCollec.size() == 0)
//                {                
//                    //supprime l'info
//
//                    informationPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
//
//                    res = EReturnCode::NO_CONTENT;
//                }
//                else
//                {
//                    res = EReturnCode::CONFLICT;
//                    responseMsg = "{\"message\":\"Conflict, an alert use this information\"}";
//                }
//            }
//            else
//            {
//                responseMsg = "{\"message\":\"Information Not Found\"}";
//                res = EReturnCode::NOT_FOUND;
//            }
//
//            transaction.commit();               
//        }
//        catch (Wt::Dbo::Exception const& e) 
//        {
//            Wt::log("error") << e.what();
//            res = EReturnCode::SERVICE_UNAVAILABLE;
//            responseMsg = "{\"message\":\"Service Unavailable\"}";
//        }
//    }    
//    return res;
//}

//EReturnCode PluginResource::deleteSearchForSourceAndPlugin(string& responseMsg)
//{
//    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    if((res = pluginIsAccessible(responseMsg)) == 200)
//    {
//        try 
//        {  
//            Wt::Dbo::Transaction transaction(m_session);
//
//            Wt::Dbo::ptr<Echoes::Dbo::Search> seaPtr = m_session.find<Echoes::Dbo::Search>()
//                    .where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1])
//                    .where("\"SRC_ID\" = ?").bind(m_pathElements[3])
//                    .where("\"SEA_ID\" = ?").bind(m_pathElements[5]);
//            //search exist ?
//            if(seaPtr)
//            {
//                Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Information>> infCollec = m_session.find<Echoes::Dbo::Information>()
//                        .where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1])
//                        .where("\"SRC_ID\" = ?").bind(m_pathElements[3])
//                        .where("\"SEA_ID\" = ?").bind(m_pathElements[5]);
//                //FIXME
////                for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Information> >::const_iterator i = infCollec.begin(); i != infCollec.end(); i++) 
////                {
////                    string path = "/" + vPathElements[1] + "/sources/" + vPathElements[3] + "/searches/" + 
////                                             vPathElements[5] + "/inf_values/" + boost::lexical_cast<string>(i->get()->pk.subSearchNumber) +
////                                            "/units/" + boost::lexical_cast<string>(i->get()->pk.unit.id());
////                    boost::split(vPathElements, path, boost::is_any_of("/"), boost::token_compress_on);
////
////                    res = deleteInformationForSeaSrcAndPlg(responseMsg);
////                    if( res != 204)
////                    {
////                        res = EReturnCode::CONFLICT;
////                        responseMsg = "{\"message\":\"Conflict, an alert use this search\"}";
////                        return res;
////                    }
////                }           
//
//                seaPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
//                res = EReturnCode::NO_CONTENT;   
//            }
//            else
//            {
//                responseMsg = "{\"message\":\"Search Not Found\"}";
//                res = EReturnCode::NOT_FOUND;
//            }
//            transaction.commit();               
//        }
//        catch (Wt::Dbo::Exception const& e) 
//        {
//            Wt::log("error") << e.what();
//            res = EReturnCode::SERVICE_UNAVAILABLE;
//            responseMsg = "{\"message\":\"Service Unavailable\"}";
//        }
//    }    
//    return res;
//    
//}

EReturnCode PluginResource::deletePlugin(string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    if((res = pluginIsAccessible(responseMsg)) == 200)
    {
        try 
        {  
            Wt::Dbo::Transaction transaction(m_session);
            Wt::Dbo::ptr<Echoes::Dbo::Plugin> plgPtr = m_session.find<Echoes::Dbo::Plugin>().where("\"PLG_ID\" = ?").bind(m_pathElements[1]);
            //verif si le plugin existe
            if(plgPtr)
            {
                plgPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
                res = EReturnCode::NO_CONTENT;
            }
            else
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = "{\"message\":\"Plugin not found\"}";
            }
            transaction.commit();               
        }
        catch (Wt::Dbo::Exception const& e) 
        {
            Wt::log("error") << e.what();
            res = EReturnCode::SERVICE_UNAVAILABLE;
            responseMsg = "{\"message\":\"Service Unavailable\"}";
        }
    }    
    return res;
}

EReturnCode PluginResource::deleteSourceForPlugin(string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    if((res = pluginIsAccessible(responseMsg)) == 200)
    {
        try 
        {  
            Wt::Dbo::Transaction transaction(m_session);
            
            m_session.execute("DELETE FROM \"TJ_PLG_SRC\" WHERE \"T_SOURCE_SRC_SRC_ID\" = " + m_pathElements[3]
                    + " AND \"T_PLUGIN_PLG_PLG_ID\" = " + m_pathElements[1]);


            transaction.commit();               
        }
        catch (Wt::Dbo::Exception const& e) 
        {
            Wt::log("error") << e.what();
            res = EReturnCode::SERVICE_UNAVAILABLE;
            responseMsg = "{\"message\":\"Service Unavailable\"}";
        }
    }
    return res; 
}

void PluginResource::processDeleteRequest(Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";

    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
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
                m_statusCode = deletePlugin(responseMsg);
            }
            else if(!nextElement.compare("sources"))
            {
                nextElement = getNextElementFromPath();
                boost::lexical_cast<unsigned long long>(nextElement);

                nextElement = getNextElementFromPath();
                if(!nextElement.compare(""))
                {
                    m_statusCode = deleteSourceForPlugin(responseMsg);
                }
//                    if(!nextElement.compare("searches"))
//                {
//                    
//                    nextElement = getNextElementFromPath();
//                    boost::lexical_cast<unsigned long long>(nextElement);
//
//                    nextElement = getNextElementFromPath();
//
//                    if(!nextElement.compare(""))
//                    {
//                        m_statusCode = deleteSearchForSourceAndPlugin(responseMsg);
//                    }
//                    else if (!nextElement.compare("inf_values"))
//                    {
//                        nextElement = getNextElementFromPath();
//                        boost::lexical_cast<unsigned long long>(nextElement);
//                        nextElement = getNextElementFromPath();
//                        if (!nextElement.compare("units"))
//                        {
//                            nextElement = getNextElementFromPath();
//                            boost::lexical_cast<unsigned long long>(nextElement);
//                            nextElement = getNextElementFromPath();
//                            if (!nextElement.compare("informations"))
//                            {
//                                m_statusCode = deleteInformationForSeaSrcAndPlg(responseMsg);
//                            }
//                            else
//                            {
//                                m_statusCode = EReturnCode::BAD_REQUEST;
//                                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                            }
//                        }
//                        else
//                        {
//                            m_statusCode = EReturnCode::BAD_REQUEST;
//                            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                        }
//                    }
//                    else
//                    {
//                        m_statusCode = EReturnCode::BAD_REQUEST;
//                        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                    }
//                }
                else
                {
                    m_statusCode = EReturnCode::BAD_REQUEST;
                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                }
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            m_statusCode = EReturnCode::BAD_REQUEST;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}

void PluginResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    m_role = "";
    m_media = "";
    if (!request.getParameterValues("role").empty())
    {
        m_role = request.getParameterValues("role")[0];
    }
    
    if (!request.getParameterValues("media").empty())
    {
        m_media = request.getParameterValues("media")[0];
    }
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);
    return;
}

