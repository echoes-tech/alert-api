/* 
 * API AlertResource
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
#include <Wt/Json/Array>
#include <Wt/Json/Value>
Wt::Json::Array Wt::Json::Array::Empty;


using namespace std;

AlertResource::AlertResource(){
}

string AlertResource::getMediasValuesForAlert()
{
    string res = "";
    int idx = 0;
    try 
    {
        Wt::Dbo::Transaction transaction(*this->session);
        
         std::string queryString = "SELECT mev, ams FROM \"T_MEDIA_VALUE_MEV\" mev," 
                    " \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" ams "
                    "WHERE ams.\"AMS_ALE_ALE_ID\" = " + boost::lexical_cast<std::string > (this->vPathElements[1]) +
                    " AND ams.\"AMS_MEV_MEV_ID\" = mev.\"MEV_ID\" "
                    " AND mev.\"MEV_USR_USR_ID\" = " + boost::lexical_cast<std::string > (this->session->user().id());

        Wt::Dbo::Query
                <
                boost::tuple
                <
                Wt::Dbo::ptr<MediaValue>,
                Wt::Dbo::ptr<AlertMediaSpecialization>
                >
                , Wt::Dbo::DynamicBinding
                > resQuery = this->session->query
                <
                boost::tuple
                <
                Wt::Dbo::ptr<MediaValue>,
                Wt::Dbo::ptr<AlertMediaSpecialization>
                >, Wt::Dbo::DynamicBinding
                > (queryString);


        Wt::Dbo::collection<boost::tuple<
                Wt::Dbo::ptr<MediaValue>,
                Wt::Dbo::ptr<AlertMediaSpecialization> > > listTuples = resQuery.resultList();
        if (listTuples.size() > 0) 
        {
            if(listTuples.size() > 1)
            {
            res = "[\n";
            }
            for (Wt::Dbo::collection<boost::tuple<
                    Wt::Dbo::ptr<MediaValue>,
                    Wt::Dbo::ptr<AlertMediaSpecialization> > >::const_iterator i = listTuples.begin(); i != listTuples.end(); ++i) 
            {
                res += "{\n";
                res += "media_value :" + i->get<0>().modify()->toJSON();
                res += "media_specialization :" + i->get<1>().modify()->toJSON();
                res += "}\n";
                 ++idx;
                if(listTuples.size()-idx > 0)
                {
                    res.replace(res.size()-1, 1, "");
                    res += ",\n";
                }
                
            }
            if(listTuples.size() > 1)
            {
            res += "]";
            }
            this->statusCode = 200; 
        } 
        else 
        {
            this->statusCode = 404;
            res = "{\"message\":\"Alert not found\"}";
            return res;
        }
        transaction.commit();
    }                    
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}

string AlertResource::getTrackingAlertList()
{
    string res = "";
    int idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(*this->session);

        Wt::Dbo::ptr<User> user = this->session->find<User>().where("\"USR_ID\" = ?").bind(this->session->user().id());

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
            if(listTuples.size() > 1)
            {
            res += "[\n";
            }
            for (Wt::Dbo::collection<boost::tuple<
                 Wt::Dbo::ptr<Alert>,
                 Wt::Dbo::ptr<MediaValue>,
                 Wt::Dbo::ptr<AlertTracking> > >::const_iterator i = listTuples.begin(); i != listTuples.end(); ++i) 
            {
                res += "{\n";
                res +="\"alert\" :" + i->get<0>().modify()->toJSON();
                res +="\"media_value\" :" + i->get<1>().modify()->toJSON();
                res +="\"alert_tracking\" :" + i->get<2>().modify()->toJSON();
                res += "}\n";
                 ++idx;
                if(listTuples.size()-idx > 0)
                {
                    res.replace(res.size()-1, 1, "");
                    res += ",\n";
                }
              
                
                /*
                res += "{\n\"";
                        //<< "  \"send_date\" : \"" << i->get<2>().get()->sendDate << "\",\n\""
                res += "  \"alert_name\" : \"" + boost::lexical_cast<std::string > (i->get<0>().get()->name) + "\",\n\"";
                res += "  \"value\" : \"" + boost::lexical_cast<std::string > (i->get<1>().get()->value) + "\"\n\"" ;
                res += "}\n";*/
            }
            if(listTuples.size() > 1)
            {
            res += "]\n";
            }
            this->statusCode = 200;

        }
        else
        {
            this->statusCode = 404;
            res = "{\"message\":\"Tracking alert not found\"}";
            return res;
        }

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}


