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

UnitResource::UnitResource(){
}

std::string UnitResource::getUnit()
{
    std::string res = "";
    try
    {
        Wt::Dbo::Transaction transaction(*this->session);
      
        Wt::Dbo::ptr<InformationUnit> informationUnit = session->find<InformationUnit>()
                                                          .where("\"INU_ID\" = ?").bind(this->vPathElements[1]);
        if (!informationUnit)
        {
            this->statusCode = 404;
            res = "{\"message\":\"Unit not found\"}";
            return res;
        }
        else
        {
            informationUnit.modify()->setId(informationUnit.id());
            res = informationUnit.modify()->toJSON();
            this->statusCode = 200;
        }

        transaction.commit();
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

std::string UnitResource::getSubUnitsForUnit()
{
    std::string res = "";
    int idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(*this->session);
      
        Wt::Dbo::ptr<InformationUnit> informationUnit = session->find<InformationUnit>()
                                                          .where("\"INU_ID\" = ?").bind(this->vPathElements[1]);
        if (!informationUnit)
        {
            this->statusCode = 404;
            res = "{\"message\":\"Unit not found\"}";
            return res;
        }
        else
        {
             Wt::Dbo::collection<Wt::Dbo::ptr<InformationSubUnit> > infoSubUnit = session->find<InformationSubUnit>()
                                                                             .where("\"ISU_INU_INU_ID\" = ?").bind(this->vPathElements[1]);
            if(infoSubUnit.size() > 0)
            {
                if(infoSubUnit.size() > 1)
                {
                res += "[\n";
                }
                for (Wt::Dbo::collection<Wt::Dbo::ptr<InformationSubUnit> >::const_iterator i = infoSubUnit.begin(); i != infoSubUnit.end(); i++)
                {
                    i->modify()->setId(i->id());
                    res += i->modify()->toJSON();
                    ++idx;
                    if(infoSubUnit.size()-idx > 0)
                    {
                        res.replace(res.size()-1, 1, "");
                        res += ",\n";
                    }
                }  
                if(infoSubUnit.size() > 1)
                {
                res += "]\n";
                }
            }
            else 
            {
                this->statusCode = 404;
                res = "{\"message\":\"Subunit not found\"}";
                return res;
            }
            this->statusCode = 200;
        }

        transaction.commit();
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

void UnitResource::processGetRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    std::string responseMsg = "", nextElement = "";
    
    try
    {
        nextElement = getNextElementFromPath();
        boost::lexical_cast<unsigned int>(nextElement);

        nextElement = getNextElementFromPath();
        if(!nextElement.compare(""))
        {
            responseMsg = getUnit();
        }
        else if(!nextElement.compare("subunits"))
        {
            responseMsg = getSubUnitsForUnit();
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
    response.setStatus(this->statusCode);
    response.out() << responseMsg;
    return;
}


void UnitResource::processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{   
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



void UnitResource::processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{    
    return;
}


void UnitResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);

    return;
}

UnitResource::~UnitResource()
{
    beingDeleted();
}
