/* 
 * API UnitResource
 * @author ECHOES Technologies (GDR)
 * @date 21/02/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "UnitResource.h"

using namespace std;

UnitResource::UnitResource() : PublicApiResource::PublicApiResource()
{
}

UnitResource::~UnitResource()
{
}

unsigned short UnitResource::getTypeOfUnits(string &responseMsg)
{
    unsigned short res = 500;
    unsigned idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);
      
        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationUnitType>> unitTypePtr = m_session.find<Echoes::Dbo::InformationUnitType>().where("\"IUT_DELETE\" IS NULL");
                                                         
        if (unitTypePtr.size() > 0)
        {
            
            responseMsg = "[\n";
            
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationUnitType> >::const_iterator i = unitTypePtr.begin(); i != unitTypePtr.end(); ++i)
            {
                i->modify()->setId(i->id());                
                responseMsg += i->get()->toJSON();
                 ++idx;
                if(unitTypePtr.size()-idx > 0)
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
            responseMsg = "{\"message\":\"UnitType not found\"}";
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

unsigned short UnitResource::getListUnits(string& responseMsg)
{
    unsigned short res = 500;
    int idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);
      
        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationUnit>> unitCollec = m_session.find<Echoes::Dbo::InformationUnit>()
                .where("\"INU_DELETE\" IS NULL")
                .orderBy("\"INU_ID\"");

        if (unitCollec.size() > 0)
        {
            
            responseMsg = "[\n";
            
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationUnit> >::const_iterator i = unitCollec.begin(); i != unitCollec.end(); ++i)
            {
                i->modify()->setId(i->id());                
                responseMsg += i->get()->toJSON();
                 ++idx;
                if(unitCollec.size()-idx > 0)
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
            responseMsg = "{\"message\":\"Unit not found\"}";
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

unsigned short UnitResource::getUnit(string &responseMsg)
{
    unsigned short res = 500;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);
      
        Wt::Dbo::ptr<Echoes::Dbo::InformationUnit> informationUnit = m_session.find<Echoes::Dbo::InformationUnit>()
                .where("\"INU_ID\" = ?").bind(this->m_pathElements[1])
                .where("\"INU_DELETE\" IS NULL");

        if (!informationUnit)
        {
            res = 404;
            responseMsg = "{\"message\":\"Unit not found\"}";
        }
        else
        {
            informationUnit.modify()->setId(informationUnit.id());
            responseMsg = informationUnit->toJSON();
            res = 200;
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

unsigned short UnitResource::getSubUnitsForUnit(string &responseMsg)
{
    unsigned short res = 500;
    unsigned idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);
      
        Wt::Dbo::ptr<Echoes::Dbo::InformationUnit> informationUnit = m_session.find<Echoes::Dbo::InformationUnit>()
                .where("\"INU_ID\" = ?").bind(this->m_pathElements[1])
                .where("\"INU_DELETE\" IS NULL");

        if (!informationUnit)
        {
            res = 404;
            responseMsg = "{\"message\":\"Unit not found\"}";
        }
        else
        {
             Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationSubUnit> > infoSubUnit = m_session.find<Echoes::Dbo::InformationSubUnit>()
                    .where("\"ISU_INU_INU_ID\" = ?").bind(this->m_pathElements[1])
                    .where("\"ISU_DELETE\" IS NULL");

            if(infoSubUnit.size() > 0)
            {
                responseMsg = "[\n";
                for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationSubUnit> >::const_iterator i = infoSubUnit.begin(); i != infoSubUnit.end(); i++)
                {
                    i->modify()->setId(i->id());
                    responseMsg += i->get()->toJSON();
                    ++idx;
                    if(infoSubUnit.size()-idx > 0)
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
                responseMsg = "{\"message\":\"Subunit not found\"}";
            }
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

 unsigned short UnitResource::getTypeForUnit(std::string &responseMsg)
 {
    unsigned short res = 500;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);
      
         Wt::Dbo::ptr<Echoes::Dbo::InformationUnit> informationUnit = m_session.find<Echoes::Dbo::InformationUnit>()
                .where("\"INU_ID\" = ?").bind(this->m_pathElements[1])
                .where("\"INU_DELETE\" IS NULL");

                                                         
        if (informationUnit)
        {
//            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationUnitType>> unitTypePtr = _session.find<InformationUnitType>().where("\"IUT_DELETE\" IS NULL")
//                                                                                                                     .where("\"IUT_ID\" = ?").bind(informationUnit.get()->unitType.id());

                informationUnit.get()->unitType.modify()->setId(informationUnit.get()->unitType.id());                
                responseMsg += informationUnit.get()->unitType.modify()->toJSON();

            res = 200;
        }
        else
        {
            res = 404;
            responseMsg = "{\"message\":\"Unit not found\"}";
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

void UnitResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";
    nextElement = getNextElementFromPath();
    
    if(!nextElement.compare(""))
    {
        this->m_statusCode = getListUnits(responseMsg);
    }
    else if(!nextElement.compare("types"))
    {
        this->m_statusCode = getTypeOfUnits(responseMsg);
    }
    else
    {
        try
        {
            
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();
            if(!nextElement.compare(""))
            {
                this->m_statusCode = getUnit(responseMsg);
            }
            else if(!nextElement.compare("subunits"))
            {
                this->m_statusCode = getSubUnitsForUnit(responseMsg);
            }
            else if(!nextElement.compare("types"))
            {
                this->m_statusCode = getTypeForUnit(responseMsg);
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

unsigned short UnitResource::postUnit(string& responseMsg, const string& sRequest)
{
    unsigned short res = 500;
    Wt::WString name;
    int typeId;
      
    try
    {
        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);
        //descriptif
        name = result.get("inu_name");
        typeId = result.get("iut_id");
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
        Wt::Dbo::Transaction transaction(m_session);
        Wt::Dbo::ptr<Echoes::Dbo::InformationUnitType> unitTypePtr = m_session.find<Echoes::Dbo::InformationUnitType>().where("\"IUT_ID\" = ?").bind(typeId);
        Wt::Dbo::ptr<Echoes::Dbo::InformationUnit> unitPtr;
        
        if(unitTypePtr)
        {        
            Echoes::Dbo::InformationUnit *informationUnit = new Echoes::Dbo::InformationUnit;
            informationUnit->name = name;
            informationUnit->unitType = unitTypePtr;
            unitPtr = m_session.add<Echoes::Dbo::InformationUnit>(informationUnit);
            
            unitPtr.flush();
            unitPtr.modify()->setId(unitPtr.id());
            responseMsg = unitPtr->toJSON();
            res = 200;
        }
        else
        {
            res = 404;
            responseMsg = "{\"message\":\"UnitType not found\"}";
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

void UnitResource::processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{   
    string responseMsg = "", nextElement = "", sRequest = "";

    sRequest = request2string(request);
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->m_statusCode = postUnit(responseMsg, sRequest);
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


void UnitResource::processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}


void UnitResource::processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}

unsigned short UnitResource::deleteUnit(string& responseMsg)
{
    unsigned short res = 500;
    try 
    {  
        Wt::Dbo::Transaction transaction(m_session);
           
        Wt::Dbo::ptr<Echoes::Dbo::InformationUnit> informationUnitPtr = m_session.find<Echoes::Dbo::InformationUnit>().where("\"INU_ID\" = ?").bind(this->m_pathElements[1]);
        //Unit exist ?
        if(informationUnitPtr)
        {
            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SearchUnit>> seaUnitCollec = m_session.find<Echoes::Dbo::SearchUnit>().where("\"SEU_INU_INU_ID\" = ?").bind(this->m_pathElements[1]);
            //verif si l'unité n'est pas utilisée                                                                
            if (seaUnitCollec.size() == 0)
            {                
                //supprime l'info
                informationUnitPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
                res = 204;
                transaction.commit();   
            }
            else
            {
                res = 409;
                responseMsg = "{\"message\":\"Conflict, an information use this unit\"}";
            } 
        }
        else
        {
            responseMsg = "{\"message\":\"Unit Not Found\"}";
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

void UnitResource::processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
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
                this->m_statusCode = deleteUnit(responseMsg);
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


void UnitResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);

    return;
}