string AlertResource::getAlerts()
{
    string res = "";
    int idx = 0;
    try 
    {
        Wt::Dbo::Transaction transaction(*this->session);
        std::string queryString = "SELECT ale, acr, ava, inu FROM \"T_ALERT_ALE\" ale,"
                " \"T_ALERT_VALUE_AVA\" ava,"
                " \"T_ALERT_CRITERIA_ACR\" acr,"
                " \"T_INFORMATION_UNIT_UNT\" inu "
                " WHERE \"ALE_ID\" IN "
                "("
                "SELECT \"AMS_ALE_ALE_ID\" FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" WHERE \"AMS_MEV_MEV_ID\" IN "
                "("
                "SELECT \"MEV_ID\" FROM \"T_MEDIA_VALUE_MEV\" WHERE \"MEV_USR_USR_ID\"  = "
                + boost::lexical_cast<std::string > (this->session->user().id())
                + ""
                " )"
                ") "
                " AND ale.\"ALE_AVA_AVA_ID\" = ava.\"AVA_ID\" "
                " AND ale.\"ALE_DELETE\" IS NULL "
                " AND ava.\"AVA_ACR_ACR_ID\" = acr.\"ACR_ID\" "
                " AND inu.\"INU_ID\" = ava.\"INU_ID_INU_ID\" "
                " ORDER BY ale.\"ALE_ID\" ";
                
         
        
        Wt::Dbo::Query
                <
                boost::tuple
                <
                Wt::Dbo::ptr<Alert>,
                Wt::Dbo::ptr<AlertCriteria>,
                Wt::Dbo::ptr<AlertValue>,
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
                Wt::Dbo::ptr<InformationUnit>
                >, Wt::Dbo::DynamicBinding
                > (queryString);


        Wt::Dbo::collection<boost::tuple<
                Wt::Dbo::ptr<Alert>,
                Wt::Dbo::ptr<AlertCriteria>,
                Wt::Dbo::ptr<AlertValue>,
                Wt::Dbo::ptr<InformationUnit> > > listTuples = resQuery.resultList();
        if (listTuples.size() > 0) 
        {
            if(listTuples.size() > 1)
            {
            res = "[\n";
            }
            for (Wt::Dbo::collection<boost::tuple<
                    Wt::Dbo::ptr<Alert>,
                    Wt::Dbo::ptr<AlertCriteria>,
                    Wt::Dbo::ptr<AlertValue>,
                    Wt::Dbo::ptr<InformationUnit> > >::const_iterator i = listTuples.begin(); i != listTuples.end(); ++i) 
            {
                res += "{\n";
                res += "\"alert\" :" + i->get<0>().modify()->toJSON();
                res += "\"criteria\" :" + i->get<1>().modify()->toJSON();
                res += "\"alert_value\" :" + i->get<2>().modify()->toJSON();
                res += "\"information_unit\" :" + i->get<3>().modify()->toJSON();
                res += "}\n";
                
                  ++idx;
                if(listTuples.size()-idx > 0)
                {
                    res.replace(res.size()-1, 1, "");
                    res += ",\n";
                }
                
                /*res += "{\n\"";
                res +="  \"id\" : \"" + boost::lexical_cast<std::string > (i->get < 0 > ().id()) + "\",\n\"";
                res +="  \"alert_name\" : \"" + boost::lexical_cast<std::string > (i->get < 0 > ().get()->name) + "\",\n\"";
                res +="  \"criteria\" : \"" + boost::lexical_cast<std::string > (i->get < 1 > ().get()->name) + "\",\n\"";
                res +="  \"threshold_value\" : \"" + boost::lexical_cast<std::string > (i->get < 2 > ().get()->value) + "\",\n\"";
                res +="  \"unit\" : \"" + boost::lexical_cast<std::string > (i->get < 5 > ().get()->name) + "\",\n\"";
                res +="  \"key_value\" : \"" + boost::lexical_cast<std::string > (i->get < 2 > ().get()->keyValue.get()) + "\",\n\"";
                res +="  \"media\" : \"" + boost::lexical_cast<std::string > (i->get < 3 > ().get()->value) + "\",\n\"";
                res +="  \"snoozeDuration\" : \"" + boost::lexical_cast<std::string > (i->get < 4 > ().get()->snoozeDuration) + "\",\n\"";
                res +="}\n";*/

            }
            if(listTuples.size() > 1)
            {
            res += "]";
            }
            this->statusCode = 200; 
        } 
        else 
        {
            this->statusCode = 404;
            res = "{\"message\":\"User not found\"}";
            return res;
        }
        transaction.commit();
    }                    
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}

