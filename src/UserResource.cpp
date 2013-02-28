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

using namespace std;

UserResource::UserResource() {
}

string UserResource::getInformationForUser()
{
    string res = "";
    try
    {        
        Wt::Dbo::Transaction transaction(*this->session);
        Wt::Dbo::ptr<User> user = session->find<User>().where("\"USR_ID\" = ?").bind(this->session->user().id());
      
        if(user)
        {
            res += user.modify()->toJSON();
            this->statusCode = 200;
        }
        else
        {
            res = "{\"message\":\"User not found\"}";
            this->statusCode = 404;
        }
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

void UserResource::processGetRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";
    
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        responseMsg = getInformationForUser();
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

/*string UserResource::postRegistrationForUser(string sRequest)
{
    string res = "";
    Wt::WString firstName, lastName, eMail, organizationType, token, pckId, optId, optValue;
    
    try
    {
        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);
        
        firstName = result.get("first_name");
        lastName = result.get("last_name");
        eMail = result.get("email");
        organizationType = result.get("organization_type");
        token = result.get("token");
        pckId = result.get("pck_id");
        optId = result.get("opt_id");
        optValue = result.get("option_value");
       
    }
    catch (Wt::Json::ParseError const& e)
    {
        this->statusCode = 400;
        res = "{\"message\":\"Problems parsing JSON\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON:" << sRequest;
        return res;
    }
    catch (Wt::Json::TypeException const& e)
    {
        this->statusCode = 400;
        res = "{\"message\":\"Problems parsing JSON.\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON.:" << sRequest;
        return res;
    }
    try
    {
        Wt::Auth::RegistrationModel *model_;
        Wt::Auth::User user = model_->doRegister();
        Session *session = static_cast<Session*>(dynamic_cast<UserDatabase*>(user.database())->find(user).get()->user().get()->session());
    
        dynamic_cast<UserDatabase*>(user.database())->find(user).get()->user().modify()->firstName = firstName;
        dynamic_cast<UserDatabase*>(user.database())->find(user).get()->user().modify()->eMail = eMail;
        dynamic_cast<UserDatabase*>(user.database())->find(user).get()->user().modify()->lastName = lastName;
    }
    catch (Wt::Dbo::Exception e)
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\"message\":\"Service Unavailable\"}";
        return res;
    }   
}*/

string UserResource::postActionForUser(string sRequest)
{
    string res = "";
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
        this->statusCode = 400;
        res = "{\"message\":\"Problems parsing JSON\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON:" << sRequest;
        return res;
    }
    catch (Wt::Json::TypeException const& e)
    {
        this->statusCode = 400;
        res = "{\"message\":\"Problems parsing JSON.\"}";
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

        this->statusCode = 200;
        res = "{\"message\":\"Action added\"}";
        transaction.commit();
    }
    catch (Wt::Dbo::Exception e)
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    catch(boost::bad_lexical_cast &)
    {
        this->statusCode = 400;
        res = "{\n\t\"message\":\"Bad Request\"\n}";
    }
    return res;
}


void UserResource::processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "", sRequest = "";

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
                responseMsg = postActionForUser(sRequest);
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


/*
                else if (splitPath[2] == "asset") // liste les assets disponible pour l'utilisateur
                {
                    try
                    {
                        Wt::Dbo::Transaction transaction(*this->session);
                        std::string queryStr = " SELECT ast FROM \"T_ASSET_AST\" ast" 
                                               " WHERE \"AST_PRB_PRB_ID\" IN " 
                                               "("
                                               "SELECT \"PRB_ID\" FROM \"T_PROBE_PRB\" "
                                               " WHERE \"PRB_ORG_ORG_ID\" IN "
                                               "("
                                               "SELECT \"ORG_ID\" FROM \"T_ORGANIZATION_ORG\" "
                                               " WHERE \"ORG_ID\" IN "
                                               "("
                                               "SELECT \"CURRENT_ORG_ID\" FROM \"T_USER_USR\" "
                                               " WHERE \"USR_ID\" = " + boost::lexical_cast<std::string > (this->userId) + ""
                                               " )"
                                               " )"
                                               " )"
                                               "AND \"AST_DELETE\" IS NULL";

                        Wt::Dbo::Query<Wt::Dbo::ptr<Asset> > queryRes = session->query<Wt::Dbo::ptr<Asset> >(queryStr);

                        Wt::Dbo::collection<Wt::Dbo::ptr<Asset> > asset = queryRes.resultList();

                        if (asset.size() > 0)
                        {
                            for (Wt::Dbo::collection<Wt::Dbo::ptr<Asset> >::const_iterator i = asset.begin(); i != asset.end(); i++) 
                            {
                                response.out() << "{\n\""
                                            << "  \"id\" : \"" << (*i).id() << "\",\n\""
                                            << "  \"ast_name\" : \"" <<(*i).get()->name << "\"\n\""
                                        << "}\n";
                            }
                            response.setStatus(200);
                        }
                        else 
                        {
                            response.setStatus(404);
                            response.out() << "{\"message\":\"Asset not found\"}";
                            return;
                        }
                        transaction.commit();
                    }
                    catch (Wt::Dbo::Exception const& e) 
                    {
                        Wt::log("error") << e.what();
                        response.setStatus(503);
                        response.out() << "{\"message\":\"Service Unavailable\"}";
                        return;
                    }
                    
                }  
*/
UserResource::~UserResource() {
        beingDeleted();
}