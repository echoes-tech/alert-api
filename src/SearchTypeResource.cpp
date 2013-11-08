/* 
 * API InformationResource
 * @author ECHOES Technologies (GDR)
 * @date 14/03/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "SearchTypeResource.h"

using namespace std;

SearchTypeResource::SearchTypeResource() : PublicApiResource::PublicApiResource()
{
}

SearchTypeResource::~SearchTypeResource()
{
}

unsigned short SearchTypeResource::getSearchTypeList(string& responseMsg)
{
    unsigned short res = 500;
    unsigned idx = 0;
    try 
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SearchType> > seaTypePtr = m_session.find<Echoes::Dbo::SearchType>()
                .where("\"STY_DELETE\" IS NULL")                                                        
                .orderBy("\"STY_ID\"");
        
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
            responseMsg += "\n]\n";
            res = 200;
        }
        else
        {        
            res = 404;
            responseMsg = "{\"message\":\"Search type not found\"}";
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

unsigned short SearchTypeResource::getParameterForSearchType(string &responseMsg)
{
    unsigned short res = 500;
    unsigned idx = 0;
    try 
    {
        Wt::Dbo::Transaction transaction(m_session);
        
        string queryStr = "SELECT sep FROM \"T_SEARCH_PARAMETER_SEP\" sep "
                " WHERE \"SEP_ID\" IN "
                "("
                "SELECT \"T_SEARCH_PARAMETER_SEP_SEP_ID\" FROM \"TJ_STY_SEP\" WHERE \"T_SEARCH_TYPE_STY_STY_ID\" IN"
                " (SELECT \"STY_ID\" FROM \"T_SEARCH_TYPE_STY\" "
                " WHERE \"STY_ID\" = "+ boost::lexical_cast<string > (this->m_pathElements[1])+ 
                " AND \"STY_DELETE\" IS NULL "
                " )"
                " ) "
                "ORDER BY \"SEP_ID\"";
       
        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::SearchParameter> > queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::SearchParameter> >(queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SearchParameter> > seaParamPtr = queryRes.resultList();
        
        if(seaParamPtr.size() != 0)
        {
            responseMsg = "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SearchParameter> >::const_iterator i = seaParamPtr.begin(); i != seaParamPtr.end(); ++i)
            {
                i->modify()->setId(i->id());
                responseMsg += "\t" + i->get()->toJSON();
                ++idx;
                if(seaParamPtr.size()-idx > 0)
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

void SearchTypeResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";

    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->m_statusCode = getSearchTypeList(responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();

            if(!nextElement.compare("parameters"))
            {
                nextElement = getNextElementFromPath();
                if(!nextElement.compare(""))
                {
                    this->m_statusCode = getParameterForSearchType(responseMsg);
                }
                else
                {
                    this->m_statusCode = 400;
                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                }
            }
            else
            {
                this->m_statusCode = 400;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            this->m_statusCode = 400;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(this->m_statusCode);
    response.out() << responseMsg;
    return;
}

unsigned short SearchTypeResource::postSearchType(string &responseMsg, const string &sRequest)
{
    unsigned short res = 500;
    Wt::WString name;
    Wt::Json::Array& parameter = Wt::Json::Array::Empty;
    Wt::Json::Array& addonsId = Wt::Json::Array::Empty;
    try
    {
        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);
        //descriptif
        name = result.get("name");
        
        try
        {
            Wt::Dbo::Transaction transaction(m_session);
            Echoes::Dbo::SearchType *searchType = new Echoes::Dbo::SearchType;
            searchType->name = name;
            Wt::Dbo::ptr<Echoes::Dbo::SearchType> seaTypePtr = m_session.add<Echoes::Dbo::SearchType>(searchType);
            seaTypePtr.flush();
            addonsId = result.get("addons");
            if(addonsId.size() != 0 )
            {
                for (Wt::Json::Array::const_iterator idx2 = addonsId.begin(); idx2 < addonsId.end(); idx2++)
                {
                    Wt::WString tmp2 = idx2->toString();
                    Wt::Dbo::ptr<Echoes::Dbo::Addon> addonPtr = m_session.find<Echoes::Dbo::Addon>().where("\"ADO_ID\" = ?").bind(tmp2);
                    if (addonPtr)
                    {
                        seaTypePtr.modify()->addons.insert(addonPtr);
                    }
                    else
                    {
                        res = 404;
                        responseMsg = "{\"message\":\"Addon not found\"}";
                        return res;
                    }
                }
            }
             else
            {
                res = 400;
                responseMsg = "{\n\t\"message\":\"Bad Request.\"\n}";
                return res;
            }
            parameter = result.get("parameters");
            if(parameter.size() != 0)
            {
                for (Wt::Json::Array::const_iterator idx1 = parameter.begin() ; idx1 < parameter.end(); idx1++)
                {
                    Wt::Json::Object tmp = *idx1;
                    Wt::WString namePar = tmp.get("name");
                    Wt::WString formatPar = tmp.get("format");

                    //verif si le param exit déjà en base sinon on l'ajoute
                    Wt::Dbo::ptr<Echoes::Dbo::SearchParameter> seaParamPtr = m_session.find<Echoes::Dbo::SearchParameter>()
                            .where("\"SEP_NAME\" = ?").bind(namePar)
                            .where("\"SEP_FORMAT\" = ?").bind(formatPar);
                    if (!seaParamPtr)
                    {
                        Echoes::Dbo::SearchParameter *searchParameter = new Echoes::Dbo::SearchParameter;
                        searchParameter->name = namePar;
                        searchParameter->format = formatPar;
                        Wt::Dbo::ptr<Echoes::Dbo::SearchParameter> seaParamPtr1 = m_session.add<Echoes::Dbo::SearchParameter>(searchParameter);
                        seaTypePtr.modify()->searchParameters.insert(seaParamPtr1);
                    }
                    else
                    {
                        seaTypePtr.modify()->searchParameters.insert(seaParamPtr);
                    }
                }           
            }
            else
            {
                res = 400;
                responseMsg = "{\n\t\"message\":\"Bad Request.\"\n}";
                return res;
            }
            
            seaTypePtr.modify()->setId(seaTypePtr.id());
            responseMsg = seaTypePtr->toJSON();
            transaction.commit();
            res = 200;
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

void SearchTypeResource::processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{   
    string responseMsg = "", nextElement = "", sRequest = "";

    sRequest = request2string(request);
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->m_statusCode = postSearchType(responseMsg, sRequest);
    }
    else
    {  
        this->m_statusCode = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }
    response.setStatus(this->m_statusCode);
    response.out() << responseMsg;
    return ;
}


void SearchTypeResource::processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}


void SearchTypeResource::processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}

unsigned short SearchTypeResource::deleteSearchType(string &responseMsg)
{
    unsigned short res = 500;
    try 
    {  
        Wt::Dbo::Transaction transaction(m_session);
        Wt::Dbo::ptr<Echoes::Dbo::SearchType> seaTypePtr = m_session.find<Echoes::Dbo::SearchType>().where("\"STY_ID\" = ?").bind(this->m_pathElements[1]);   
        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Search> > seaPtr = m_session.find<Echoes::Dbo::Search>().where("\"SEA_STY_STY_ID\" = ?").bind(this->m_pathElements[1]);
        //verif si le search type existe
        if(seaTypePtr)
        {
            //verif si le search type n'est pas utilisé par une search
            if(seaPtr.size() == 0)
            {
                //supprime le search type
                
                seaTypePtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
                responseMsg = "";
                res = 204;
            }
            else
            {
                res = 409;
                responseMsg = "{\"message\":\"Conflict, an search use this search type\"}";
            }
        }
        else
        {
            res = 404;
            responseMsg = "{\"message\":\"Search type not found\"}";
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

void SearchTypeResource::processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{    
 string responseMsg = "", nextElement = "", sRequest = "";

    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->m_statusCode = 400;
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
                this->m_statusCode = deleteSearchType(responseMsg);
            }
            else
            {
                this->m_statusCode = 400;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            this->m_statusCode = 400;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(this->m_statusCode);
    response.out() << responseMsg;
    return;
}


void SearchTypeResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);

    return;
}