void AlertResource::processGetRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "" ;

    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        responseMsg = getAlerts();
    }
    else
    {
        if(!nextElement.compare("tracking"))
        {
            responseMsg =getTrackingAlertList() ;
        }
        else
        { 
                try
                {
                boost::lexical_cast<unsigned int>(nextElement);

                nextElement = getNextElementFromPath();

                if(!nextElement.compare("medias_values"))
                {
                    responseMsg = getMediasValuesForAlert();
                }
                else
                {
                    this->statusCode = 400;
                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                }
            }
            catch(boost::bad_lexical_cast &)
            {
                this->statusCode = 400;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }             
        }
    }
   
    response.setStatus(this->statusCode);
    response.out() << responseMsg;
    return;
}


string AlertResource::postAlert(string sRequest)
{
    string res = "";
    Wt::WString alertName,alertValue, threadSleep, keyVal, astId, seaId, 
                srcId, plgId, infValNum, inuId, acrId;

    int alertValueInt, threadSleepInt, astIdInt, seaIdInt, 
                srcIdInt, plgIdInt, infValNumInt, inuIdInt, acrIdInt;
    
    Wt::Json::Array& amsId = Wt::Json::Array::Empty;
    try
    {
        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);
        //descriptif
        alertName = result.get("name");
        alertValueInt = result.get("alert_value");
        alertValue = boost::lexical_cast<std::string>(alertValueInt);
        
        threadSleepInt = result.get("thread_sleep");
        threadSleep = boost::lexical_cast<std::string>(threadSleepInt);
        
        keyVal = result.get("key_value");   
        
        //Id asset à verifer
        astIdInt = result.get("ast_id");
        astId = boost::lexical_cast<std::string>(astIdInt);
        
        //information
        seaIdInt = result.get("sea_id");
        seaId = boost::lexical_cast<std::string>(seaIdInt);
        
        srcIdInt = result.get("src_id");
        srcId = boost::lexical_cast<std::string>(srcIdInt);
        
        plgIdInt = result.get("plg_id");
        plgId = boost::lexical_cast<std::string>(plgIdInt);
        
        infValNumInt = result.get("inf_value_num");
        infValNum = boost::lexical_cast<std::string>(infValNumInt);
        
        inuIdInt = result.get("inu_id");
        inuId = boost::lexical_cast<std::string>(inuIdInt);
        //operateur
        acrIdInt = result.get("acr_id");
        acrId = boost::lexical_cast<std::string>(acrIdInt);
        //media
        amsId = result.get("ams_id");
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
    catch(boost::bad_lexical_cast &)
    {
        this->statusCode = 400;
        res = "{\n\t\"message\":\"Bad Request\"\n}";
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

        for (Wt::Json::Array::const_iterator idx1 = amsId.begin() ; idx1 < amsId.end(); idx1++)
        {
            Wt::WString tmp1 = (*idx1).toString();
            Wt::Dbo::ptr<AlertMediaSpecialization> amsPtr = session->find<AlertMediaSpecialization>().where("\"AMS_ID\" = ?").bind(tmp1)
                                                                                                     .where("\"AMS_ALE_ALE_ID\" IS NULL");

            if (!infoPtr || !critPtr || !assetPtr || !amsPtr)
            {
                if(!infoPtr)
                {
                    cerr << "information not found" << endl;
                }
                if(!critPtr)
                {
                    cerr << "criteria not found" << endl;
                }
                if(!assetPtr)
                {
                    cerr << "asset not found" << endl;
                }
                if(!amsPtr)
                {
                    cerr << "alert_media_specialization not found or not available" << endl;
                }
                this->statusCode = 404;
                res = "{\"message\":\"Not found\"}";
                return res; 
            }
        }

/*
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
                Wt::log("debug") << " [AlertResource] " << " - " << " For ava list : " << (*i).id();
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
                    cerr << " [AlertResource] " << " - " << " For ale list : " << (*i).id() << "\n";
                    inString2 += boost::lexical_cast<std::string,long long>((*i).id()) + ",";
                    i->flush();
                }
                inString2.replace(inString2.size()-1, 1, "");
                inString2 += ")";

                std::string queryStr2 = " SELECT ams FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" ams WHERE "
                                        " \"AMS_ALE_ALE_ID\" IN" + inString2 +
                                        " AND \"AMS_ID\" = " + boost::lexical_cast<std::string>(amsId[1]);


                Wt::Dbo::Query<Wt::Dbo::ptr<AlertMediaSpecialization> > queryRes2 = session->query<Wt::Dbo::ptr<AlertMediaSpecialization> >(queryStr2);

                Wt::Dbo::collection<Wt::Dbo::ptr<AlertMediaSpecialization> > alertMS = queryRes2.resultList();

                if(alertMS.size() > 0)
                {

                this->statusCode = 400;
                res = "{\"message\":\"Alert already exists\"}";

                return res;
                }
            }

        }*/

        transaction.commit();

    }
    catch (Wt::Dbo::Exception e)
    {
        Wt::log("error") << "[AlertResource]" << e.what();
        this->statusCode = 503;
        res = "{\"message\":\"Service Unavailable.\"}";
        return res;
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

        for (Wt::Json::Array::const_iterator idx2 = amsId.begin() ; idx2 < amsId.end(); idx2++)
        {
            Wt::WString tmp = (*idx2).toString();
            Wt::Dbo::ptr<AlertMediaSpecialization> amsPtr = session->find<AlertMediaSpecialization>().where("\"AMS_ID\" = ?").bind(tmp);
            amsPtr.modify()->alert = alePtr;
        }
        transaction.commit();

        this->statusCode = 200;
        res = "{\"message\":\"Alert added\"}";

    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\"message\":\"Service Unavailable\"}";
    }
    
    return res;
}


