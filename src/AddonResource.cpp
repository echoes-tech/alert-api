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

using namespace std;

AddonResource::AddonResource() : PublicApiResource::PublicApiResource()
{
}

AddonResource::~AddonResource()
{
}

unsigned short AddonResource::getSearchTypeForAddon(string &responseMsg)
{
    unsigned short res = 500;
    unsigned idx = 0;
    try 
    {
        Wt::Dbo::Transaction transaction(m_session);
        string queryStr = "SELECT set FROM \"T_SEARCH_TYPE_STY\" set "
                               " WHERE \"STY_ID\" IN"
                                "("
                                    "SELECT \"T_SEARCH_TYPE_STY_STY_ID\" FROM \"TJ_ADO_STY\" "
                                    "WHERE \"T_ADDON_ADO_ADO_ID\" = " + m_pathElements[1] +
                                ")";
        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::SearchType> > queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::SearchType> >(queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SearchType> > seaTypePtr = queryRes.resultList();
        
        if(seaTypePtr.size() != 0)
        {
            responseMsg = "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SearchType> >::const_iterator i = seaTypePtr.begin(); i != seaTypePtr.end(); ++i)
            {
                i->modify()->setId(i->id());
                responseMsg += "\t" + i->get()->toJSON();
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

unsigned short AddonResource::getParameterForAddon(string& responseMsg)
{
    unsigned short res = 500;
    unsigned idx = 0;
    try 
    {
        Wt::Dbo::Transaction transaction(m_session);

        
        string queryStr = "SELECT srp FROM \"T_SOURCE_PARAMETER_SRP\" srp"
                " WHERE \"SRP_ID\" IN "
                "("
                "SELECT \"T_SOURCE_PARAMETER_SRP_SRP_ID\" FROM \"TJ_ADO_SRP\" WHERE \"T_ADDON_ADO_ADO_ID\" ="
                + m_pathElements[1]
                + " )";
       
        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::SourceParameter>> queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::SourceParameter>>(queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SourceParameter>> srcParamPtr = queryRes.resultList();
        if(srcParamPtr.size() > 0)
        {
            responseMsg = "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SourceParameter> >::const_iterator i = srcParamPtr.begin(); i != srcParamPtr.end(); ++i)
            {
                i->modify()->setId(i->id());
                responseMsg += "\t" + i->get()->toJSON();
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

unsigned short AddonResource::getAddonList(string& responseMsg)
{
    unsigned short res = 500;
    unsigned idx = 0;
    try 
    {
        Wt::Dbo::Transaction transaction(m_session);
        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Addon> > addonPtr = m_session.find<Echoes::Dbo::Addon>();
        responseMsg = "[\n";
        for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Addon> >::const_iterator i = addonPtr.begin(); i != addonPtr.end(); ++i)
        {
            i->modify()->setId(i->id());
            responseMsg += "\t" + i->get()->toJSON();
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
        m_statusCode = getAddonList(responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);
            nextElement = getNextElementFromPath();

            if(!nextElement.compare("parameters"))
            {
                m_statusCode = getParameterForAddon(responseMsg);
            }
            else if(!nextElement.compare("search_types"))
            {
                m_statusCode = getSearchTypeForAddon(responseMsg);
            }
            else
            {
                m_statusCode = 400;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            m_statusCode = 400;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }             
    }

    response.setStatus(m_statusCode);
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
            Wt::Dbo::Transaction transaction(m_session);
            Echoes::Dbo::Addon *addon = new Echoes::Dbo::Addon;
            addon->name = name;
            Wt::Dbo::ptr<Echoes::Dbo::Addon> addonPtr = m_session.add<Echoes::Dbo::Addon>(addon);

            if(parameter.size() > 0)
            {
                for (Wt::Json::Array::const_iterator idx1 = parameter.begin() ; idx1 < parameter.end(); idx1++)
                {
                    Wt::Json::Object tmp = *idx1;
                    Wt::WString namePar = tmp.get("name");
                    Wt::WString formatPar = tmp.get("format");

                    //verif si le param exit déjà en base sinon on l'ajoute
                    Wt::Dbo::ptr<Echoes::Dbo::SourceParameter> srcParamPtr = m_session.find<Echoes::Dbo::SourceParameter>()
                            .where("\"SRP_NAME\" = ?").bind(namePar)
                            .where("\"SRP_FORMAT\" = ?").bind(formatPar);
                    if (!srcParamPtr)
                    {
                        Echoes::Dbo::SourceParameter *sourceParameter = new Echoes::Dbo::SourceParameter;
                        sourceParameter->name = namePar;
                        sourceParameter->format = formatPar;
                        Wt::Dbo::ptr<Echoes::Dbo::SourceParameter> srcParamPtr1 = m_session.add<Echoes::Dbo::SourceParameter>(sourceParameter);
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
                    Wt::Dbo::ptr<Echoes::Dbo::SearchType> seaTypePtr = m_session.find<Echoes::Dbo::SearchType>().where("\"STY_ID\" = ?").bind(tmp2);
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
                responseMsg = addonPtr->toJSON();
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

void AddonResource::processPostRequest(Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";

    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        m_statusCode = postAddon(responseMsg, m_requestData);
    }
    else
    {        
        m_statusCode = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return ;
}

void AddonResource::processPutRequest(Wt::Http::Response &response)
{
    return;
}

void AddonResource::processPatchRequest(Wt::Http::Response &response)
{
    return;
}

unsigned short AddonResource::deleteAddon(string& responseMsg)
{
    unsigned short res = 500;
    try 
    {  
        Wt::Dbo::Transaction transaction(m_session);
        Wt::Dbo::ptr<Echoes::Dbo::Addon> addonPtr = m_session.find<Echoes::Dbo::Addon>().where("\"ADO_ID\" = ?").bind(m_pathElements[1]);                                                             
        //verif si l'addon existe
        if(addonPtr)
        {
            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Source>> srcCollec = m_session.find<Echoes::Dbo::Source>().where("\"SRC_ADO_ADO_ID\" = ?").bind(m_pathElements[1]);
            //verif si l'addon n'est pas utilisée
            if(srcCollec.size() == 0)
            {
                //supprime l'addon
                
                addonPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
                res = 204; 
            }
            else
            {
                res = 409;
                responseMsg = "{\"message\":\"Conflict, a source use this addon\"}";
            }
        }
        else
        {
            res = 404;
            responseMsg = "{\"message\":\"Addon not found\"}";
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

void AddonResource::processDeleteRequest( Wt::Http::Response &response)
{    
    string responseMsg = "", nextElement = "", sRequest = "";

    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        m_statusCode = 400;
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
                m_statusCode = deleteAddon(responseMsg);
            }
            else
            {
                m_statusCode = 400;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            m_statusCode = 400;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}


void AddonResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);

    return;
}

