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
#include "itooki/ItookiSMSSender.h"

using namespace std;

AlertResource::AlertResource()
{
}

unsigned short AlertResource::getMediasValuesForAlert(string &responseMsg) const
{
    unsigned short res = 500;
    int idx = 0;
    try 
    {
        Wt::Dbo::Transaction transaction(*this->session);

        string queryString = "SELECT mev, ams FROM \"T_MEDIA_VALUE_MEV\" mev,"
                " \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" ams "
                "WHERE ams.\"AMS_ALE_ALE_ID\" = " + boost::lexical_cast<string > (this->vPathElements[1]) +
                " AND ams.\"AMS_MEV_MEV_ID\" = mev.\"MEV_ID\" "
                " AND mev.\"MEV_USR_USR_ID\" = " + boost::lexical_cast<string > (this->session->user().id());

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

unsigned short AlertResource::getTrackingAlertList(string &responseMsg) const
{
    unsigned short res = 500;
    int idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(*this->session);

        Wt::Dbo::ptr<User> user = this->session->find<User>().where("\"USR_ID\" = ?").bind(this->session->user().id());

        string queryString = "SELECT ale, mev, atr FROM \"T_ALERT_TRACKING_ATR\" atr, \"T_ALERT_ALE\" ale , \"T_MEDIA_VALUE_MEV\" mev "
            " WHERE atr.\"ATR_ALE_ALE_ID\" = ale.\"ALE_ID\" "
            " AND ale.\"ALE_DELETE\" IS NULL "
            " AND atr.\"ATR_MEV_MEV_ID\" = mev.\"MEV_ID\" "
            " AND mev.\"MEV_USR_USR_ID\" IN"
            "("
                "SELECT \"T_USER_USR_USR_ID\" FROM \"TJ_USR_ORG\" WHERE \"T_ORGANIZATION_ORG_ORG_ID\" = " 
                + boost::lexical_cast<string>(user.get()->currentOrganization.id()) + ""
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

unsigned short AlertResource::getAlerts(string &responseMsg) const
{
    unsigned short res = 500;
    int idx = 0;
    try 
    {
        Wt::Dbo::Transaction transaction(*this->session);
        string queryString = "SELECT ale, acr, ava, inu FROM \"T_ALERT_ALE\" ale,"
                " \"T_ALERT_VALUE_AVA\" ava,"
                " \"T_ALERT_CRITERIA_ACR\" acr,"
                " \"T_INFORMATION_UNIT_UNT\" inu "
                " WHERE \"ALE_ID\" IN "
                "("
                "SELECT \"AMS_ALE_ALE_ID\" FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" WHERE \"AMS_MEV_MEV_ID\" IN "
                "("
                "SELECT \"MEV_ID\" FROM \"T_MEDIA_VALUE_MEV\" WHERE \"MEV_USR_USR_ID\"  = "
                + boost::lexical_cast<string > (this->session->user().id())
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
    string responseMsg = "", nextElement = "" ;

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


unsigned short AlertResource::postAlert(string &responseMsg, const string &sRequest)
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
        alertValue = boost::lexical_cast<string>(alertValueInt);
        
        threadSleepInt = result.get("thread_sleep");
        threadSleep = boost::lexical_cast<string>(threadSleepInt);
        
        keyVal = result.get("key_value");   
        
        //Id asset à verifer
        astIdInt = result.get("ast_id");
        astId = boost::lexical_cast<string>(astIdInt);
        
        //information
        seaIdInt = result.get("sea_id");
        seaId = boost::lexical_cast<string>(seaIdInt);
        
        srcIdInt = result.get("src_id");
        srcId = boost::lexical_cast<string>(srcIdInt);
        
        plgIdInt = result.get("plg_id");
        plgId = boost::lexical_cast<string>(plgIdInt);
        
        infValNumInt = result.get("inf_value_num");
        infValNum = boost::lexical_cast<string>(infValNumInt);
        
        inuIdInt = result.get("inu_id");
        inuId = boost::lexical_cast<string>(inuIdInt);
        //operateur
        acrIdInt = result.get("acr_id");
        acrId = boost::lexical_cast<string>(acrIdInt);
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
            string inString = "(";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<AlertValue> >::const_iterator i = avaPtrCollec.begin(); i != avaPtrCollec.end(); i++) 
            {
                Wt::log("debug") << " [AlertResource] " << " - " << " For ava list : " << (*i).id();
                inString += boost::lexical_cast<string,long long>((*i).id()) + ",";
                i->flush();
            }
            inString.replace(inString.size()-1, 1, "");
            inString += ")";


            string queryStr = "SELECT ale FROM \"T_ALERT_ALE\" ale WHERE "
                                    " \"ALE_ID\" IN"
                                    "("
                                        "SELECT \"T_ALERT_ALE_ALE_ID\" FROM \"TJ_AST_ALE\" WHERE \"T_ASSET_AST_AST_ID\" = " 
                                        + boost::lexical_cast<string>(astId) +
                                    ")"
                                    "AND ale.\"ALE_DELETE\" IS NULL "
                                    "AND \"ALE_AVA_AVA_ID\" IN" + inString;

            Wt::Dbo::Query<Wt::Dbo::ptr<Alert> > queryRes = session->query<Wt::Dbo::ptr<Alert> >(queryStr);

            Wt::Dbo::collection<Wt::Dbo::ptr<Alert> > alerts = queryRes.resultList();

            if (alerts.size() > 0)
            {

                string inString2 = "(";
                for (Wt::Dbo::collection<Wt::Dbo::ptr<Alert> >::const_iterator i = alerts.begin(); i != alerts.end(); i++) 
                {
                    cerr << " [AlertResource] " << " - " << " For ale list : " << (*i).id() << "\n";
                    inString2 += boost::lexical_cast<string,long long>((*i).id()) + ",";
                    i->flush();
                }
                inString2.replace(inString2.size()-1, 1, "");
                inString2 += ")";

                string queryStr2 = " SELECT ams FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" ams WHERE "
                                        " \"AMS_ALE_ALE_ID\" IN" + inString2 +
                                        " AND \"AMS_ID\" = " + boost::lexical_cast<string>(amsId[1]);


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

int AlertResource::sendMAIL
(
 Wt::Dbo::collection<Wt::Dbo::ptr<InformationValue>> ivaPtrCollection,
 Wt::Dbo::ptr<Alert> alertPtr,
 Wt::Dbo::ptr<AlertTracking> alertTrackingPtr,
 Wt::Dbo::ptr<AlertMediaSpecialization> amsPtr,
 bool overSMSQuota
)
{    
    Wt::WString mailRecipient;
    const Wt::WString mailRecipientName = amsPtr->mediaValue->user->firstName + " " + amsPtr->mediaValue->user->lastName ;
    Wt::WString mailBody = "";
    const Wt::WDateTime now = Wt::WDateTime::currentDateTime(); //for setting the send date of the alert
    const Wt::WString unit = alertPtr->alertValue->information->pk.unit->name;
    Wt::Mail::Message mailMessage;
    Wt::Mail::Client mailClient;

    // Normal case
    if (overSMSQuota == 0)
    {
        mailRecipient = amsPtr->mediaValue->value;
    }
    else if (overSMSQuota == 1)
    {
        mailRecipient = amsPtr->mediaValue->user->eMail;

        mailBody += "MAIL sent instead of SMS (quota = 0) <br />";
    }

    mailBody += "Alert name : " + alertPtr->name + "<br />";

    //TODO: à revoir pour les alertes complexes !!
    for (Wt::Dbo::collection<Wt::Dbo::ptr<InformationValue>>::const_iterator i = ivaPtrCollection.begin(); i != ivaPtrCollection.end(); ++i)
    {
        mailBody += "Asset name : " +  i->get()->asset->name + "<br />";
        if (alertPtr->alertValue->keyValue.is_initialized() && alertPtr->alertValue->keyValue.get() != "N/A")
        {
           mailBody += "Key : " + alertPtr->alertValue->keyValue.get() + "<br />";
        }
        mailBody += "Received value : " + i->get()->value + " " + unit + "<br />"
                + "Criteria : " + alertPtr->alertValue->alertCriteria->name + "<br />"
                + "Expected value : " + alertPtr->alertValue->value + " " + unit + "<br />"
                + "Time : " + i->get()->creationDate.toString() + "<br />";
    }
    mailBody += "Check it on https://alert.echoes-tech.com";
        
    Wt::log("info") << " [Alert Resource] " << mailBody; 
    
    mailMessage.setFrom(Wt::Mail::Mailbox("alert@echoes-tech.com", "ECHOES Alert"));
    mailMessage.addRecipient(Wt::Mail::To, Wt::Mail::Mailbox(mailRecipient.toUTF8(), mailRecipientName));
    mailMessage.setSubject("[ECHOES Alert] " + alertPtr->name);
    mailMessage.addHtmlBody(mailBody);
    mailClient.connect("hermes.gayuxweb.fr");
    mailClient.send(mailMessage);

    Wt::log("info") << " [Class:AlertSender] " << "insert date of last send in db : " << now.toString();
    amsPtr.modify()->lastSend = now;

    alertTrackingPtr.modify()->sendDate = now;
}

int AlertResource::sendSMS
(
 Wt::Dbo::collection<Wt::Dbo::ptr<InformationValue>> ivaPtrCollection,
 Wt::Dbo::ptr<Alert> alertPtr,
 Wt::Dbo::ptr<AlertTracking> alertTrackingPtr,
 Wt::Dbo::ptr<AlertMediaSpecialization> amsPtr
)
{
    const Wt::WString unit = alertPtr->alertValue->information->pk.unit->name;
    const Wt::WDateTime now = Wt::WDateTime::currentDateTime(); //for setting the send date of the alert

    Wt::WString sms = "New alert about : " + alertPtr->name;
    
    //TODO: à revoir pour les alertes complexes !!
    for (Wt::Dbo::collection<Wt::Dbo::ptr<InformationValue>>::const_iterator i = ivaPtrCollection.begin(); i != ivaPtrCollection.end(); ++i)
    {
        sms += " on " + i->get()->asset->name;

        //we check if there is a key and get it if it's the case to put in the sms
    //    if (!boost::lexical_cast<Wt::WString,boost::optional<Wt::WString> >(alertPtr.get()->alertValue.get()->keyValue).empty())
        if (alertPtr->alertValue->keyValue.is_initialized() && alertPtr->alertValue->keyValue.get() != "N/A")
        {
           sms += " for : " + alertPtr->alertValue->keyValue.get();
        }

        sms += " Received information : " + i->get()->value + " " + unit
                + " expected : " + alertPtr->alertValue->value + " " + unit
                + " at : " + i->get()->creationDate.toString();
    }

    Wt::log("info") << " [Alert Resource] New SMS for " << amsPtr->mediaValue->value << " : " << sms;
    
    
    ItookiSMSSender *itookiSMSSender =  new ItookiSMSSender(amsPtr->mediaValue->value.toUTF8(), sms.toUTF8(), this);
    itookiSMSSender->setAlertTrackingPtr(alertTrackingPtr);
    if(!itookiSMSSender->send())
    {
        amsPtr.modify()->lastSend = now;
    }
    
    delete itookiSMSSender;
}

unsigned short AlertResource::postAlertTracking(string &responseMsg, const string &sRequest)
{
    unsigned short res = 500;
    string ivaIDWhereString = "";
    Wt::Dbo::collection<Wt::Dbo::ptr<InformationValue>> ivaPtrCollection;

    if(sRequest.compare(""))
    {    
        try
        {
            unsigned idx = 1;

            Wt::Json::Value result;

            Wt::Json::parse(sRequest, result);
            
            Wt::Json::Array array = result;

            for (Wt::Json::Array::const_iterator i = array.begin(); i != array.end(); ++i)
            {
                Wt::Json::Object tmp = *i;
                long long id = tmp.get("id");
                
                ivaIDWhereString += "\"IVA_ID\" = " + boost::lexical_cast<string>(id);
                if(idx++ < array.size())
                {
                    ivaIDWhereString += " OR";
                }
            }

        }
        catch (Wt::Json::ParseError const& e)
        {
            Wt::log("warning") << "[Alert Ressource] Problems parsing JSON: " << sRequest;
            res = 400;
            responseMsg = "{\n\t\"message\": \"Problems parsing JSON\"\n}";
        }
        catch (Wt::Json::TypeException const& e)
        {
            Wt::log("warning") << "[Alert Ressource] Problems parsing JSON: " << sRequest;
            res = 400;
            responseMsg = "{\n\t\"message\": \"Problems parsing JSON\"\n}";
        }
    }
    else
    {
        res = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }
    
    if(!responseMsg.compare(""))
    {
        try
        {
            Wt::Dbo::Transaction transaction(*session);

            //TODO: check if this alert is set for the current user organisation
            Wt::Dbo::ptr<Alert> alertPtr = session->find<Alert>()
                    .where("\"ALE_ID\" = ?").bind(this->vPathElements[1])
                    .where("\"ALE_DELETE\" IS NULL");

            if (Utils::checkId<Alert>(alertPtr))
            {
                alertPtr.modify()->lastAttempt = Wt::WDateTime::currentDateTime();
                
                //TODO: verifier si les IVA correspondent bien aux INF de l'alerte
                ivaPtrCollection = session->find<InformationValue>()
                    .where(ivaIDWhereString)
                    .where("\"IVA_DELETE\" IS NULL");
                
                Wt::WDateTime now = Wt::WDateTime::currentDateTime();

                for (Wt::Dbo::collection<Wt::Dbo::ptr<AlertMediaSpecialization>>::const_iterator i = alertPtr->alertMediaSpecializations.begin(); i != alertPtr->alertMediaSpecializations.end(); ++i)
                {
                    // it is the first time we send the alert there is no last send date filled
                    // or date.now() - last_send = nb_secs then, if nb_secs >= snooze of the media, we send the alert
                    if (!i->get()->lastSend.isValid() || i->get()->lastSend.secsTo(now) >= i->get()->snoozeDuration)
                    {
                        AlertTracking *newAlertTracking = new AlertTracking();

                        newAlertTracking->alert = alertPtr;
                        newAlertTracking->mediaValue = i->get()->mediaValue;

                        // WARNING : SendDate must be set by the API when the alert was sent, not before
                        newAlertTracking->sendDate = *(new Wt::WDateTime());

                        Wt::Dbo::ptr<AlertTracking> alertTrackingPtr = session->add<AlertTracking>(newAlertTracking);
                        alertTrackingPtr.flush();

                        Wt::log("info") << " [Alert Ressource] " << "Alert tracking number creation : " << alertTrackingPtr.id();

                        Wt::log("debug") << " [Alert Ressource] " << "snooze = " << i->get()->snoozeDuration;
                        switch (i->get()->mediaValue->media.id())
                        {
                            case sms:
                            {
                                Wt::log("info") << " [Alert Ressource] " << "Media value SMS choosed for the alert : " << alertPtr->name;

                                // Verifying the quota of sms
                                Wt::Dbo::ptr<OptionValue> optionValuePtr = session->find<OptionValue>()
                                        .where("\"OPT_ID_OPT_ID\" = ?").bind(quotasms)
                                        .where("\"ORG_ID_ORG_ID\" = ?").bind(i->get()->mediaValue->user->currentOrganization.id())
                                        .limit(1);

                                try
                                {
                                    int smsQuota = boost::lexical_cast<int>(optionValuePtr->value); 
                                    if (smsQuota == 0)
                                    {

                                        Wt::log("info") << " [Alert Ressource] " << "SMS quota 0 for alert : " <<  alertPtr->name;
                                        Wt::log("info") << " [Alert Ressource] " << "Sending e-mail instead." ;

                                        sendMAIL(ivaPtrCollection, alertPtr, alertTrackingPtr, *i, true);
                                    }
                                    else
                                    {
                                        Wt::log("debug") << " [Alert Ressource] " << "We send a SMS, quota : "<< smsQuota;
                                        optionValuePtr.modify()->value = boost::lexical_cast<string>(smsQuota - 1);
                                        optionValuePtr.flush();                        
                                        sendSMS(ivaPtrCollection,alertPtr, alertTrackingPtr, *i); 
                                    }
                                }
                                catch(boost::bad_lexical_cast &)
                                {
                                    res = 503;
                                    responseMsg = "{\n\t\"message\": \"Service Unavailable\"\n}";
                                }
                                break;
                            }
                            case mail:
                                Wt::log("info") << " [Alert Ressource] " << "Media value MAIL choosed for the alert : " << alertPtr->name;              
                                sendMAIL(ivaPtrCollection, alertPtr, alertTrackingPtr, *i);
                                break;
                            default:
                                break;        
                        }
                    }
                    else
                    {
                        Wt::log("debug") << " [Class:AlertSender] "
                                << "Last time we send the alert : " << alertPtr->name
                                << "was : " << i->get()->lastSend.toString()
                                << "the snooze for the media " << i->get()->mediaValue->media->name
                                << " is : " << i->get()->snoozeDuration << "secs,  it's not the time to send the alert";  
                    }
                }
                res = 201;
            }
            else 
            {
                responseMsg = "{\n\t\"message\":\"Alert not found\"\n}";
                res = 404;
            }

            transaction.commit();
        }
        catch (Wt::Dbo::Exception const& e) 
        {
            Wt::log("error") << e.what();
            res = 503;
            responseMsg = "{\"message\":\"Service Unavailable\"}";
        }
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
        this->statusCode = postAlert(responseMsg, sRequest);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned int>(nextElement);

            nextElement = getNextElementFromPath();
//            TO Test whithout DELETE Method
//            if(!nextElement.compare(""))
//            {
//                this->statusCode = deleteAlert(responseMsg);
//            }
            if(!nextElement.compare("trackings"))
            {
                this->statusCode = postAlertTracking(responseMsg, sRequest);
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

unsigned short AlertResource::deleteAlert(string &responseMsg)
{
    unsigned short res = 500;
    try 
    {

        Wt::Dbo::Transaction transaction(*this->session);
        Wt::Dbo::ptr<Alert> alertPtr = session->find<Alert>().where("\"ALE_ID\" = ?").bind(boost::lexical_cast<int>(this->vPathElements[1])); 
        Wt::Dbo::ptr<AlertValue> avaPtr = session->find<AlertValue>().where("\"AVA_ID\" = ?").bind(alertPtr.get()->alertValue.id());

        string executeString1 = " SELECT astale FROM \"TJ_AST_ALE\" astale" 
                                    " WHERE astale.\"T_ALERT_ALE_ALE_ID\" = " + boost::lexical_cast<string>(this->vPathElements[1]) + "FOR UPDATE";
        string executeString1bis = " DELETE FROM \"TJ_AST_ALE\" " 
                                        " WHERE \"TJ_AST_ALE\".\"T_ALERT_ALE_ALE_ID\" = " + boost::lexical_cast<string>(this->vPathElements[1]);
        session->execute(executeString1);
        session->execute(executeString1bis);

        string executeString2 = "SELECT ams FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" ams " 
                                     " WHERE \"AMS_ALE_ALE_ID\" = " + boost::lexical_cast<string>(this->vPathElements[1]) + "FOR UPDATE";
        string executeString2bis =  " DELETE FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" "
                                    " WHERE \"AMS_ALE_ALE_ID\" = " + boost::lexical_cast<string>(this->vPathElements[1]);


        session->execute(executeString2);
        session->execute(executeString2bis);

        avaPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
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