void AlertResource::processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "", sRequest = "";

    sRequest = request2string(request);
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        responseMsg = postAlert(sRequest);
    }
    else
    {
        //// SUPPRIMER     
        try
        {
            boost::lexical_cast<unsigned int>(nextElement);

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
        }
        catch(boost::bad_lexical_cast &)
        {
            this->statusCode = 400;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
        //// SUPPRIMER
        
       // this->statusCode = 400;
       // responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }

    response.setStatus(this->statusCode);
    response.out() << responseMsg;
    return ;
}

void AlertResource::processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}

void AlertResource::processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}

string AlertResource::deleteAlert()
{
    string res = "";
    try 
    {

        Wt::Dbo::Transaction transaction(*this->session);
        Wt::Dbo::ptr<Alert> alertPtr = session->find<Alert>().where("\"ALE_ID\" = ?").bind(boost::lexical_cast<int>(this->vPathElements[1])); 

        std::string executeString1 = " SELECT astale FROM \"TJ_AST_ALE\" astale" 
                                    " WHERE astale.\"T_ALERT_ALE_ALE_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[1]) + "FOR UPDATE";
        std::string executeString1bis = " DELETE FROM \"TJ_AST_ALE\" " 
                                        " WHERE \"TJ_AST_ALE\".\"T_ALERT_ALE_ALE_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[1]);
        session->execute(executeString1);
        session->execute(executeString1bis);

        std::string executeString2 = "SELECT ams FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" ams " 
                                     " WHERE \"AMS_ALE_ALE_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[1]) + "FOR UPDATE";
        std::string executeString2bis =  " DELETE FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" "
                                    " WHERE \"AMS_ALE_ALE_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[1]);


        session->execute(executeString2);
        session->execute(executeString2bis);

        alertPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
        transaction.commit();
        std::cerr<<"after commit" << std::endl;

        this->statusCode = 204;
        res = "";//"{\"message\":\"Alert deleted\"}";

    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}

void AlertResource::processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "", sRequest = "";

    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->statusCode = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}"; 
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned int>(nextElement);

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
        }
        catch(boost::bad_lexical_cast &)
        {
            this->statusCode = 400;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(this->statusCode);
    response.out() << responseMsg;
    return;
}


void AlertResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);

    return;
}


AlertResource::~AlertResource()
{
    beingDeleted();
}