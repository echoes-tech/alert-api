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


void UserResource::processGetRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}


void UserResource::processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{

    return ;
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

   /* else if(request.method() == "POST")
    {
        switch (splitPath.size()) 
        {
            case 3:
                if (splitPath[2] == "action") // ajoute chaque action qu'un utilisateur fait
                {
                    string json;
                    Wt::WString uacId, tableObject, tableObjectId, actionAfter, actionBefore, actionRelative;

                    json = request2string(request);
                    try
                    {
                        
                        Wt::Json::Object result;                   
                        Wt::Json::parse(json, result);
                        
                        uacId = result.get("uac_id");
                        tableObject = result.get("table_object");
                        tableObjectId = result.get("table_object_id");
                        actionAfter = result.get("action_after");
                        actionBefore = result.get("action_before");
                        actionRelative = result.get("action_relative");
                        
                    }
                    
                    catch (Wt::Json::ParseError const& e)
                    {
                        response.setStatus(400);
                        response.out() << "{\"message\":\"Problems parsing JSON\"}";
                        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON:" << json;
                        return;
                    }
                    catch (Wt::Json::TypeException const& e)
                    {
                        response.setStatus(400);
                        response.out() << "{\"message\":\"Problems parsing JSON.\"}";
                        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON.:" << json;
                        return;
                    }
                    try
                    {
                        Wt::Dbo::Transaction transaction(*this->session);
                        
                        Wt::Dbo::ptr<UserAction> ptrUserAction = session->find<UserAction>().where("\"UAC_ID\" = ?").bind(uacId);

                        UserHistoricalAction *userHistoricalAction = new UserHistoricalAction();
                        userHistoricalAction->tableObject = tableObject;
                        userHistoricalAction->tableObjectId = id;

                        Wt::Dbo::ptr<UserHistoricalAction> ptrUserHistoricalAction = session->add<UserHistoricalAction>(userHistoricalAction);
                        ptrUserHistoricalAction.modify()->userAction = ptrUserAction;
                        ptrUserHistoricalAction.modify()->user = session->user();
                        ptrUserHistoricalAction.modify()->dateTime = Wt::WDateTime::currentDateTime();
                        ptrUserHistoricalAction.modify()->actionAfter = 0;
                        ptrUserHistoricalAction.modify()->actionBefore = 0;
                        ptrUserHistoricalAction.modify()->actionRelative = 0;

                        transaction.commit();
                    }
                    catch (Wt::Dbo::Exception e)
                    {
                        Wt::log("error") << e.what();
                        response.setStatus(503);
                        response.out() << "{\"message\":\"Service Unavailable\"}";
                        return;
                    } 
                }
                else 
                {
                    response.setStatus(422);
                    response.out() << "{\"message\":\"Validation Failed\"}";
                }
                break;
            default:
                response.setStatus(422);
                response.out() << "{\"message\":\"Validation Failed\"}";
                break;
        }
    }*/
/*    else 
    {
        response.setStatus(405);
        response.out() << "{\"message\":\"Only GET or POST method is allowed.\"}";
        return;
    }

}
*/
UserResource::~UserResource() {
        beingDeleted();
}