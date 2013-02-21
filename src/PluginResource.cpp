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


string PluginResource::getKeyValueForInformation()
{
    string res = "";
    try
    {
        Wt::Dbo::Transaction transaction(*session);

        Wt::Dbo::ptr<Information2> ptrInfoKey = session->find<Information2>().where("\"INF_NAME\" = ?").bind(this->vPathElements[1])
                                    .limit(1);

        if (!ptrInfoKey)
        {
            this->statusCode = 404;
            res = "{\"message\":\"Information not found\"}";
            return res;
        }

        std::string queryString = 
        "SELECT iva FROM \"T_INFORMATION_VALUE_IVA\" iva WHERE \"IVA_ID\" IN ( SELECT \"IVA_ID\" FROM"
        "("
        "SELECT DISTINCT ON (\"IVA_VALUE\") \"IVA_VALUE\", \"IVA_ID\" FROM"
        "(" "SELECT iva.\"IVA_VALUE\", iva.\"IVA_ID\" FROM \"T_INFORMATION_VALUE_IVA\" iva"
        " WHERE \"SEA_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[5]) + 
        " AND \"SRC_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[3]) + 
        " AND \"PLG_ID_PLG_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[1]) + 
        " AND \"INF_VALUE_NUM\" = " + boost::lexical_cast<std::string>(this->vPathElements[7]) + 
        " AND \"INU_ID_INU_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[9]) + 
        " ORDER BY \"IVA_ID\" DESC LIMIT 50) sr"
        " ) sr_sr"
        ")";

        Wt::Dbo::collection<Wt::Dbo::ptr<InformationValue> > collPtrIva = session->query<Wt::Dbo::ptr<InformationValue> >(queryString);

        if(collPtrIva.size() > 0)
        {
            res = "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<InformationValue> >::const_iterator i = collPtrIva.begin(); i != collPtrIva.end(); i++)
            {
              
                res += i->modify()->toJSON();
                /*res += "{\n\"";
                res += "  \"iva_value\" : \"" + boost::lexical_cast<std::string>(i->id()) + "\"\n\"";
                res += "}\n";*/
            }
            res = "]";
            this->statusCode = 200;
        }
        else
        {
            this->statusCode = 404;
            res = "{\"message\":\"Information value not found\"}";
            return res;
        }
    }
    catch (Wt::Dbo::Exception const &e)
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}

string PluginResource::getInformationListForPlugin()
{
    string res = "";
    try
    {
        Wt::Dbo::Transaction transaction(*this->session);
        std::string queryStr = " SELECT inf FROM \"T_INFORMATION_INF\" inf "
                            "WHERE \"INF_DISPLAY\" = TRUE AND \"PLG_ID_PLG_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[1]) +
                            " AND \"INF_DELETE\" IS NULL";

        Wt::Dbo::Query<Wt::Dbo::ptr<Information2> > queryRes = session->query<Wt::Dbo::ptr<Information2> >(queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<Information2> > information = queryRes.resultList();

        if(information.size() > 0 )
        {
            res = "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Information2> >::const_iterator i = information.begin(); i != information.end(); i++) 
            {
                res += "\t" + i->modify()->toJSON();
                /*
                res += "{\n\"";
                res +="  \"id\" : \"" + boost::lexical_cast<std::string > (i->id()) + "\",\n\"";
                res += "  \"inf_name\" : \"" + boost::lexical_cast<std::string>((*i).get()->name) + "\"\n\"";
                res += "}\n";*/
            }
            res = "]";
            this->statusCode = 200;
            transaction.commit();
        }
        else
        {
            this->statusCode = 404;
            res = "{\"message\":\"Information not found\"}";
            return res;
        }
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}

void PluginResource::processGetRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";
    
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

            if(!nextElement.compare("informations"))
            {
                responseMsg = getInformationListForPlugin();
            }
            else if (!nextElement.compare("sources"))
            {
                nextElement = getNextElementFromPath();
                boost::lexical_cast<unsigned int>(nextElement);
                nextElement = getNextElementFromPath();
                if (!nextElement.compare("searches"))
                {
                    nextElement = getNextElementFromPath();
                    boost::lexical_cast<unsigned int>(nextElement);
                    nextElement = getNextElementFromPath();
                    
                    if (!nextElement.compare("inf_values"))
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
                                responseMsg = getKeyValueForInformation();
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


void PluginResource::processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    
    return ;
}


void PluginResource::processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}


void PluginResource::processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}



void PluginResource::processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    
    return;
}


void PluginResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);

    return;
}


PluginResource::~PluginResource()
{
    beingDeleted();
}













/*

void PluginResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
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
  
  /*  
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
                    /// A bouger dans AssetRessource
                    if(splitPath[2] == "plugin") // liste les plugin d'un asset
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
*/