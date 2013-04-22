/* 
 * API CriteriaResource
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

#include "CriteriaResource.h"

using namespace std;

CriteriaResource::CriteriaResource() : PublicApiResource::PublicApiResource()
{
}

CriteriaResource::CriteriaResource(const CriteriaResource& orig) : PublicApiResource::PublicApiResource(orig)
{
}

CriteriaResource::~CriteriaResource()
{
}

unsigned short CriteriaResource::getCriterias(string &responseMsg) const
{
    unsigned short res = 500;
    unsigned idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(*session);
        Wt::Dbo::collection<Wt::Dbo::ptr<AlertCriteria> > alertCriterias = session->find<AlertCriteria>();
        responseMsg = "[\n";
        for (Wt::Dbo::collection<Wt::Dbo::ptr<AlertCriteria> >::const_iterator i = alertCriterias.begin(); i != alertCriterias.end(); ++i)
        {
            i->modify()->setId(i->id());
            responseMsg += "\t" + i->modify()->toJSON();
            ++idx;
            if(alertCriterias.size()-idx > 0)
            {
                responseMsg += ",\n";
            }
        }
        responseMsg += "\n]\n";
        res = 200;
        transaction.commit();
    }
    catch (Wt::Dbo::Exception e)
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;
}

void CriteriaResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";
    
    nextElement = getNextElementFromPath();

    if(!nextElement.compare(""))
    {
        this->statusCode = getCriterias(responseMsg);
    }
    else
    {
        this->statusCode = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }

    response.setStatus(this->statusCode);
    response.out() << responseMsg;
    return;
}


void CriteriaResource::processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{   
    return ;
}


void CriteriaResource::processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}


void CriteriaResource::processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}



void CriteriaResource::processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{    
    return;
}


void CriteriaResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);

    return;
}

