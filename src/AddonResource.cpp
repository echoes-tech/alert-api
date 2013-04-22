/* 
 * API AddonResource
 * @author ECHOES Technologies (GDR)
 * @date 6/03/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "AddonResource.h"

Wt::Json::Array Wt::Json::Array::Empty;

using namespace std;

AddonResource::AddonResource() : PublicApiResource::PublicApiResource()
{
}

AddonResource::AddonResource(const AddonResource& orig) : PublicApiResource::PublicApiResource(orig)
{
}

AddonResource::~AddonResource()
{
}

unsigned short AddonResource::getSearchTypeForAddon(string &responseMsg) const
{
    unsigned short res = 500;
    unsigned idx = 0;
    try 
    {
        Wt::Dbo::Transaction transaction(*this->session);
        string queryStr = "SELECT set FROM \"T_SEARCH_TYPE_STY\" set "
                               " WHERE \"STY_ID\" IN"
                                "("
                                    "SELECT \"T_SEARCH_TYPE_STY_STY_ID\" FROM \"TJ_ADO_STY\" "
                                    "WHERE \"T_ADDON_ADO_ADO_ID\" = " + boost::lexical_cast<string>(this->vPathElements[1]) +
                                ")";
        Wt::Dbo::Query<Wt::Dbo::ptr<SearchType> > queryRes = session->query<Wt::Dbo::ptr<SearchType> >(queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<SearchType> > seaTypePtr = queryRes.resultList();
        
        if(seaTypePtr.size() != 0)
        {
            responseMsg = "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<SearchType> >::const_iterator i = seaTypePtr.begin(); i != seaTypePtr.end(); ++i)
            {
                i->modify()->setId(i->id());
                responseMsg += "\t" + i->modify()->toJSON();
                ++idx;
                if(seaTypePtr.size()-idx > 0)
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
            responseMsg = "{\"message\":\"Search parameter not found\"}";
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

unsigned short AddonResource::getParameterForAddon(string& responseMsg) const
{
    unsigned short res = 500;
    unsigned idx = 0;
    try 
    {
        Wt::Dbo::Transaction transaction(*this->session);

        
        string queryStr = "SELECT srp FROM \"T_SOURCE_PARAMETER_SRP\" srp"
                " WHERE \"SRP_ID\" IN "
                "("
                "SELECT \"T_SOURCE_PARAMETER_SRP_SRP_ID\" FROM \"TJ_ADO_SRP\" WHERE \"T_ADDON_ADO_ADO_ID\" ="
                + this->vPathElements[1]
                + " )";
       
        Wt::Dbo::Query<Wt::Dbo::ptr<SourceParameter>> queryRes = session->query<Wt::Dbo::ptr<SourceParameter>>(queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<SourceParameter>> srcParamPtr = queryRes.resultList();
        if(srcParamPtr.size() > 0)
        {
            responseMsg = "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<SourceParameter>>::const_iterator i = srcParamPtr.begin(); i != srcParamPtr.end(); ++i)
            {
                i->modify()->setId(i->id());
                responseMsg += "\t" + i->modify()->toJSON();
                ++idx;
                if(srcParamPtr.size()-idx > 0)
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
            responseMsg = "{\"message\":\"Source parameter not found\"}";
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

unsigned short AddonResource::getAddonList(string& responseMsg) const
{
    unsigned short res = 500;
    unsigned idx = 0;
    try 
    {
        Wt::Dbo::Transaction transaction(*this->session);
        Wt::Dbo::collection<Wt::Dbo::ptr<Addon> > addonPtr = session->find<Addon>();
        responseMsg = "[\n";
        for (Wt::Dbo::collection<Wt::Dbo::ptr<Addon>>::const_iterator i = addonPtr.begin(); i != addonPtr.end(); ++i)
        {
            i->modify()->setId(i->id());
            responseMsg += "\t" + i->modify()->toJSON();
            ++idx;
            if(addonPtr.size()-idx > 0)
            {
                responseMsg += ",\n";
            }
        }
        responseMsg += "]\n";
        res = 200;
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

void AddonResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";
    
    nextElement = getNextElementFromPath();

    if(!nextElement.compare(""))
    {
        this->statusCode = getAddonList(responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);
            nextElement = getNextElementFromPath();

            if(!nextElement.compare("parameters"))
            {
                this->statusCode = getParameterForAddon(responseMsg);
            }
            else if(!nextElement.compare("search_types"))
            {
                this->statusCode = getSearchTypeForAddon(responseMsg);
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

unsigned short AddonResource::postAddon(string& responseMsg, const string& sRequest)
{
    unsigned short res = 500;
    Wt::WString name;
    Wt::Json::Array& parameter = Wt::Json::Array::Empty, searchType = Wt::Json::Array::Empty;

    try
    {
        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);
        //descriptif
        name = result.get("name");
        parameter = result.get("parameters");
        
        try
        {
            Wt::Dbo::Transaction transaction(*session);
            Addon *addon = new Addon;
            addon->name = name;
            Wt::Dbo::ptr<Addon> addonPtr = session->add<Addon>(addon);

            if(parameter.size() > 0)
            {
                for (Wt::Json::Array::const_iterator idx1 = parameter.begin() ; idx1 < parameter.end(); idx1++)
                {
                    Wt::Json::Object tmp = *idx1;
                    Wt::WString namePar = tmp.get("name");
                    Wt::WString formatPar = tmp.get("format");

                    //verif si le param exit déjà en base sinon on l'ajoute
                    Wt::Dbo::ptr<SourceParameter> srcParamPtr = session->find<SourceParameter>()
                            .where("\"SRP_NAME\" = ?").bind(namePar)
                            .where("\"SRP_FORMAT\" = ?").bind(formatPar);
                    if (!srcParamPtr)
                    {
                        SourceParameter *sourceParameter = new SourceParameter;
                        sourceParameter->name = namePar;
                        sourceParameter->format = formatPar;
                        Wt::Dbo::ptr<SourceParameter> srcParamPtr1 = session->add<SourceParameter>(sourceParameter);
                        addonPtr.modify()->sourceParameters.insert(srcParamPtr1);
                    }
                    else
                    {
                        addonPtr.modify()->sourceParameters.insert(srcParamPtr);
                    }
                }
                
                searchType = result.get("search_type");
                for (Wt::Json::Array::const_iterator idx2 = searchType.begin(); idx2 < searchType.end(); idx2++)
                {
                    Wt::WString tmp2 = idx2->toString();
                    Wt::Dbo::ptr<SearchType> seaTypePtr = session->find<SearchType>().where("\"STY_ID\" = ?").bind(tmp2);
                    if (seaTypePtr)
                    {
                        addonPtr.modify()->searchTypes.insert(seaTypePtr);
                    }
                    else
                    {
                        res = 404;
                        responseMsg = "{\"message\":\"Search type not found\"}";
                        return res;
                    }
                }
                addonPtr.flush();
                addonPtr.modify()->setId(addonPtr.id());
                responseMsg = addonPtr.modify()->toJSON();
                transaction.commit();
                res = 200;
            }
            else
            {
                 res = 400;
                 responseMsg = "{\n\t\"message\":\"Bad Request.\"\n}"; 
            }
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

void AddonResource::processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "", sRequest = "";

    sRequest = request2string(request);
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->statusCode = postAddon(responseMsg, sRequest);
    }
    else
    {        
        this->statusCode = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }

    response.setStatus(this->statusCode);
    response.out() << responseMsg;
    return ;
}

void AddonResource::processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}

void AddonResource::processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}

unsigned short AddonResource::deleteAddon(string& responseMsg)
{
    unsigned short res = 500;
    try 
    {  
        Wt::Dbo::Transaction transaction(*this->session);
        Wt::Dbo::ptr<Addon> addonPtr = session->find<Addon>().where("\"ADO_ID\" = ?").bind(this->vPathElements[1]);                                                             
        //verif si l'addon existe
        if(addonPtr)
        {
            Wt::Dbo::collection<Wt::Dbo::ptr<Source>> srcCollec = session->find<Source>().where("\"SRC_ADO_ADO_ID\" = ?").bind(this->vPathElements[1]);
            //verif si l'addon n'est pas utilisée
            if(srcCollec.size() == 0)
            {
                //supprime les lien dans table jointe
                string queryString1 = "DELETE FROM \"TJ_ADO_SRP\" "
                                 "WHERE \"T_ADDON_ADO_ADO_ID\" = " + boost::lexical_cast<string>(this->vPathElements[1]);
                session->execute(queryString1);
                
                string queryString1Bis = "DELETE FROM \"TJ_ADO_STY\" "
                                 "WHERE \"T_ADDON_ADO_ADO_ID\" = " + boost::lexical_cast<string>(this->vPathElements[1]);
                session->execute(queryString1Bis);
                
                //supprime l'addon
                string queryString2 = "DELETE FROM \"T_ADDON_ADO\" "
                                           "WHERE \"ADO_ID\" = " + boost::lexical_cast<string>(this->vPathElements[1]);
                session->execute(queryString2);
                //supprime les parametres non utilisés par un addon de la base 
                string queryString3 = "DELETE FROM \"T_SOURCE_PARAMETER_SRP\" "
                                           "WHERE \"SRP_ID\" NOT IN "
                                           "("
                                                "SELECT \"T_SOURCE_PARAMETER_SRP_SRP_ID\" FROM \"TJ_ADO_SRP\" "
                                                "GROUP BY \"T_SOURCE_PARAMETER_SRP_SRP_ID\" "
                                           ")";
                session->execute(queryString3);
                
            }
            else
            {
                res = 409;
                responseMsg = "{\"message\":\"Conflict, a source use this addon\"}";
                return res;
            }
        }
        else
        {
            res = 404;
            responseMsg = "{\"message\":\"Addon not foubd\"}";
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

void AddonResource::processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
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
                this->statusCode = deleteAddon(responseMsg);
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


void AddonResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);

    return;
}

