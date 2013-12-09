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

EReturnCode PluginResource::getAliasForPlugin(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    if (m_role.empty())
    {
        res = EReturnCode::BAD_REQUEST;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        return res;
    }

    if (m_media_type.empty())
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
                .where("\"MED_ID_MED_ID\" = ?").bind(m_media_type)
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
//    if((res = pluginIsAccessible(responseMsg)) == 200)
//    {
//        try
//        {
//            Wt::Dbo::Transaction transaction(m_session);
//
//            Wt::Dbo::collection <Wt::Dbo::ptr<Echoes::Dbo::Information>> information = m_session.find<Echoes::Dbo::Information>()
//                    .where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1])
//                    .where("\"INF_DELETE\" IS NULL")
//                    .orderBy("\"INF_ID\"");
//
//            if(information.size() > 0 )
//            {
//                responseMsg = "[\n";
//                for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Information> >::const_iterator i = information.begin(); i != information.end(); i++) 
//                { 
//                    Echoes::Dbo::Information info(*i->get());
//                    responseMsg +=  info.toJSON();
//                    idx++;
//                    if(information.size()-idx > 0)
//                    {
//                        responseMsg += ",\n";
//                    }
//                }
//                responseMsg += "\n]\n";
//                res = EReturnCode::OK;
//            }
//            else
//            {
//                res = EReturnCode::NOT_FOUND;
//                responseMsg = "{\"message\":\"Information not found\"}";
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
    return res;  
}

//FIXME : MOVE OUT
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
                m_statusCode = getPlugin(responseMsg);
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
//                nextElement = getNextElementFromPath();
//                if(!nextElement.compare(""))
//                {
//                    m_statusCode = getSourceForPlugin(responseMsg);
//                }
//                else
//                {
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
//                }
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
//    if(!nextElement.compare(""))
//    {
//        m_statusCode = postPlugin(responseMsg, m_requestData);
//    }
//    else
//    {       
//        try
//        {
//            boost::lexical_cast<unsigned long long>(nextElement);
//
//            nextElement = getNextElementFromPath();
//            if(!nextElement.compare("sources"))
//            {
//                nextElement = getNextElementFromPath();
//                if (!nextElement.compare(""))
//                {
//                    m_statusCode = postSourceForPlugin(responseMsg, m_requestData);
//                }
//                else
//                {
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
//                    m_statusCode = EReturnCode::BAD_REQUEST;
//                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
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

EReturnCode PluginResource::putAliasForPlugin(string &responseMsg, const string &sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    Wt::WString sRole;
    Wt::WString sMedia;
    Wt::WString sValue;
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
//                Wt::Dbo::ptr<Echoes::Dbo::Plugin> infPlg = m_session.find<Echoes::Dbo::Plugin>()
//                        .where("\"PLG_ID\" = ?").bind(m_pathElements[1])
//
//                        ;
//                if(infPlg)
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
//                    Wt::Dbo::ptr<Echoes::Dbo::MediaType> mtyPtr = m_session.find<Echoes::Dbo::MediaType>()
//                            .where("\"MED_ID\" = ?").bind(sMedia)
//                            .where("\"MED_DELETE\" IS NULL");
//
//                    if (!mtyPtr)
//                    {
//                        res = EReturnCode::NOT_FOUND;
//                        responseMsg = "{\n\t\"message\":\"Media not found\"\n}";
//                        return res;
//                    }
//
//
//                    Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasPlugin> ptrPluginAlias = m_session.find<Echoes::Dbo::AlertMessageAliasPlugin>()
//                            .where("\"URO_ID_URO_ID\" = ?").bind(sRole)
//                            .where("\"PLG_ID_PLG_ID\" = ?").bind(infPlg.id())
//                            .where("\"MED_ID_MED_ID\" = ?").bind(sMedia);
//                    if (ptrPluginAlias) 
//                    {
//                        ptrPluginAlias.modify()->alias = sValue;
//                    }
//                    else
//                    {
//                        Echoes::Dbo::AlertMessageAliasPlugin *newPluginAlias = new Echoes::Dbo::AlertMessageAliasPlugin();
//                        newPluginAlias->pk.plugin = infPlg;
//                        newPluginAlias->pk.userRole = ptrRole;
//                        newPluginAlias->pk.mediaType = mtyPtr;
//                        newPluginAlias->alias = sValue;
//                        ptrPluginAlias = m_session.add<Echoes::Dbo::AlertMessageAliasPlugin>(newPluginAlias);
//                    }
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
    return res; 
}

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

EReturnCode PluginResource::deletePlugin(string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    if((res = pluginIsAccessible(responseMsg)) == 200)
//    {
//        try 
//        {  
//            Wt::Dbo::Transaction transaction(m_session);
//            Wt::Dbo::ptr<Echoes::Dbo::Plugin> plgPtr = m_session.find<Echoes::Dbo::Plugin>().where("\"PLG_ID\" = ?").bind(m_pathElements[1]);
//            //verif si le plugin existe
//            if(plgPtr)
//            {
//                plgPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
//                res = EReturnCode::NO_CONTENT;
//            }
//            else
//            {
//                res = EReturnCode::NOT_FOUND;
//                responseMsg = "{\"message\":\"Plugin not found\"}";
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
//                if(!nextElement.compare(""))
//                {
//                    m_statusCode = deleteSourceForPlugin(responseMsg);
//                }
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
//                else
//                {
//                    m_statusCode = EReturnCode::BAD_REQUEST;
//                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
//                }
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
    m_media_type = "";
    if (request.getParameter("role") != 0)
    {
        m_role = *request.getParameter("role");
    }
    
    if (request.getParameter("media_type") != 0)
    {
        m_media_type = *request.getParameter("media_type");
    }

    PublicApiResource::handleRequest(request, response);
    return;
}

