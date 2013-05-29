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

UnitResource::UnitResource(const UnitResource& orig) : PublicApiResource::PublicApiResource(orig)
{
}

UnitResource::~UnitResource()
{
}

unsigned short UnitResource::getTypeOfUnit(string &responseMsg)
{
    unsigned short res = 500;
    unsigned idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(_session);
      
        Wt::Dbo::collection<Wt::Dbo::ptr<InformationUnitType>> unitTypePtr = _session.find<InformationUnitType>().where("\"IUT_DELETE\" IS NULL");
                                                         
        if (unitTypePtr.size() > 0)
        {
            
            responseMsg = "[\n";
            
            for (Wt::Dbo::collection<Wt::Dbo::ptr<InformationUnitType> >::const_iterator i = unitTypePtr.begin(); i != unitTypePtr.end(); ++i)
            {
                i->modify()->setId(i->id());                
                responseMsg += i->modify()->toJSON();
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
        Wt::Dbo::Transaction transaction(_session);
      
        Wt::Dbo::collection<Wt::Dbo::ptr<InformationUnit>> unitCollec = _session.find<InformationUnit>()
                .where("\"INU_DELETE\" IS NULL")
                .orderBy("\"INU_ID\"");

        if (unitCollec.size() > 0)
        {
            
            responseMsg = "[\n";
            
            for (Wt::Dbo::collection<Wt::Dbo::ptr<InformationUnit> >::const_iterator i = unitCollec.begin(); i != unitCollec.end(); ++i)
            {
                i->modify()->setId(i->id());                
                responseMsg += i->modify()->toJSON();
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
        Wt::Dbo::Transaction transaction(_session);
      
        Wt::Dbo::ptr<InformationUnit> informationUnit = _session.find<InformationUnit>()
                .where("\"INU_ID\" = ?").bind(this->vPathElements[1])
                .where("\"INU_DELETE\" IS NULL");

        if (!informationUnit)
        {
            res = 404;
            responseMsg = "{\"message\":\"Unit not found\"}";
        }
        else
        {
            informationUnit.modify()->setId(informationUnit.id());
            responseMsg = informationUnit.modify()->toJSON();
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
        Wt::Dbo::Transaction transaction(_session);
      
        Wt::Dbo::ptr<InformationUnit> informationUnit = _session.find<InformationUnit>()
                .where("\"INU_ID\" = ?").bind(this->vPathElements[1])
                .where("\"INU_DELETE\" IS NULL");

        if (!informationUnit)
        {
            res = 404;
            responseMsg = "{\"message\":\"Unit not found\"}";
        }
        else
        {
             Wt::Dbo::collection<Wt::Dbo::ptr<InformationSubUnit> > infoSubUnit = _session.find<InformationSubUnit>()
                    .where("\"ISU_INU_INU_ID\" = ?").bind(this->vPathElements[1])
                    .where("\"ISU_DELETE\" IS NULL");

            if(infoSubUnit.size() > 0)
            {
                responseMsg = "[\n";
                for (Wt::Dbo::collection<Wt::Dbo::ptr<InformationSubUnit> >::const_iterator i = infoSubUnit.begin(); i != infoSubUnit.end(); i++)
                {
                    i->modify()->setId(i->id());
                    responseMsg += i->modify()->toJSON();
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

void UnitResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";
    nextElement = getNextElementFromPath();
    
    if(!nextElement.compare(""))
    {
        this->statusCode = getListUnits(responseMsg);
    }
    else if(!nextElement.compare("types"))
    {
        this->statusCode = getTypeOfUnit(responseMsg);
    }
    else
    {
        try
        {
            
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();
            if(!nextElement.compare(""))
            {
                this->statusCode = getUnit(responseMsg);
            }
            else if(!nextElement.compare("subunits"))
            {
                this->statusCode = getSubUnitsForUnit(responseMsg);
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
        Wt::Dbo::Transaction transaction(_session);
        Wt::Dbo::ptr<InformationUnitType> unitTypePtr = _session.find<InformationUnitType>().where("\"IUT_ID\" = ?").bind(typeId);
        Wt::Dbo::ptr<InformationUnit> unitPtr;
        
        if(unitTypePtr)
        {        
            InformationUnit *informationUnit = new InformationUnit;
            informationUnit->name = name;
            informationUnit->unitType = unitTypePtr;
            unitPtr = _session.add<InformationUnit>(informationUnit);
            
            unitPtr.flush();
            unitPtr.modify()->setId(unitPtr.id());
            responseMsg = unitPtr.modify()->toJSON();
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
        this->statusCode = postUnit(responseMsg, sRequest);
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
        Wt::Dbo::Transaction transaction(_session);
           
        Wt::Dbo::ptr<InformationUnit> informationUnitPtr = _session.find<InformationUnit>().where("\"INU_ID\" = ?").bind(this->vPathElements[1]);
        //Unit exist ?
        if(informationUnitPtr)
        {
            Wt::Dbo::collection<Wt::Dbo::ptr<SearchUnit>> seaUnitCollec = _session.find<SearchUnit>().where("\"SEU_INU_INU_ID\" = ?").bind(this->vPathElements[1]);
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
                this->statusCode = deleteUnit(responseMsg);
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


void UnitResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);

    return;
}

