/* 
 * API AlertRessource
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


#include "AlertResource.h"

using namespace std;

AlertRessource::AlertRessource(){
}


/*void AssetRessource::processGetRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}


string AlertRessource::postAlert(string sRequest)
{
    string res = "";
    Wt::WString alertName, alertValue, threadSleep, keyVal, astId, seaId, 
                srcId, plgId, infValNum, inuId, acrId, amsId, snooze, mevId;

    try
    {
        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);
        //descriptif
        alertName = result.get("name");
        alertValue = result.get("alert_value");
        threadSleep = result.get("thread_sleep");
        keyVal = result.get("key_value");
        snooze = result.get("ams_snooze");
        //Id asset à verifer
        astId = result.get("ast_id");
        //information
        seaId = result.get("sea_id");
        srcId = result.get("src_id");
        plgId = result.get("plg_id");
        infValNum = result.get("inf_value_num");
        inuId = result.get("inu_id");
        //operateur
        acrId = result.get("acr_id");
        //serveur
        amsId = result.get("ams_id");
        //media
        mevId = result.get("mev_id");
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

        //info requête bonne?
         Wt::Dbo::ptr<Information2> infoPtr = session->find<Information2>().where("\"SEA_ID\" = ?")
                                                        .bind(seaId)
                                                        .where("\"SRC_ID\" = ?")
                                                        .bind(srcId)
                                                        .where("\"PLG_ID_PLG_ID\" = ?")
                                                        .bind(plgId)
                                                        .where("\"INF_VALUE_NUM\" = ?")
                                                        .bind(infValNum)
                                                        .where("\"INU_ID_INU_ID\" = ?")
                                                        .bind(inuId);

        Wt::Dbo::ptr<AlertCriteria> critPtr = session->find<AlertCriteria>().where("\"ACR_ID\" = ?").bind(acrId);

        Wt::Dbo::ptr<Asset> assetPtr = session->find<Asset>().where("\"AST_ID\" = ?").bind(astId);

        Wt::Dbo::ptr<MediaValue> mevPtr = session->find<MediaValue>().where("\"MEV_ID\" = ?").bind(mevId);

        if (!infoPtr || !critPtr || !assetPtr || !mevPtr)
        {
             response.setStatus(404);
             response.out() << "{\"message\":\"Not found\"}";
             return; 
        }



        //Alert already exist ?
        Wt::Dbo::collection<Wt::Dbo::ptr<AlertValue> > avaPtrCollec = session->find<AlertValue>()
                                                        .where("\"SEA_ID\" = ?")
                                                        .bind(seaId)
                                                        .where("\"SRC_ID\" = ?")
                                                        .bind(srcId)
                                                        .where("\"PLG_ID_PLG_ID\" = ?")
                                                        .bind(plgId)
                                                        .where("\"INF_VALUE_NUM\" = ?")
                                                        .bind(infValNum)
                                                        .where("\"INU_ID_INU_ID\" = ?")
                                                        .bind(inuId);

        if (avaPtrCollec.size() > 0)
        {
            std::string inString = "(";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<AlertValue> >::const_iterator i = avaPtrCollec.begin(); i != avaPtrCollec.end(); i++) 
            {
                Wt::log("debug") << " [AlertRessource] " << " - " << " For ava list : " << (*i).id();
                inString += boost::lexical_cast<std::string,long long>((*i).id()) + ",";
                i->flush();
            }
            inString.replace(inString.size()-1, 1, "");
            inString += ")";


            std::string queryStr = "SELECT ale FROM \"T_ALERT_ALE\" ale WHERE "
                                    " \"ALE_ID\" IN"
                                    "("
                                        "SELECT \"T_ALERT_ALE_ALE_ID\" FROM \"TJ_AST_ALE\" WHERE \"T_ASSET_AST_AST_ID\" = " 
                                        + boost::lexical_cast<std::string>(astId) +
                                    ")"
                                    "AND ale.\"ALE_DELETE\" IS NULL "
                                    "AND \"ALE_AVA_AVA_ID\" IN" + inString;

            Wt::Dbo::Query<Wt::Dbo::ptr<Alert> > queryRes = session->query<Wt::Dbo::ptr<Alert> >(queryStr);

            Wt::Dbo::collection<Wt::Dbo::ptr<Alert> > alerts = queryRes.resultList();

            if (alerts.size() > 0)
            {

                std::string inString2 = "(";
                for (Wt::Dbo::collection<Wt::Dbo::ptr<Alert> >::const_iterator i = alerts.begin(); i != alerts.end(); i++) 
                {
                    cerr << " [AlertRessource] " << " - " << " For ale list : " << (*i).id() << "\n";
                    inString2 += boost::lexical_cast<std::string,long long>((*i).id()) + ",";
                    i->flush();
                }
                inString2.replace(inString2.size()-1, 1, "");
                inString2 += ")";

                std::string queryStr2 = " SELECT ams FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" ams WHERE "
                                        " \"AMS_ALE_ALE_ID\" IN" + inString2 +
                                        " AND \"AMS_MEV_MEV_ID\" = " + boost::lexical_cast<std::string>(mevId);


                Wt::Dbo::Query<Wt::Dbo::ptr<AlertMediaSpecialization> > queryRes2 = session->query<Wt::Dbo::ptr<AlertMediaSpecialization> >(queryStr2);

                Wt::Dbo::collection<Wt::Dbo::ptr<AlertMediaSpecialization> > alertMS = queryRes2.resultList();

                if(alertMS.size() > 0)
                {

                response.setStatus(400);
                response.out() << "{\"message\":\"Alert already exists\"}";

                return;
                }
            }

        }

        transaction.commit();

    }
    catch (Wt::Dbo::Exception e)
    {
        Wt::log("error") << "[AlertRessource]" << e.what();
        response.setStatus(503);
        response.out() << "{\"message\":\"Service Unavailable\"}";
        return;
    }

    //Create new alert
    Alert *alert = new Alert();
    AlertValue *ava = new AlertValue();
    AlertMediaSpecialization *ams = new AlertMediaSpecialization();

    try
    {
        Wt::Dbo::Transaction transaction(*session);

        Wt::Dbo::ptr<Information2> infoPtr = session->find<Information2>().where("\"SEA_ID\" = ?")
                                                        .bind(seaId)
                                                        .where("\"SRC_ID\" = ?")
                                                        .bind(srcId)
                                                        .where("\"PLG_ID_PLG_ID\" = ?")
                                                        .bind(plgId)
                                                        .where("\"INF_VALUE_NUM\" = ?")
                                                        .bind(infValNum)
                                                        .where("\"INU_ID_INU_ID\" = ?")
                                                        .bind(inuId);

        Wt::Dbo::ptr<AlertCriteria> critPtr = session->find<AlertCriteria>().where("\"ACR_ID\" = ?").bind(acrId);

        Wt::Dbo::ptr<Asset> assetPtr = session->find<Asset>().where("\"AST_ID\" = ?").bind(astId);

        Wt::Dbo::ptr<MediaValue> mevPtr = session->find<MediaValue>().where("\"MEV_ID\" = ?").bind(mevId);

        ava->information = infoPtr;
        ava->alertCriteria = critPtr;
        ava->value = alertValue;
        ava->keyValue = keyVal;

        Wt::Dbo::ptr<AlertValue> avaPtr = session->add<AlertValue>(ava);



        alert->alertValue = avaPtr;
        alert->name = alertName;
        alert->creaDate = Wt::WDateTime::currentDateTime();
        alert->threadSleep = boost::lexical_cast<int>(threadSleep);


        Wt::Dbo::ptr<Alert> alePtr = session->add<Alert>(alert);

        alePtr.modify()->assets.insert(assetPtr);




        ams->snoozeDuration = boost::lexical_cast<int>(snooze);
        Wt::Dbo::ptr<AlertMediaSpecialization> amsPtr = session->add<AlertMediaSpecialization>(ams);
        amsPtr.modify()->alert = alePtr;
        amsPtr.modify()->mediaValue = mevPtr;

        transaction.commit();

        response.setStatus(200);
        response.out() << "{\"message\":\"Alert added\"}";

    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        response.setStatus(503);
        response.out() << "{\"message\":\"Service Unavailable\"}";
        return;
    }
    
}


void AssetRessource::processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";

    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        responseMsg = postAlert();
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





void AssetRessource::processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}






void AssetRessource::processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}





string AlertRessource::DeleteAlert(string sRequest)
{
    try 
    {

        Wt::Dbo::Transaction transaction(*this->session);
        Wt::Dbo::ptr<Alert> alertPtr = session->find<Alert>().where("\"ALE_ID\" = ?").bind(boost::lexical_cast<int>(this->alertId)); 

        std::string executeString1 = " SELECT astale FROM \"TJ_AST_ALE\" astale" 
                                    " WHERE astale.\"T_ALERT_ALE_ALE_ID\" = " + boost::lexical_cast<std::string>(this->alertId) + "FOR UPDATE";
        std::string executeString1bis = " DELETE FROM \"TJ_AST_ALE\" " 
                                        " WHERE \"TJ_AST_ALE\".\"T_ALERT_ALE_ALE_ID\" = " + boost::lexical_cast<std::string>(this->alertId);
        session->execute(executeString1);
        session->execute(executeString1bis);

        std::string executeString2 = "SELECT ams FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" ams " 
                                     " WHERE \"AMS_ALE_ALE_ID\" = " + boost::lexical_cast<std::string>(this->alertId) + "FOR UPDATE";
        std::string executeString2bis =  " DELETE FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" "
                                    " WHERE \"AMS_ALE_ALE_ID\" = " + boost::lexical_cast<std::string>(this->alertId);


        session->execute(executeString2);
        session->execute(executeString2bis);

        alertPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
        transaction.commit();
        std::cerr<<"after commit";

        response.setStatus(200);
        response.out() << "{\"message\":\"Alert deleted\"}";

    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        response.setStatus(503);
        response.out() << "{\"message\":\"Service Unavailable\"}";
        return;
    }
}


void AssetRessource::processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
  /*  string responseMsg = "", nextElement = "";

    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        responseMsg = deleteAlert();
    }
    else
    {
        this->statusCode = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }

    response.setStatus(this->statusCode);
    response.out() << responseMsg;
    return;*//*
}


void AssetRessource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);

    return;
}


*/















