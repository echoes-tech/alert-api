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
    m_parameters["media_type_id"] = 0;
    m_parameters["user_role_id"] = 0;
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

    if (m_parameters["media_type_id"] <= 0 || m_parameters["user_role_id"] <= 0)
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Criterion Resource] media_types or/and user_role are empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session);

            Wt::Dbo::ptr<Echoes::Dbo::UserRole> uroPtr = m_session.find<Echoes::Dbo::UserRole>()
                    .where(QUOTE(TRIGRAM_USER_ROLE ID) " = ?").bind(m_parameters["user_role_id"])
                    .where(QUOTE(TRIGRAM_USER_ROLE SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(m_session.user()->organization.id())
                    .where(QUOTE(TRIGRAM_USER_ROLE SEP "DELETE") " IS NULL");
            if (uroPtr)
            {
                Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasPlugin> aapPtr = m_session.find<Echoes::Dbo::AlertMessageAliasPlugin>()
                        .where(QUOTE(TRIGRAM_ALERT_MESSAGE_ALIAS_PLUGIN SEP "DELETE") " IS NULL")
                        .where(QUOTE(TRIGRAM_USER_ROLE ID SEP TRIGRAM_USER_ROLE ID) " = ?").bind(m_parameters["user_role_id"])
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE ID SEP TRIGRAM_MEDIA_TYPE ID) " = ?").bind(m_parameters["media_type_id"])
                        .where(QUOTE(TRIGRAM_PLUGIN ID SEP TRIGRAM_PLUGIN ID) " = ?").bind(m_pathElements[1]);

                res = serialize(aapPtr, responseMsg);
            }
            else
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, uroPtr);
            }

            transaction.commit();
        }
        catch (Wt::Dbo::Exception const& e)
        {
            res = EReturnCode::SERVICE_UNAVAILABLE;
            responseMsg = httpCodeToJSON(res, e);
        }
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
    long long uroId;
    long long mtyId;
    Wt::WString value;
    
    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            uroId = result.get("user_role_id");
            mtyId = result.get("media_type_id");
            value = result.get("value");
        }
        catch (Wt::Json::ParseError const& e)
        {
            res = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(res, e);
        }
        catch (Wt::Json::TypeException const& e)
        {
            res = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(res, e);
        }
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Information Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session);

            Wt::Dbo::ptr<Echoes::Dbo::Plugin> plgPtr =  m_session.find<Echoes::Dbo::Plugin>()
                .where(QUOTE(TRIGRAM_PLUGIN ID) " = ?").bind(m_pathElements[1])
                .where(QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(m_session.user()->organization.id())
                .where(QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL");
            if (plgPtr)
            {
                Wt::Dbo::ptr<Echoes::Dbo::UserRole> uroPtr = m_session.find<Echoes::Dbo::UserRole>()
                        .where(QUOTE(TRIGRAM_USER_ROLE ID) " = ?").bind(uroId)
                        .where(QUOTE(TRIGRAM_USER_ROLE SEP "DELETE") " IS NULL");
                if (!uroPtr)
                {
                    res = EReturnCode::NOT_FOUND;
                    responseMsg = httpCodeToJSON(res, uroPtr);
                    return res;
                }

                Wt::Dbo::ptr<Echoes::Dbo::MediaType> mtyPtr = m_session.find<Echoes::Dbo::MediaType>()
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE ID) " = ?").bind(mtyId)
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE SEP "DELETE") " IS NULL");
                if (!mtyPtr)
                {
                    res = EReturnCode::NOT_FOUND;
                    responseMsg = httpCodeToJSON(res, mtyPtr);
                    return res;
                }

                Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasPlugin> aapPtr = m_session.find<Echoes::Dbo::AlertMessageAliasPlugin>()
                        .where(QUOTE(TRIGRAM_USER_ROLE ID SEP TRIGRAM_USER_ROLE ID) " = ?").bind(uroId)
                        .where(QUOTE(TRIGRAM_PLUGIN ID SEP TRIGRAM_PLUGIN ID) " = ?").bind(m_pathElements[1])
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE ID SEP TRIGRAM_MEDIA_TYPE ID) " = ?").bind(mtyId);
                if (aapPtr)
                {
                    aapPtr.modify()->alias = value;
                }
                else
                {
                    Echoes::Dbo::AlertMessageAliasPlugin *newAap = new Echoes::Dbo::AlertMessageAliasPlugin();
                    newAap->pk.plugin = plgPtr;
                    newAap->pk.userRole = uroPtr;
                    newAap->pk.mediaType = mtyPtr;
                    newAap->alias = value;
                    aapPtr = m_session.add<Echoes::Dbo::AlertMessageAliasPlugin>(newAap);
                }
                res = EReturnCode::OK;
            }
            else
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, plgPtr);
            }
            transaction.commit();
        }
        catch (Wt::Dbo::Exception const& e)
        {
            res = EReturnCode::SERVICE_UNAVAILABLE;
            responseMsg = httpCodeToJSON(res, e);
        }
    }

    return res; 
}

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

