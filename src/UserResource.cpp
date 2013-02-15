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

void UserResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response) 
{
    // Create Session and Check auth
    // PublicApiResource::handleRequest(request, response);
    session = new Session(Utils::connection);
    // set Content-Type
    response.setMimeType("application/json; charset=utf-8");

    /*  if (!this->authentified) {
          response.setStatus(401);
          response.out() << "{\"message\":\"Authentification Failed\"}";
          return;
      }*/


    // URL path after /alert
    string path = request.pathInfo();

    vector< string > splitPath;
    boost::split(splitPath, path, boost::is_any_of("/"), boost::token_compress_on);

    try 
    {
        this->userId = boost::lexical_cast<int>(splitPath[1]);
    }    
    catch (boost::bad_lexical_cast &) 
    {
        response.setStatus(422);
        response.out() << "{\"message\":\"Validation Failed\"}";
        return;
    }

    if (request.method() == "GET") 
    {
        switch (splitPath.size()) 
        {
            case 3:
               /* if (splitPath[2] == "alert") //liste les alertes d'un utlisateur
                {
                    try 
                    {
                        Wt::Dbo::Transaction transaction(*this->session);
                        Wt::Dbo::ptr<User> tempUser = session->find<User > ().where("\"USR_ID\" = ?").bind(this->userId);
                        if (tempUser) 
                        {
                            Wt::Dbo::ptr<Organization> tempOrga = tempUser->currentOrganization;
                            std::string queryString = "SELECT ale, acr, ava, mev, ams, inu FROM \"T_ALERT_ALE\" ale,"
                                    " \"T_ALERT_VALUE_AVA\" ava,"
                                    " \"T_ALERT_CRITERIA_ACR\" acr,"
                                    " \"T_MEDIA_VALUE_MEV\" mev,"
                                    " \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" ams, "
                                    " \"T_INFORMATION_UNIT_UNT\" inu "
                                    " WHERE \"ALE_ID\" IN "
                                    "("
                                    "SELECT \"AMS_ALE_ALE_ID\" FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" WHERE \"AMS_MEV_MEV_ID\" IN "
                                    "("
                                    "SELECT \"MEV_ID\" FROM \"T_MEDIA_VALUE_MEV\" WHERE \"MEV_USR_USR_ID\"  = "
                                    + boost::lexical_cast<std::string > (this->userId)
                                    + ""

                                    " )"
                                    " AND \"AMS_ALE_ALE_ID\" IS NOT NULL"
                                    ") "
                                    " AND ale.\"ALE_AVA_AVA_ID\" = ava.\"AVA_ID\" "
                                    " AND ale.\"ALE_DELETE\" IS NULL "
                                    " AND ava.\"AVA_ACR_ACR_ID\" = acr.\"ACR_ID\" "
                                    " AND ams.\"AMS_ALE_ALE_ID\" = ale.\"ALE_ID\" "
                                    " AND mev.\"MEV_ID\" = ams.\"AMS_MEV_MEV_ID\" "
                                    " AND inu.\"INU_ID\" = ava.\"INU_ID_INU_ID\" ";

                            Wt::Dbo::Query
                                    <
                                    boost::tuple
                                    <
                                    Wt::Dbo::ptr<Alert>,
                                    Wt::Dbo::ptr<AlertCriteria>,
                                    Wt::Dbo::ptr<AlertValue>,
                                    Wt::Dbo::ptr<MediaValue>,
                                    Wt::Dbo::ptr<AlertMediaSpecialization>,
                                    Wt::Dbo::ptr<InformationUnit>
                                    >
                                    , Wt::Dbo::DynamicBinding
                                    > resQuery = this->session->query
                                    <
                                    boost::tuple
                                    <
                                    Wt::Dbo::ptr<Alert>,
                                    Wt::Dbo::ptr<AlertCriteria>,
                                    Wt::Dbo::ptr<AlertValue>,
                                    Wt::Dbo::ptr<MediaValue>,
                                    Wt::Dbo::ptr<AlertMediaSpecialization>,
                                    Wt::Dbo::ptr<InformationUnit>
                                    >, Wt::Dbo::DynamicBinding
                                    > (queryString);


                            Wt::Dbo::collection<boost::tuple<
                                    Wt::Dbo::ptr<Alert>,
                                    Wt::Dbo::ptr<AlertCriteria>,
                                    Wt::Dbo::ptr<AlertValue>,
                                    Wt::Dbo::ptr<MediaValue>,
                                    Wt::Dbo::ptr<AlertMediaSpecialization>,
                                    Wt::Dbo::ptr<InformationUnit> > > listTuples = resQuery.resultList();
                            if (listTuples.size() > 0) 
                            {

                                for (Wt::Dbo::collection<boost::tuple<
                                        Wt::Dbo::ptr<Alert>,
                                        Wt::Dbo::ptr<AlertCriteria>,
                                        Wt::Dbo::ptr<AlertValue>,
                                        Wt::Dbo::ptr<MediaValue>,
                                        Wt::Dbo::ptr<AlertMediaSpecialization>,
                                        Wt::Dbo::ptr<InformationUnit> > >::const_iterator i = listTuples.begin(); i != listTuples.end(); ++i) 
                                {


                                    response.out() << "{\n\""
                                        << "  \"id\" : \"" << i->get < 0 > ().id() << "\",\n\""
                                        << "  \"alert_name\" : \"" << i->get < 0 > ().get()->name << "\",\n\""
                                        << "  \"criteria\" : \"" << i->get < 1 > ().get()->name << "\",\n\""
                                        << "  \"threshold_value\" : \"" << i->get < 2 > ().get()->value << "\",\n\""
                                        << "  \"unit\" : \"" << i->get < 5 > ().get()->name << "\",\n\""
                                        << "  \"key_value\" : \"" << i->get < 2 > ().get()->keyValue.get() << "\",\n\""
                                        << "  \"media\" : \"" << i->get < 3 > ().get()->value << "\",\n\""
                                        << "  \"snoozeDuration\" : \"" << i->get < 4 > ().get()->snoozeDuration << "\",\n\""
                                    << "}\n";

                                }
                                response.setStatus(200);
                            } 
                            else 
                            {
                                response.setStatus(404);
                                response.out() << "{\"message\":\"Alert not found\"}";
                                return;
                            }
                        } 
                        else 
                        {
                            response.setStatus(404);
                            response.out() << "{\"message\":\"User not found\"}";
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
                else*/ if (splitPath[2] == "asset") // liste les assets disponible pour l'utilisateur
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
                else if(splitPath[2] == "userOrg") //liste les utilisateurs présent dans la même organatisation
                {
                    try
                    {
                        Wt::Dbo::Transaction transaction(*this->session);
                        
                        std::string queryStr =  "SELECT usr FROM \"T_USER_USR\" usr where \"USR_ID\" IN"
                            "("
                                "SELECT \"T_USER_USR_USR_ID\" FROM \"TJ_USR_ORG\" where \"T_ORGANIZATION_ORG_ORG_ID\" IN "
                                "("
                                    "SELECT \"T_ORGANIZATION_ORG_ORG_ID\" from \"TJ_USR_ORG\" where \"T_USER_USR_USR_ID\" = " + boost::lexical_cast<std::string > (this->userId) +
                                ")"
                            ")"
                            "AND \"USR_DELETE\" IS NULL";
                        
                        Wt::Dbo::Query<Wt::Dbo::ptr<User> > queryRes = session->query<Wt::Dbo::ptr<User> >(queryStr);

                        Wt::Dbo::collection<Wt::Dbo::ptr<User> > user = queryRes.resultList();
                        
                        if (user.size() > 0)
                        {
                            for (Wt::Dbo::collection<Wt::Dbo::ptr<User> >::const_iterator i = user.begin(); i != user.end(); i++) 
                            {
                                response.out() << "{\n\""
                                            << "  \"id\" : \"" << (*i).id() << "\",\n\""
                                            << "  \"usr_firstname\" : \"" <<(*i).get()->firstName << "\",\n\""
                                            << "  \"usr_lastname\" : \"" <<(*i).get()->lastName << "\"\n\""
                                        << "}\n";
                            }
                            response.setStatus(200);
                        }
                        else 
                        {
                            response.setStatus(404);
                            response.out() << "{\"message\":\"User not found\"}";
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
               /* else if(splitPath[2] == "mediaType") // liste les type de media dispo pour l'utilisateur
                {
                    try
                    {
                        Wt::Dbo::Transaction transaction(*this->session);
                        std::string queryStr = "SELECT med FROM \"T_MEDIA_MED\" med where \"MED_ID\" IN"
                                        "("
                                            "SELECT \"MEV_MED_MED_ID\" FROM \"T_MEDIA_VALUE_MEV\" "
                                            "WHERE \"MEV_USR_USR_ID\" = " + boost::lexical_cast<std::string > (this->userId) +
                                        ")"
                                        "AND \"MED_DELETE\" IS NULL";
                        
                        Wt::Dbo::Query<Wt::Dbo::ptr<Media> > queryRes = session->query<Wt::Dbo::ptr<Media> >(queryStr);

                        Wt::Dbo::collection<Wt::Dbo::ptr<Media> > media = queryRes.resultList();
                        
                        if (media.size() > 0)
                        {
                            for (Wt::Dbo::collection<Wt::Dbo::ptr<Media> >::const_iterator i = media.begin(); i != media.end(); i++) 
                            {
                                response.out() << "{\n\""
                                            << "  \"med_name\" : \"" <<(*i).get()->name << "\"\n\""
                                        << "}\n";
                            }
                            response.setStatus(200);
                        }
                        else 
                        {
                            response.setStatus(404);
                            response.out() << "{\"message\":\"Media not found\"}";
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
                }*/
                else if(splitPath[2] == "tracking_alert") // liste les alertes qui ont déjà eu lieu
                {
                    try
                    {
                        Wt::Dbo::Transaction transaction(*this->session);
                        
                        Wt::Dbo::ptr<User> user = this->session->find<User>().where("\"USR_ID\" = ?").bind(this->userId);
                
                        std::string queryString = "SELECT ale, mev, atr FROM \"T_ALERT_TRACKING_ATR\" atr, \"T_ALERT_ALE\" ale , \"T_MEDIA_VALUE_MEV\" mev "
                            " WHERE atr.\"ATR_ALE_ALE_ID\" = ale.\"ALE_ID\" "
                            " AND ale.\"ALE_DELETE\" IS NULL "
                            " AND atr.\"ATR_MEV_MEV_ID\" = mev.\"MEV_ID\" "
                            " AND mev.\"MEV_USR_USR_ID\" IN"
                            "("
                                "SELECT \"T_USER_USR_USR_ID\" FROM \"TJ_USR_ORG\" WHERE \"T_ORGANIZATION_ORG_ORG_ID\" = " 
                                + boost::lexical_cast<std::string>(user.get()->currentOrganization.id()) + ""
                            ")";
                        Wt::Dbo::Query
                                <
                                boost::tuple
                                <
                                Wt::Dbo::ptr<Alert>,
                                Wt::Dbo::ptr<MediaValue>,
                                Wt::Dbo::ptr<AlertTracking> 
                                >
                                ,Wt::Dbo::DynamicBinding
                                >resQuery = this->session->query
                                <
                                boost::tuple
                                <
                                Wt::Dbo::ptr<Alert>,
                                Wt::Dbo::ptr<MediaValue>,
                                Wt::Dbo::ptr<AlertTracking> 
                                >
                                ,Wt::Dbo::DynamicBinding>(queryString);

                        Wt::Dbo::collection<boost::tuple
                                <
                                Wt::Dbo::ptr<Alert>,
                                Wt::Dbo::ptr<MediaValue>,
                                Wt::Dbo::ptr<AlertTracking> 
                                > > listTuples = resQuery.resultList();
                        
                        if (listTuples.size() > 0)
                        {
                            for (Wt::Dbo::collection<boost::tuple<
                                 Wt::Dbo::ptr<Alert>,
                                 Wt::Dbo::ptr<MediaValue>,
                                 Wt::Dbo::ptr<AlertTracking> > >::const_iterator i = listTuples.begin(); i != listTuples.end(); ++i) 
                            {
                                response.out() << "{\n\""
                                        //<< "  \"send_date\" : \"" << i->get<2>().get()->sendDate << "\",\n\""
                                        << "  \"alert_name\" : \"" << i->get<0>().get()->name << "\",\n\""
                                        << "  \"value\" : \"" << i->get<1>().get()->value << "\"\n\"" 
                                    << "}\n";
                            }
                            response.setStatus(200);
                            
                        }
                        else
                        {
                            response.setStatus(404);
                            response.out() << "{\"message\":\"Tracking alert not found\"}";
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
                else if (splitPath[2] == "quota_sms") // renvoie le quota sms restant de l'organisation dans laquelle est l'utilisateur
                {
                    try
                    {
                        Wt::Dbo::Transaction transaction(*this->session);
                        Wt::Dbo::ptr<User> user = session->find<User>().where("\"USR_ID\" = ?").bind(this->userId);

                        if (user)
                        {
                            Wt::Dbo::ptr<Organization> tempOrga = user->currentOrganization;
                            Wt::Dbo::ptr<PackOption> ptrPackOption = session->find<PackOption>()
                                    .where("\"POP_PCK_PCK_ID\" = ?").bind(tempOrga.get()->pack.id())
                                    .where("\"POP_OPT_OPT_ID\" = 2")
                                    .limit(1);
                            if (ptrPackOption.get())
                            {
                                Wt::Dbo::ptr<OptionValue> ptrOptionValue = session->find<OptionValue>().where("\"OPT_ID_OPT_ID\" = ?").bind(ptrPackOption.get()->pk.option.id())
                                                                                .where("\"ORG_ID_ORG_ID\" = ?").bind(tempOrga.id())
                                                                                .limit(1);
                                if (ptrOptionValue.get())
                                {
                                    response.setStatus(200);
                                    response.out() << "{\n\""
                                        << "  \"opv_value\" : \"" << ptrOptionValue.get()->value << "\"\n\""
                                    << "}\n";
                                }
                            }
                        }
                        else
                        {
                            response.setStatus(404);
                            response.out() << "{\"message\":\"user not found\"}";
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
                else 
                {
                    response.setStatus(422);
                    response.out() << "{\"message\":\"Validation Failed\"}";
                }
                break;
                
                
                
                
           /* case 4:
                if(splitPath[2] == "media") // liste les media de type mail ou sms de l'utilisateur
                {
                    try 
                    {
                        this->medId = boost::lexical_cast<int>(splitPath[3]);
                    }    
                    catch (boost::bad_lexical_cast &) 
                    {
                        response.setStatus(422);
                        response.out() << "{\"message\":\"Validation Failed\"}";
                        return;
                    }
                    try
                    {
                        Wt::Dbo::Transaction transaction(*this->session);
                        Wt::Dbo::collection<Wt::Dbo::ptr<MediaValue> > medias = session->find<MediaValue>()
                                                                                .where("\"MEV_USR_USR_ID\" = ?").bind(this->userId)
                                                                                .where("\"MEV_MED_MED_ID\" = ?").bind(medId);
                        if (medias.size() > 0)
                        {
                            for (Wt::Dbo::collection<Wt::Dbo::ptr<MediaValue> >::const_iterator i = medias.begin(); i != medias.end(); ++i)
                            {
                                response.out() << "{\n\""
                                            << "  \"mev_value\" : \"" <<(*i).get()->value << "\"\n\""
                                        << "}\n";
                            }
                            response.setStatus(200);
                        }
                        else
                        {
                            response.setStatus(404);
                            response.out() << "{\"message\":\"Media not found\"}";
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
                else 
                {
                    response.setStatus(422);
                    response.out() << "{\"message\":\"Validation Failed\"}";
                }
                break;*/
                
            default:
                response.setStatus(422);
                response.out() << "{\"message\":\"Validation Failed\"}";
                break;
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
    else 
    {
        response.setStatus(405);
        response.out() << "{\"message\":\"Only GET or POST method is allowed.\"}";
        return;
    }

}

UserResource::~UserResource() {
}