/* 
 * API RoleResource
 * @author ECHOES Technologies (TSA)
 * @date 08/06/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "RoleResource.h"

using namespace std;

RoleResource::RoleResource() : PublicApiResource::PublicApiResource()
{
}

RoleResource::~RoleResource()
{
}

EReturnCode RoleResource::getRoleForUser(std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {        
        Wt::Dbo::Transaction transaction(m_session);
        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::UserRole>> userRoles = m_session.find<Echoes::Dbo::UserRole>().where("\"URO_ORG_ORG_ID\" = ?").bind(this->m_session.user()->organization);
        if (userRoles.size() > 0)
        {
            unsigned int idx = 0;
            responseMsg += "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::UserRole> >::const_iterator uro = userRoles.begin() ; uro != userRoles.end(); uro++)
            {
                idx++;
                uro->modify()->setId(uro->id());
                responseMsg += uro->modify()->toJSON();
                if (idx < userRoles.size())
                {
                    responseMsg += ",";
                }
                responseMsg += "\n";
            }
            responseMsg += "]\n";
        }
        else
        {
            responseMsg = "{\"message\":\"Role not found\"}";
            res = EReturnCode::NOT_FOUND;
        }
        res = EReturnCode::OK;
        transaction.commit();
    }
    catch (Wt::Dbo::Exception e)
    {
        Wt::log("error") << e.what();
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}

void RoleResource::processGetRequest(Wt::Http::Response &response)
{
    std::string responseMsg = "", nextElement = "";
    
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->m_statusCode = getRoleForUser(responseMsg);
    }
    else
    {
        this->m_statusCode = EReturnCode::BAD_REQUEST;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }

    response.setStatus(this->m_statusCode);
    response.out() << responseMsg;
    return;
}

void RoleResource::processPostRequest(Wt::Http::Response &response)
{
    return;
}


void RoleResource::processPutRequest(Wt::Http::Response &response)
{
    return;
}


void RoleResource::processPatchRequest(Wt::Http::Response &response)
{
    return;
}



void RoleResource::processDeleteRequest(Wt::Http::Response &response)
{
    return;
}


void RoleResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);

    return;
}

