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

AlertResource::AlertResource(){
}

unsigned short AlertResource::getMediasValuesForAlert(std::string &responseMsg) const
{
    unsigned short res = 500;
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
            responseMsg = "[\n";
            }
            for (Wt::Dbo::collection<boost::tuple<
                    Wt::Dbo::ptr<MediaValue>,
                    Wt::Dbo::ptr<AlertMediaSpecialization> > >::const_iterator i = listTuples.begin(); i != listTuples.end(); ++i) 
            {
                i->get<0>().modify()->setId(i->get<0>().id());
                i->get<1>().modify()->setId(i->get<1>().id());
                responseMsg += "{\n";
                responseMsg += "media_value :" + i->get<0>().modify()->toJSON();
                responseMsg.replace(responseMsg.size()-1, 1, ",\n");
                responseMsg += "media_specialization :" + i->get<1>().modify()->toJSON();
                responseMsg += "}\n";
                 ++idx;
                if(listTuples.size()-idx > 0)
                {
                    responseMsg.replace(responseMsg.size()-1, 1, "");
                    responseMsg += ",\n";
                }
                
            }
            if(listTuples.size() > 1)
            {
            responseMsg += "]";
            }
            res = 200; 
        } 
        else 
        {
            res = 404;
            responseMsg = "{\"message\":\"Alert not found\"}";
            return res;
        }
        transaction.commit();
    }                    
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}

unsigned short AlertResource::getTrackingAlertList(std::string &responseMsg) const
{
    unsigned short res = 500;
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
            responseMsg += "[\n";
            }
            for (Wt::Dbo::collection<boost::tuple<
                 Wt::Dbo::ptr<Alert>,
                 Wt::Dbo::ptr<MediaValue>,
                 Wt::Dbo::ptr<AlertTracking> > >::const_iterator i = listTuples.begin(); i != listTuples.end(); ++i) 
            {
                i->get<0>().modify()->setId(i->get<0>().id());
                i->get<1>().modify()->setId(i->get<1>().id());
                i->get<2>().modify()->setId(i->get<2>().id());
                
                responseMsg += "{\n";
                responseMsg +="\"alert\" :" + i->get<0>().modify()->toJSON();
                responseMsg.replace(responseMsg.size()-1, 1, ",\n");
                responseMsg +="\"media_value\" :" + i->get<1>().modify()->toJSON();
                responseMsg.replace(responseMsg.size()-1, 1, ",\n");
                responseMsg +="\"alert_tracking\" :" + i->get<2>().modify()->toJSON();
                responseMsg += "}\n";
                 ++idx;
                if(listTuples.size()-idx > 0)
                {
                    responseMsg.replace(responseMsg.size()-1, 1, "");
                    responseMsg += ",\n";
                }
            }
            if(listTuples.size() > 1)
            {
            responseMsg += "]\n";
            }
            res = 200;

        }
        else
        {
            res = 404;
            responseMsg = "{\"message\":\"Tracking alert not found\"}";
            return res;
        }

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}


unsigned short AlertResource::getAlerts(std::string &responseMsg) const
{
    unsigned short res = 500;
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
            responseMsg = "[\n";
            }
            for (Wt::Dbo::collection<boost::tuple<
                    Wt::Dbo::ptr<Alert>,
                    Wt::Dbo::ptr<AlertCriteria>,
                    Wt::Dbo::ptr<AlertValue>,
                    Wt::Dbo::ptr<InformationUnit> > >::const_iterator i = listTuples.begin(); i != listTuples.end(); ++i) 
            {
                i->get<0>().modify()->setId(i->get<0>().id());
                i->get<1>().modify()->setId(i->get<1>().id());
                i->get<2>().modify()->setId(i->get<2>().id());
                i->get<3>().modify()->setId(i->get<3>().id());
                responseMsg += "{\n";
                responseMsg += "\"alert\" :" + i->get<0>().modify()->toJSON();
                responseMsg.replace(responseMsg.size()-1, 1, ",\n");
                responseMsg += "\"criteria\" :" + i->get<1>().modify()->toJSON();
                responseMsg.replace(responseMsg.size()-1, 1, ",\n");
                responseMsg += "\"alert_value\" :" + i->get<2>().modify()->toJSON();
                responseMsg.replace(responseMsg.size()-1, 1, ",\n");
                responseMsg += "\"information_unit\" :" + i->get<3>().modify()->toJSON();
                responseMsg += "}\n";
                
                  ++idx;
                if(listTuples.size()-idx > 0)
                {
                    responseMsg.replace(responseMsg.size()-1, 1, "");
                    responseMsg += ",\n";
                }

            }
            if(listTuples.size() > 1)
            {
            responseMsg += "]";
            }
            res = 200; 
        } 
        else 
        {
            res = 404;
            responseMsg = "{\"message\":\"User not found\"}";
            return res;
        }
        transaction.commit();
    }                    
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}

void AlertResource::processGetRequest(Wt::Http::Response &response)
{
    std::string responseMsg = "", nextElement = "" ;

    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->statusCode = getAlerts(responseMsg);
    }
    else
    {
        if(!nextElement.compare("tracking"))
        {
            this->statusCode = getTrackingAlertList(responseMsg) ;
        }
        else
        { 
            try
            {
                boost::lexical_cast<unsigned int>(nextElement);

                nextElement = getNextElementFromPath();

                if(!nextElement.compare("medias_values"))
                {
                    this->statusCode = getMediasValuesForAlert(responseMsg);
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


unsigned short AlertResource::postAlert(std::string &responseMsg, const std::string &sRequest)
{
    unsigned short res = 500;
    Wt::WString alertName,alertValue, threadSleep, keyVal, astId, seaId, 
                srcId, plgId, infValNum, inuId, acrId;

    Wt::Json::Array& amsId = Wt::Json::Array::Empty;
    try
    {
        int alertValueInt, threadSleepInt, astIdInt, seaIdInt, 
        srcIdInt, plgIdInt, infValNumInt, inuIdInt, acrIdInt;
        
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
    catch(boost::bad_lexical_cast &)
    {
        res = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        return res;
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
                    std::cerr << "information not found" << std::endl;
                }
                if(!critPtr)
                {
                    std::cerr << "criteria not found" << std::endl;
                }
                if(!assetPtr)
                {
                    std::cerr << "asset not found" << std::endl;
                }
                if(!amsPtr)
                {
                    std::cerr << "alert_media_specialization not found or not available" << std::endl;
                }
                res = 404;
                responseMsg = "{\"message\":\"Not found\"}";
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
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable.\"}";
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
        alePtr.flush();
        alePtr.modify()->setId(alePtr.id());
        responseMsg = alePtr.modify()->toJSON();
        transaction.commit();

        res = 200;

    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    
    return res;
}


void AlertResource::processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    std::string responseMsg = "", nextElement = "", sRequest = "";

    sRequest = request2string(request);
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->statusCode = postAlert(responseMsg, sRequest);
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
                this->statusCode = deleteAlert(responseMsg);
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

unsigned short AlertResource::deleteAlert(std::string &responseMsg)
{
    unsigned short res = 500;
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

        res = 204;
        responseMsg = "";

    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}

void AlertResource::processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    std::string responseMsg = "", nextElement = "", sRequest = "";

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
                this->statusCode = deleteAlert(responseMsg);
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