void AlertRessource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
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
        this->alertId = boost::lexical_cast<int>(splitPath[1]);
    }
    catch(boost::bad_lexical_cast &)
    {
        response.setStatus(422);
        response.out() << "{\"message\":\"Validation Failed\"}";
        return;
    }
    
    if (this->alertId)
    {
        if (request.method() == "POST")
        {
        switch (splitPath.size())
            {
            case 3:
                if (splitPath[2] == "delete") // supprime une alerte
                {
                    try 
                    {

                        Wt::Dbo::Transaction transaction(*this->session);
                        Wt::Dbo::ptr<Alert> alertPtr = session->find<Alert>().where("\"ALE_ID\" = ?").bind(boost::lexical_cast<int>(this->alertId)); 

                        std::string executeString1 = " SELECT astale FROM \"TJ_AST_ALE\" astale" 
                                                    " WHERE astale.\"T_ALERT_ALE_ALE_ID\" = " + boost::lexical_cast<std::string>(this->alertId) + "FOR UPDATE";
                        std::string executeString1bis = " DELETE FROM \"TJ_AST_ALE\" " 
                                                        " WHERE \"TJ_AST_ALE\".\"T_ALERT_ALE_ALE_ID\" = " + boost::lexical_cast<std::string>(this->alertId);
                        session->execute(executeString1);
                        session->execute(executeString1bis);

                        std::string executeString2 = "SELECT ams FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" ams " 
                                                     " WHERE \"AMS_ALE_ALE_ID\" = " + boost::lexical_cast<std::string>(this->alertId) + "FOR UPDATE";
                        std::string executeString2bis =  " DELETE FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" "
                                                    " WHERE \"AMS_ALE_ALE_ID\" = " + boost::lexical_cast<std::string>(this->alertId);

                        
                        session->execute(executeString2);
                        session->execute(executeString2bis);

                        alertPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
                        transaction.commit();
                        std::cerr<<"after commit";

                        response.setStatus(200);
                        response.out() << "{\"message\":\"Alert deleted\"}";

                    }
                    catch (Wt::Dbo::Exception const& e) 
                    {
                        Wt::log("error") << e.what();
                        response.setStatus(503);
                        response.out() << "{\"message\":\"Service Unavailable\"}";
                        return;
                    }
                }
                else if (splitPath[2] == "add") //ajoute une alerte
                {
                    string json;
                    Wt::WString alertName, alertValue, threadSleep, keyVal, astId, seaId, 
                            srcId, plgId, infValNum, inuId, acrId, amsId, snooze, mevId;

                    json = request2string(request);
                    try
                    {
                        
                        Wt::Json::Object result;                   
                        Wt::Json::parse(json, result);
                        //descriptif
                        alertName = result.get("name");
                        alertValue = result.get("alert_value");
                        threadSleep = result.get("thread_sleep");
                        keyVal = result.get("key_value");
                        snooze = result.get("ams_snooze");
                        //Id asset à verifer
                        astId = result.get("ast_id");
                        //information
                        seaId = result.get("sea_id");
                        srcId = result.get("src_id");
                        plgId = result.get("plg_id");
                        infValNum = result.get("inf_value_num");
                        inuId = result.get("inu_id");
                        //operateur
                        acrId = result.get("acr_id");
                        //serveur
                        amsId = result.get("ams_id");
                        //media
                        mevId = result.get("mev_id");
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

                        //info requête bonne?
                         Wt::Dbo::ptr<Information2> infoPtr = session->find<Information2>().where("\"SEA_ID\" = ?")
                                                                        .bind(seaId)
                                                                        .where("\"SRC_ID\" = ?")
                                                                        .bind(srcId)
                                                                        .where("\"PLG_ID_PLG_ID\" = ?")
                                                                        .bind(plgId)
                                                                        .where("\"INF_VALUE_NUM\" = ?")
                                                                        .bind(infValNum)
                                                                        .where("\"INU_ID_INU_ID\" = ?")
                                                                        .bind(inuId);
                        
                        Wt::Dbo::ptr<AlertCriteria> critPtr = session->find<AlertCriteria>().where("\"ACR_ID\" = ?").bind(acrId);
                       
                        Wt::Dbo::ptr<Asset> assetPtr = session->find<Asset>().where("\"AST_ID\" = ?").bind(astId);
                        
                        Wt::Dbo::ptr<MediaValue> mevPtr = session->find<MediaValue>().where("\"MEV_ID\" = ?").bind(mevId);
                        
                        if (!infoPtr || !critPtr || !assetPtr || !mevPtr)
                        {
                             response.setStatus(404);
                             response.out() << "{\"message\":\"Not found\"}";
                             return; 
                        }
                        
                        
                        
                        //Alert already exist ?
                        Wt::Dbo::collection<Wt::Dbo::ptr<AlertValue> > avaPtrCollec = session->find<AlertValue>()
                                                                        .where("\"SEA_ID\" = ?")
                                                                        .bind(seaId)
                                                                        .where("\"SRC_ID\" = ?")
                                                                        .bind(srcId)
                                                                        .where("\"PLG_ID_PLG_ID\" = ?")
                                                                        .bind(plgId)
                                                                        .where("\"INF_VALUE_NUM\" = ?")
                                                                        .bind(infValNum)
                                                                        .where("\"INU_ID_INU_ID\" = ?")
                                                                        .bind(inuId);

                        if (avaPtrCollec.size() > 0)
                        {
                            std::string inString = "(";
                            for (Wt::Dbo::collection<Wt::Dbo::ptr<AlertValue> >::const_iterator i = avaPtrCollec.begin(); i != avaPtrCollec.end(); i++) 
                            {
                                Wt::log("debug") << " [AlertRessource] " << " - " << " For ava list : " << (*i).id();
                                inString += boost::lexical_cast<std::string,long long>((*i).id()) + ",";
                                i->flush();
                            }
                            inString.replace(inString.size()-1, 1, "");
                            inString += ")";


                            std::string queryStr = "SELECT ale FROM \"T_ALERT_ALE\" ale WHERE "
                                                    " \"ALE_ID\" IN"
                                                    "("
                                                        "SELECT \"T_ALERT_ALE_ALE_ID\" FROM \"TJ_AST_ALE\" WHERE \"T_ASSET_AST_AST_ID\" = " 
                                                        + boost::lexical_cast<std::string>(astId) +
                                                    ")"
                                                    "AND ale.\"ALE_DELETE\" IS NULL "
                                                    "AND \"ALE_AVA_AVA_ID\" IN" + inString;

                            Wt::Dbo::Query<Wt::Dbo::ptr<Alert> > queryRes = session->query<Wt::Dbo::ptr<Alert> >(queryStr);

                            Wt::Dbo::collection<Wt::Dbo::ptr<Alert> > alerts = queryRes.resultList();
                            
                            if (alerts.size() > 0)
                            {
                           
                                std::string inString2 = "(";
                                for (Wt::Dbo::collection<Wt::Dbo::ptr<Alert> >::const_iterator i = alerts.begin(); i != alerts.end(); i++) 
                                {
                                    cerr << " [AlertRessource] " << " - " << " For ale list : " << (*i).id() << "\n";
                                    inString2 += boost::lexical_cast<std::string,long long>((*i).id()) + ",";
                                    i->flush();
                                }
                                inString2.replace(inString2.size()-1, 1, "");
                                inString2 += ")";

                                std::string queryStr2 = " SELECT ams FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" ams WHERE "
                                                        " \"AMS_ALE_ALE_ID\" IN" + inString2 +
                                                        " AND \"AMS_MEV_MEV_ID\" = " + boost::lexical_cast<std::string>(mevId);


                                Wt::Dbo::Query<Wt::Dbo::ptr<AlertMediaSpecialization> > queryRes2 = session->query<Wt::Dbo::ptr<AlertMediaSpecialization> >(queryStr2);

                                Wt::Dbo::collection<Wt::Dbo::ptr<AlertMediaSpecialization> > alertMS = queryRes2.resultList();

                                if(alertMS.size() > 0)
                                {

                                response.setStatus(400);
                                response.out() << "{\"message\":\"Alert already exists\"}";
                               
                                return;
                                }
                            }

                        }
                       
                        transaction.commit();

                    }
                    catch (Wt::Dbo::Exception e)
                    {
                        Wt::log("error") << "[AlertRessource]" << e.what();
                        response.setStatus(503);
                        response.out() << "{\"message\":\"Service Unavailable\"}";
                        return;
                    }
                    
                    //Create new alert
                    Alert *alert = new Alert();
                    AlertValue *ava = new AlertValue();
                    AlertMediaSpecialization *ams = new AlertMediaSpecialization();

                    try
                    {
                        Wt::Dbo::Transaction transaction(*session);

                        Wt::Dbo::ptr<Information2> infoPtr = session->find<Information2>().where("\"SEA_ID\" = ?")
                                                                        .bind(seaId)
                                                                        .where("\"SRC_ID\" = ?")
                                                                        .bind(srcId)
                                                                        .where("\"PLG_ID_PLG_ID\" = ?")
                                                                        .bind(plgId)
                                                                        .where("\"INF_VALUE_NUM\" = ?")
                                                                        .bind(infValNum)
                                                                        .where("\"INU_ID_INU_ID\" = ?")
                                                                        .bind(inuId);
                        
                        Wt::Dbo::ptr<AlertCriteria> critPtr = session->find<AlertCriteria>().where("\"ACR_ID\" = ?").bind(acrId);
                       
                        Wt::Dbo::ptr<Asset> assetPtr = session->find<Asset>().where("\"AST_ID\" = ?").bind(astId);
                        
                        Wt::Dbo::ptr<MediaValue> mevPtr = session->find<MediaValue>().where("\"MEV_ID\" = ?").bind(mevId);
                        
                        ava->information = infoPtr;
                        ava->alertCriteria = critPtr;
                        ava->value = alertValue;
                        ava->keyValue = keyVal;

                        Wt::Dbo::ptr<AlertValue> avaPtr = session->add<AlertValue>(ava);
                        
                        

                        alert->alertValue = avaPtr;
                        alert->name = alertName;
                        alert->creaDate = Wt::WDateTime::currentDateTime();
                        alert->threadSleep = boost::lexical_cast<int>(threadSleep);


                        Wt::Dbo::ptr<Alert> alePtr = session->add<Alert>(alert);
                        
                        alePtr.modify()->assets.insert(assetPtr);
                        
                        
                        
                        
                        ams->snoozeDuration = boost::lexical_cast<int>(snooze);
                        Wt::Dbo::ptr<AlertMediaSpecialization> amsPtr = session->add<AlertMediaSpecialization>(ams);
                        amsPtr.modify()->alert = alePtr;
                        amsPtr.modify()->mediaValue = mevPtr;
                        
                        transaction.commit();
                        
                        response.setStatus(200);
                        response.out() << "{\"message\":\"Alert added\"}";
                        
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
                
             default:
                response.setStatus(422);
                response.out() << "{\"message\":\"Validation Failed\"}";
                return;
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
}


AlertRessource::~AlertRessource()
{
    beingDeleted();
}