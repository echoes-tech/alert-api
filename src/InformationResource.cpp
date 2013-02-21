/* 
 * API InformationResource
 * @author ECHOES Technologies (GDR)
 * @date 11/02/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */


#include "InformationResource.h"

using namespace std;

InformationResource::InformationResource(){
}

void InformationResource::processGetRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}


void InformationResource::processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{   
    return ;
}


void InformationResource::processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}


void InformationResource::processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}



void InformationResource::processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{    
    return;
}


void InformationResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);

    return;
}

InformationResource::~InformationResource()
{
    beingDeleted();
}