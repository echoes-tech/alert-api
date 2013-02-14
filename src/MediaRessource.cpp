/* 
 * API MediaRessource
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


#include "MediaRessource.h"


using namespace std;

MediaRessource::MediaRessource(){
}


void MediaRessource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
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
  
    
  // URL path after /media
    string path = request.pathInfo();

    vector< string > splitPath;
    boost::split(splitPath, path, boost::is_any_of("/"), boost::token_compress_on);
    
     if (request.method() == "POST") 
     {
        switch (splitPath.size()) 
        {
            case 2:
                if (splitPath[1] == "add") //ajoute un media à un utilisateur
                {
                    string json;
                    Wt::WString usrId, medId, mevValue;

                    json = request2string(request);
                    try
                    {
                        
                        Wt::Json::Object result;                   
                        Wt::Json::parse(json, result);
                        
                        usrId = result.get("user_id");
                        medId = result.get("med_id");
                        mevValue = result.get("mev_value");
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
                        Wt::Dbo::Transaction transaction(*session);
                        Wt::Dbo::ptr<User> ptrUser = session->find<User>().where("\"USR_ID\" = ?").bind(usrId);
                        Wt::Dbo::ptr<Media> media = session->find<Media>().where("\"MED_ID\" = ?").bind(medId);

                        if(!ptrUser || !media)
                        {
                            response.setStatus(404);
                            response.out() << "{\"message\":\"Not found\"}";
                            return; 
                        }
                        
                        MediaValue *mev = new MediaValue();
                        mev->user= ptrUser;
                        mev->media = media;
                        mev->value = mevValue;
                        Wt::Dbo::ptr<MediaValue> ptrMev = session->add<MediaValue>(mev);
                        
                        transaction.commit();
                        
                        response.setStatus(200);
                        response.out() << "{\"message\":\"Media added\"}";
                        
                    }
                    catch (Wt::Dbo::Exception const& e) 
                    {
                        Wt::log("error") << e.what();
                        response.setStatus(503);
                        response.out() << "{\"message\":\"Service Unavailable\"}";
                        return;
                    }
                }
                else if (splitPath[1] == "delete") //supprime un media à un utilisateur
                {
                    string json;
                    Wt::WString usrId, medId, mevValue;

                    json = request2string(request);
                    try
                    {
                        
                        Wt::Json::Object result;                   
                        Wt::Json::parse(json, result);
                        
                        usrId = result.get("user_id");
                        medId = result.get("med_id");
                        mevValue = result.get("mev_value");
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
                        Wt::Dbo::Transaction transaction2(*session);

                        
                        
                        std::string qryString = "DELETE FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" "
                                                " WHERE \"AMS_ALE_ALE_ID\" IS NULL"
                                                " AND \"AMS_MEV_MEV_ID\" IN "
                                                " (SELECT \"MEV_ID\" FROM \"T_MEDIA_VALUE_MEV\" WHERE \"MEV_USR_USR_ID\" = " + boost::lexical_cast<std::string>(usrId)  + ")";

                        session->execute(qryString);

                        transaction2.commit();
                    }
                    catch (Wt::Dbo::Exception e)
                    {
                        Wt::log("error") << e.what();
                        response.setStatus(503);
                        response.out() << "{\"message\":\"Service Unavailable\"}";
                        return;
                    }
                    try
                    {
                        Wt::Dbo::Transaction transaction(*session);
                        
                        Wt::Dbo::ptr<MediaValue> mediaValue = session->find<MediaValue>().where("\"MEV_MED_MED_ID\" = ?").bind(medId)
                                                                          .where("\"MEV_USR_USR_ID\" = ?").bind(usrId)
                                                                          .where("\"MEV_VALUE\" = ?").bind(mevValue);
                        
                        if(!mediaValue)
                        {
                            response.setStatus(404);
                            response.out() << "{\"message\":\"Media not found\"}";
                            return; 
                        }
                        
                        session->execute("DELETE FROM \"T_MEDIA_VALUE_MEV\" WHERE \"MEV_VALUE\" = \'" + boost::lexical_cast<std::string>(mevValue) + "\'"
                                         " AND \"MEV_MED_MED_ID\" = " + boost::lexical_cast<std::string>(medId) +
                                         " AND \"MEV_USR_USR_ID\" = " + boost::lexical_cast<std::string>(usrId));

                        transaction.commit();
                        response.setStatus(200);
                        response.out() << "{\"message\":\"Media deleted\"}";
                    }
                    catch (Wt::Dbo::Exception e)
                    {
                        Wt::log("error") << e.what();
                        response.setStatus(409);
                        response.out() << "{\"message\":\"Conflict, an alert use this media\"}";
                        return;
                    }

                }
                else
                {
                    response.setStatus(422);
                    response.out() << "{\"message\":\"Validation Failed\"}";
                }
                break;
            default :
                response.setStatus(422);
                response.out() << "{\"message\":\"Validation Failed\"}";
                break;
        }
     }
     else 
     { 
        response.setStatus(405);
        response.out() << "{\"message\":\"Only POST method is allowed.\"}";
        return;
        
     }
}


MediaRessource::~MediaRessource()
{
    beingDeleted();
}