/* 
 * API CriterionResource
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

#include "CriterionResource.h"

using namespace std;

CriterionResource::CriterionResource() : PublicApiResource::PublicApiResource()
{
}

CriterionResource::~CriterionResource()
{
}

unsigned short CriterionResource::getCriteria(string &responseMsg)
{
    unsigned short res = 500;
    unsigned idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);
        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertCriteria> > alertCriterias = m_session.find<Echoes::Dbo::AlertCriteria>();
        responseMsg = "[\n";
        for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertCriteria> >::const_iterator i = alertCriterias.begin(); i != alertCriterias.end(); ++i)
        {
            i->modify()->setId(i->id());
            responseMsg += "\t" + i->get()->toJSON();
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

void CriterionResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";
    
    nextElement = getNextElementFromPath();

    if(!nextElement.compare(""))
    {
        this->m_statusCode = getCriteria(responseMsg);
    }
    else
    {
        this->m_statusCode = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }

    response.setStatus(this->m_statusCode);
    response.out() << responseMsg;
    return;
}


void CriterionResource::processPostRequest(Wt::Http::Response &response)
{   
    return ;
}


void CriterionResource::processPutRequest(Wt::Http::Response &response)
{
    return;
}


void CriterionResource::processPatchRequest(Wt::Http::Response &response)
{
    return;
}



void CriterionResource::processDeleteRequest(Wt::Http::Response &response)
{    
    return;
}


void CriterionResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);

    return;
}

