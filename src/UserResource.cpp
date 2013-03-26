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


#include "UserResource.h"


UserResource::UserResource() {
}

unsigned short UserResource::getInformationForUser(std::string &responseMsg) const
{
    unsigned short res = 500;
    try
    {        
        Wt::Dbo::Transaction transaction(*this->session);
        Wt::Dbo::ptr<User> user = session->find<User>().where("\"USR_ID\" = ?").bind(this->session->user().id());
      
        if(user)
        {
            user.modify()->setId(user.id());
            responseMsg += user.modify()->toJSON();
            res = 200;
        }
        else
        {
            responseMsg = "{\"message\":\"User not found\"}";
            res = 404;
        }
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
        this->statusCode = getInformationForUser(responseMsg);
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

unsigned short UserResource::postActionForUser(std::string &responseMsg, const std::string &sRequest)
{
    unsigned short res = 500;
    Wt::WString uacId, tableObject, tableObjectId, actionAfter, actionBefore, actionRelative;

    try
    {
        int uacIdInt,  tableObjectIdInt, actionAfterInt, actionBeforeInt, actionRelativeInt;
        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);

        uacIdInt = result.get("uac_id");
        uacId = boost::lexical_cast<std::string>(uacIdInt);
        tableObject = result.get("table_object");
        tableObjectIdInt = result.get("table_object_id");
        tableObjectId = boost::lexical_cast<std::string>(tableObjectIdInt);
        actionAfterInt = result.get("action_after");
        actionAfter = boost::lexical_cast<std::string>(actionAfterInt);
        actionBeforeInt = result.get("action_before");
        actionBefore = boost::lexical_cast<std::string>(actionBeforeInt);
        actionRelativeInt = result.get("action_relative");
        actionRelative = boost::lexical_cast<std::string>(actionRelativeInt);
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
        Wt::Dbo::Transaction transaction(*this->session);

        Wt::Dbo::ptr<UserAction> ptrUserAction = session->find<UserAction>().where("\"UAC_ID\" = ?").bind(uacId);

        UserHistoricalAction *userHistoricalAction = new UserHistoricalAction();
        userHistoricalAction->tableObject = tableObject;
        userHistoricalAction->tableObjectId = boost::lexical_cast<long long>(tableObjectId);

        Wt::Dbo::ptr<UserHistoricalAction> ptrUserHistoricalAction = session->add<UserHistoricalAction>(userHistoricalAction);
        ptrUserHistoricalAction.modify()->userAction = ptrUserAction;
        ptrUserHistoricalAction.modify()->user = session->user();
        ptrUserHistoricalAction.modify()->dateTime = Wt::WDateTime::currentDateTime();
        ptrUserHistoricalAction.modify()->actionAfter = boost::lexical_cast<int>(actionAfter);
        ptrUserHistoricalAction.modify()->actionBefore = boost::lexical_cast<int>(actionBefore);
        ptrUserHistoricalAction.modify()->actionRelative = boost::lexical_cast<int>(actionRelative);
        
        ptrUserHistoricalAction.flush();
        ptrUserHistoricalAction.modify()->setId(ptrUserHistoricalAction.id());
        responseMsg = ptrUserHistoricalAction.modify()->toJSON();    
        res = 200;
        transaction.commit();
    }
    catch (Wt::Dbo::Exception e)
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    catch(boost::bad_lexical_cast &)
    {
        res = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }
    return res;
}


void UserResource::processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    std::string responseMsg = "", nextElement = "", sRequest = "";

    sRequest = request2string(request);
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->statusCode = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }
    else
    {   
            if(!nextElement.compare("action"))
            {
                this->statusCode = postActionForUser(responseMsg, sRequest);
            }
            else
            {
                this->statusCode = 400;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
    }

    response.setStatus(this->statusCode);
    response.out() << responseMsg;
    return;
}


void UserResource::processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}


void UserResource::processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}



void UserResource::processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}


void UserResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);

    return;
}

UserResource::~UserResource() {
        beingDeleted();
}