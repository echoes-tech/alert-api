/* 
 * API UserResource
 * @author ECHOES Technologies (GDR)
 * @date 05/02/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "tools/JsonSerializer.h"
#include "UserResource.h"

using namespace std;

UserResource::UserResource() : PublicApiResource::PublicApiResource()
{
}

UserResource::~UserResource()
{
}

unsigned short UserResource::getInformationForUser(std::string &responseMsg)
{
    unsigned short res = Echoes::Dbo::EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {        
        Wt::Dbo::Transaction transaction(m_session);
//        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::User>> user = m_session.find<Echoes::Dbo::User>();
//        responseMsg += this->toJSON(user);
       
        Wt::Dbo::ptr<Echoes::Dbo::User> user = m_session.find<Echoes::Dbo::User>().where("\"USR_ID\" = ?").bind(this->m_session.user().id());
      
        if(user)
        {
//            user.modify()->setId(user.id());
            responseMsg += this->toJSON(user);
//            Wt::Dbo::JsonSerializer jsonSerializer(m_session);
//            jsonSerializer.Serialize(user);
//            responseMsg += jsonSerializer.getResult();
//            Wt::Dbo::XmlSerializer xmlSerializer(std::cout, m_session);
//            xmlSerializer.Serialize(user);

            res = 200;
        }
        else
        {
            responseMsg = "{\"message\":\"User not found\"}";
            res = Echoes::Dbo::EReturnCode::NOT_FOUND;
        }
        transaction.commit();
    }
    catch (Wt::Dbo::Exception e)
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}

void UserResource::processGetRequest(Wt::Http::Response &response)
{
    std::string responseMsg = "", nextElement = "";
    
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->m_statusCode = getInformationForUser(responseMsg);
    }
    else
    {
        this->m_statusCode = Echoes::Dbo::EReturnCode::BAD_REQUEST;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }

    response.setStatus(this->m_statusCode);
    response.out() << responseMsg;
    return;
}

unsigned short UserResource::postActionForUser(std::string &responseMsg, const std::string &sRequest)
{
    unsigned short res = Echoes::Dbo::EReturnCode::INTERNAL_SERVER_ERROR;
    Wt::WString tableObject;
    int actionAfter, actionBefore, actionRelative;
    long long uacId, tableObjectId;
    try
    {
        
        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);

        uacId = result.get("uac_id");
        tableObject = result.get("table_object");
        tableObjectId = result.get("table_object_id");
        actionAfter = result.get("action_after");
        actionBefore = result.get("action_before");
        actionRelative = result.get("action_relative");
    }

    catch (Wt::Json::ParseError const& e)
    {
        res = Echoes::Dbo::EReturnCode::BAD_REQUEST;
        responseMsg = "{\"message\":\"Problems parsing JSON\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON:" << sRequest;
        return res;
    }
    catch (Wt::Json::TypeException const& e)
    {
        res = Echoes::Dbo::EReturnCode::BAD_REQUEST;
        responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON.:" << sRequest;
        return res;
    }
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::ptr<Echoes::Dbo::UserAction> ptrUserAction = m_session.find<Echoes::Dbo::UserAction>().where("\"UAC_ID\" = ?").bind(uacId);

        Echoes::Dbo::UserHistoricalAction *userHistoricalAction = new Echoes::Dbo::UserHistoricalAction();
        userHistoricalAction->tableObject = tableObject;
        userHistoricalAction->tableObjectId = boost::lexical_cast<long long>(tableObjectId);

        Wt::Dbo::ptr<Echoes::Dbo::UserHistoricalAction> ptrUserHistoricalAction = m_session.add<Echoes::Dbo::UserHistoricalAction>(userHistoricalAction);
        ptrUserHistoricalAction.modify()->userAction = ptrUserAction;
        ptrUserHistoricalAction.modify()->user = m_session.user();
        ptrUserHistoricalAction.modify()->dateTime = Wt::WDateTime::currentDateTime();
        ptrUserHistoricalAction.modify()->actionAfter = boost::lexical_cast<int>(actionAfter);
        ptrUserHistoricalAction.modify()->actionBefore = boost::lexical_cast<int>(actionBefore);
        ptrUserHistoricalAction.modify()->actionRelative = boost::lexical_cast<int>(actionRelative);
        
        ptrUserHistoricalAction.flush();
        ptrUserHistoricalAction.modify()->setId(ptrUserHistoricalAction.id());
        responseMsg = ptrUserHistoricalAction->toJSON();    
        res = 200;
        transaction.commit();
    }
    catch (Wt::Dbo::Exception e)
    {
        Wt::log("error") << e.what();
        res = Echoes::Dbo::EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    catch(boost::bad_lexical_cast &)
    {
        res = Echoes::Dbo::EReturnCode::BAD_REQUEST;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }
    return res;
}

unsigned short UserResource::postRoleForUser(std::string &responseMsg, const std::string &sRequest)
{
    unsigned short res = Echoes::Dbo::EReturnCode::INTERNAL_SERVER_ERROR;
    long long uroId;
    try
    {
        
        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);

        uroId = result.get("uro_id");
    }

    catch (Wt::Json::ParseError const& e)
    {
        res = Echoes::Dbo::EReturnCode::BAD_REQUEST;
        responseMsg = "{\"message\":\"Problems parsing JSON\"}";
        Wt::log("warning") << "[User Ressource] Problems parsing JSON:" << sRequest;
        return res;
    }
    catch (Wt::Json::TypeException const& e)
    {
        res = Echoes::Dbo::EReturnCode::BAD_REQUEST;
        responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
        Wt::log("warning") << "[User Ressource] Problems parsing JSON.:" << sRequest;
        return res;
    }
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::ptr<Echoes::Dbo::UserRole> ptrUserRole = m_session.find<Echoes::Dbo::UserRole>().where("\"URO_ID\" = ?").bind(uroId);
        Wt::Dbo::ptr<Echoes::Dbo::Organization> orgPtr = m_session.user()->organization;
        
        if (orgPtr.id() != ptrUserRole->organization.id())
        {
            res = Echoes::Dbo::EReturnCode::FORBIDDEN;
            responseMsg = "{\"message\":\"This role doesn't belong to the current organization of the current user.\"}";
            transaction.commit();
            return res;
        }
        else
        {
            m_session.user().modify()->userRole = ptrUserRole;
            res = Echoes::Dbo::EReturnCode::OK;
            transaction.commit();
        }
    }
    catch (Wt::Dbo::Exception e)
    {
        Wt::log("error") << e.what();
        res = Echoes::Dbo::EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    catch(boost::bad_lexical_cast &)
    {
        res = Echoes::Dbo::EReturnCode::BAD_REQUEST;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }
    return res;
}

void UserResource::processPostRequest(Wt::Http::Response &response)
{
    std::string responseMsg = "", nextElement = "";

    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->m_statusCode = Echoes::Dbo::EReturnCode::BAD_REQUEST;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }
    else
    {   
            if(!nextElement.compare("action"))
            {
                this->m_statusCode = postActionForUser(responseMsg, m_requestData);
            }
            else if (!nextElement.compare("role"))
            {
                this->m_statusCode = postRoleForUser(responseMsg, m_requestData);
            }
            else
            {
                this->m_statusCode = Echoes::Dbo::EReturnCode::BAD_REQUEST;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
    }

    response.setStatus(this->m_statusCode);
    response.out() << responseMsg;
    return;
}


void UserResource::processPutRequest(Wt::Http::Response &response)
{
    return;
}


void UserResource::processPatchRequest(Wt::Http::Response &response)
{
    return;
}



void UserResource::processDeleteRequest(Wt::Http::Response &response)
{
    return;
}


void UserResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);

    return;
}

