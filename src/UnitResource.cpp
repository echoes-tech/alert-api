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

EReturnCode UnitResource::getTypeOfUnits(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
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
            res = EReturnCode::OK;
        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = "{\"message\":\"UnitType not found\"}";
        }

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const &e)
    {
        Wt::log("error") << e.what();
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;
}

EReturnCode UnitResource::getListUnits(string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
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
            res = EReturnCode::OK;
        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = "{\"message\":\"Unit not found\"}";
        }

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const &e)
    {
        Wt::log("error") << e.what();
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;
}

EReturnCode UnitResource::getUnit(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);
      
        Wt::Dbo::ptr<Echoes::Dbo::InformationUnit> informationUnit = m_session.find<Echoes::Dbo::InformationUnit>()
                .where("\"INU_ID\" = ?").bind(this->m_pathElements[1])
                .where("\"INU_DELETE\" IS NULL");

        if (!informationUnit)
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = "{\"message\":\"Unit not found\"}";
        }
        else
        {
            informationUnit.modify()->setId(informationUnit.id());
            responseMsg = informationUnit->toJSON();
            res = EReturnCode::OK;
        }

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const &e)
    {
        Wt::log("error") << e.what();
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;
}

EReturnCode UnitResource::getSubUnitsForUnit(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    unsigned idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);
      
        Wt::Dbo::ptr<Echoes::Dbo::InformationUnit> informationUnit = m_session.find<Echoes::Dbo::InformationUnit>()
                .where("\"INU_ID\" = ?").bind(this->m_pathElements[1])
                .where("\"INU_DELETE\" IS NULL");

        if (!informationUnit)
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = "{\"message\":\"Unit not found\"}";
        }
        else
        {
            //FIXME
//             Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationSubUnit> > infoSubUnit = m_session.find<Echoes::Dbo::InformationSubUnit>()
//                    .where("\"ISU_INU_INU_ID\" = ?").bind(this->m_pathElements[1])
//                    .where("\"ISU_DELETE\" IS NULL");
//
//            if(infoSubUnit.size() > 0)
            {
                responseMsg = "[\n";
                //FIXME
//                for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationSubUnit> >::const_iterator i = infoSubUnit.begin(); i != infoSubUnit.end(); i++)
//                {
//                    i->modify()->setId(i->id());
//                    responseMsg += i->get()->toJSON();
//                    ++idx;
//                    if(infoSubUnit.size()-idx > 0)
//                    {
//                        responseMsg += ",\n";
//                    }
//                }  
//                responseMsg += "\n]\n";
                res = EReturnCode::OK;
            }
//            else 
//            {
//                res = EReturnCode::NOT_FOUND;
//                responseMsg = "{\"message\":\"Subunit not found\"}";
//            }
        }
        transaction.commit();
    }
    catch (Wt::Dbo::Exception const &e)
    {
        Wt::log("error") << e.what();
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;
}

 EReturnCode UnitResource::getTypeForUnit(std::string &responseMsg)
 {
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
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

            res = EReturnCode::OK;
        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = "{\"message\":\"Unit not found\"}";
        }

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const &e)
    {
        Wt::log("error") << e.what();
        res = EReturnCode::SERVICE_UNAVAILABLE;
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
    response.setStatus(this->m_statusCode);
    response.out() << responseMsg;
    return;
}

EReturnCode UnitResource::postUnit(string& responseMsg, const string& sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
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
        res = EReturnCode::BAD_REQUEST;
        responseMsg = "{\"message\":\"Problems parsing JSON\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON:" << sRequest;
        return res;
    }
    catch (Wt::Json::TypeException const& e)
    {
        res = EReturnCode::BAD_REQUEST;
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
            res = EReturnCode::OK;
        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = "{\"message\":\"UnitType not found\"}";
        }
        transaction.commit();
        
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;
}

void UnitResource::processPostRequest(Wt::Http::Response &response)
{   
    string responseMsg = "", nextElement = "";

    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->m_statusCode = postUnit(responseMsg, m_requestData);
    }
    else
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }

    response.setStatus(this->m_statusCode);
    response.out() << responseMsg;
    return ;
}


void UnitResource::processPutRequest(Wt::Http::Response &response)
{
    return;
}


void UnitResource::processPatchRequest(Wt::Http::Response &response)
{
    return;
}

EReturnCode UnitResource::deleteUnit(string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
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
                res = EReturnCode::NO_CONTENT;
                transaction.commit();   
            }
            else
            {
                res = EReturnCode::CONFLICT;
                responseMsg = "{\"message\":\"Conflict, an information use this unit\"}";
            } 
        }
        else
        {
            responseMsg = "{\"message\":\"Unit Not Found\"}";
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

void UnitResource::processDeleteRequest(Wt::Http::Response &response)
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
                m_statusCode = deleteUnit(responseMsg);
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

