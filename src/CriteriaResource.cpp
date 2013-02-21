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

CriteriaResource::CriteriaResource(){
}

string CriteriaResource::getCriterias()
{
    string res = "";
    try
    {
        Wt::Dbo::Transaction transaction(*session);
        Wt::Dbo::collection<Wt::Dbo::ptr<AlertCriteria> > alertCriterias = session->find<AlertCriteria>();
        res = "[\n";
        for (Wt::Dbo::collection<Wt::Dbo::ptr<AlertCriteria> >::const_iterator i = alertCriterias.begin(); i != alertCriterias.end(); ++i)
        {
            res += "\t" + i->modify()->toJSON();
            /*
            res += "{\n\"";
            res += "  \"id\" : \"" + boost::lexical_cast<std::string > (i->id()) + "\"\n\"";
            res += "  \"name\" : \"" + boost::lexical_cast<std::string > (i->get()->name) + "\"\n\"";
            res += "}\n";*/
        }
        res += "]\n";
        this->statusCode = 200;
        transaction.commit();
    }
    catch (Wt::Dbo::Exception e)
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}

void CriteriaResource::processGetRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";
    
    nextElement = getNextElementFromPath();

    if(!nextElement.compare(""))
    {
        responseMsg = getCriterias();
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

CriteriaResource::~CriteriaResource()
{
    beingDeleted();
}

