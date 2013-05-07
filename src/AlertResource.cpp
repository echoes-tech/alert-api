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

using namespace std;

AlertResource::AlertResource() : PublicApiResource::PublicApiResource()
{
}

AlertResource::AlertResource(const AlertResource& orig) : PublicApiResource::PublicApiResource(orig)
{
}

AlertResource::~AlertResource()
{
}

unsigned short AlertResource::getRecipientsForAlert(string &responseMsg) const
{
    unsigned short res = 500;
    unsigned idx = 0;
    try 
    {
        Wt::Dbo::Transaction transaction(*this->session);
        
         string queryString = "SELECT mev, ams FROM \"T_MEDIA_VALUE_MEV\" mev," 
                    " \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" ams "
                    "WHERE ams.\"AMS_ALE_ALE_ID\" = " + this->vPathElements[1] +
                    " AND ams.\"AMS_MEV_MEV_ID\" = mev.\"MEV_ID\" "
                    " AND mev.\"MEV_USR_USR_ID\" IN" 
                 "( Select \"T_USER_USR_USR_ID\" "
                 "FROM \"TJ_USR_ORG\" "
                "WHERE \"T_ORGANIZATION_ORG_ORG_ID\" = ?" + boost::lexical_cast<string > (this->session->user()->currentOrganization.id()) +
                 ")";

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
            responseMsg = "[";
            for (Wt::Dbo::collection<boost::tuple<
                    Wt::Dbo::ptr<MediaValue>,
                    Wt::Dbo::ptr<AlertMediaSpecialization> > >::const_iterator i = listTuples.begin(); i != listTuples.end(); ++i) 
            {
                i->get<0>().modify()->setId(i->get<0>().id());
                i->get<1>().modify()->setId(i->get<1>().id());
                responseMsg += "\n{\n";
                responseMsg += "media_value :" + i->get<0>().modify()->toJSON();
                responseMsg += ",\nmedia_specialization :" + i->get<1>().modify()->toJSON();
                responseMsg += "\n}";
                 ++idx;
                if(listTuples.size()-idx > 0)
                {
                    responseMsg += ",\n";
                }
                
            }
            responseMsg += "]";
            res = 200; 
        } 
        else 
        {
            res = 404;
            responseMsg = "{\"message\":\"Alert not found\"}";
        }
        transaction.commit();
    }                    
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;
}

unsigned short AlertResource::getTrackingAlertList(string &responseMsg) const
{
    responseMsg="";
    unsigned short res = 500;
    int idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(*this->session);

        string queryString = "SELECT ale, mev, atr FROM \"T_ALERT_TRACKING_ATR\" atr, \"T_ALERT_ALE\" ale , \"T_MEDIA_VALUE_MEV\" mev "
            " WHERE atr.\"ATR_ALE_ALE_ID\" = ale.\"ALE_ID\" "
            " AND ale.\"ALE_DELETE\" IS NULL "
            " AND atr.\"ATR_MEV_MEV_ID\" = mev.\"MEV_ID\" "
            " AND mev.\"MEV_USR_USR_ID\" IN"
            "("
                "SELECT \"T_USER_USR_USR_ID\" FROM \"TJ_USR_ORG\" WHERE \"T_ORGANIZATION_ORG_ORG_ID\" = " 
                + boost::lexical_cast<string>(this->session->user()->currentOrganization.id()) + ""
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
            responseMsg = "[";
            for (Wt::Dbo::collection<boost::tuple<
                 Wt::Dbo::ptr<Alert>,
                 Wt::Dbo::ptr<MediaValue>,
                 Wt::Dbo::ptr<AlertTracking> > >::const_iterator i = listTuples.begin(); i != listTuples.end(); ++i) 
            {
                i->get<0>().modify()->setId(i->get<0>().id());
                i->get<1>().modify()->setId(i->get<1>().id());
                i->get<2>().modify()->setId(i->get<2>().id());
                
                responseMsg += "\n{\n";
                responseMsg +="\"alert\" :" + i->get<0>().modify()->toJSON();
                responseMsg +=",\n\"media_value\" :" + i->get<1>().modify()->toJSON();
                responseMsg +=",\n\"alert_tracking\" :" + i->get<2>().modify()->toJSON();
                responseMsg += "\n}";
                 ++idx;
                if(listTuples.size()-idx > 0)
                {
                    responseMsg += ",\n";
                }
            }
            responseMsg += "\n]\n";
            res = 200;

        }
        else
        {
            res = 404;
            responseMsg = "{\"message\":\"Tracking alert not found\"}";
        }

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;
}

unsigned short AlertResource::getAlerts(string &responseMsg) const
{
    unsigned short res = 500;
    int idx = 0;
    try 
    {
        Wt::Dbo::Transaction transaction(*session);
        string queryString = "SELECT ale, acr, ava, inu FROM \"T_ALERT_ALE\" ale,"
                " \"T_ALERT_VALUE_AVA\" ava,"
                " \"T_ALERT_CRITERIA_ACR\" acr,"
                " \"T_INFORMATION_UNIT_UNT\" inu "
                " WHERE \"ALE_ID\" IN "
                " ("
                " SELECT \"AMS_ALE_ALE_ID\" FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" WHERE \"AMS_MEV_MEV_ID\" IN "
                " ("
                " SELECT \"MEV_ID\" FROM \"T_MEDIA_VALUE_MEV\" WHERE \"MEV_USR_USR_ID\"  IN"
                " ( SELECT \"T_USER_USR_USR_ID\" "
                " FROM \"TJ_USR_ORG\" "
                " WHERE \"T_ORGANIZATION_ORG_ORG_ID\" = " + boost::lexical_cast<string > (this->session->user()->currentOrganization.id()) +
                " )"
                " )"
                " ) "
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
                > resQuery = session->query
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
            responseMsg = "[";
            for (Wt::Dbo::collection<boost::tuple<
                    Wt::Dbo::ptr<Alert>,
                    Wt::Dbo::ptr<AlertCriteria>,
                    Wt::Dbo::ptr<AlertValue>,
                    Wt::Dbo::ptr<InformationUnit> > >::const_iterator i = listTuples.begin(); i != listTuples.end(); ++i) 
            {        
                Wt::Dbo::ptr<AlertValue> aleVal(i->get<2>()); 
                Wt::Dbo::ptr<Alert> ale(i->get<0>()); 
                Wt::Dbo::ptr<AlertCriteria> aleCrit(i->get<1>());
                Wt::Dbo::ptr<InformationUnit> info(i->get<3>()); 
                
                ale.modify()->setId(ale.id());
                aleCrit.modify()->setId(aleCrit.id());
                aleVal.modify()->setId(aleVal.id());      
                info.modify()->setId(info.id());
                responseMsg += "\n{\n";
                responseMsg += "\"alert\" :" + ale.modify()->toJSON();
                responseMsg += ",\n\"criteria\" :" + aleCrit.modify()->toJSON();
                responseMsg += ",\n\"alert_value\" :" + aleVal.modify()->toJSON();
                responseMsg += ",\n\"information_unit\" :" + info.modify()->toJSON();
                responseMsg += "\n}";


                ++idx;
                if(listTuples.size()-idx > 0)
                {
                    responseMsg += ",\n";
                }

            }
            responseMsg += "\n]";
            res = 200; 
        } 
        else 
        {
            res = 404;
            responseMsg = "{\"message\":\"User not found\"}";
        }
        transaction.commit();
    }                    
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
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
                boost::lexical_cast<unsigned long long>(nextElement);

                nextElement = getNextElementFromPath();

                if(!nextElement.compare("recipients"))
                {
                    this->statusCode = getRecipientsForAlert(responseMsg);
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
    Wt::WString alertName, keyVal, alertValue;
    long long astId, seaId, srcId, plgId, infValNum, inuId, acrId;
    int threadSleep;
    Wt::Json::Array& amsId = Wt::Json::Array::Empty;
    try
    {
        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);
        //descriptif
        alertName = result.get("name");
        alertValue = result.get("alert_value");
        
        threadSleep = result.get("thread_sleep");
        
        keyVal = result.get("key_value");   
        
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
        Wt::Dbo::ptr<Information2> infoPtr = session->find<Information2>()
                .where("\"SEA_ID\" = ?").bind(seaId)
                .where("\"SRC_ID\" = ?").bind(srcId)
                .where("\"PLG_ID_PLG_ID\" = ?").bind(plgId)
                .where("\"INF_VALUE_NUM\" = ?").bind(infValNum)
                .where("\"INU_ID_INU_ID\" = ?").bind(inuId);

        Wt::Dbo::ptr<AlertCriteria> critPtr = session->find<AlertCriteria>().where("\"ACR_ID\" = ?").bind(acrId);

        Wt::Dbo::ptr<Asset> assetPtr = session->find<Asset>().where("\"AST_ID\" = ?").bind(astId);

        for (Wt::Json::Array::const_iterator idx1 = amsId.begin() ; idx1 < amsId.end(); idx1++)
        {
            Wt::WString tmp1 = idx1->toString();
            Wt::Dbo::ptr<AlertMediaSpecialization> amsPtr = session->find<AlertMediaSpecialization>()
                    .where("\"AMS_ID\" = ?").bind(tmp1)
                    .where("\"AMS_ALE_ALE_ID\" IS NULL");

            if (!infoPtr || !critPtr || !assetPtr || !amsPtr)
            {
                if(!infoPtr)
                {
                    Wt::log("info") << "information not found";
                }
                if(!critPtr)
                {
                    Wt::log("info") << "criteria not found";
                }
                if(!assetPtr)
                {
                    Wt::log("info") << "asset not found";
                }
                if(!amsPtr)
                {
                    Wt::log("info") << "alert_media_specialization not found or not available";
                }
                res = 404;
                responseMsg = "{\"message\":\"Not found\"}";
                return res; 
            }
        }
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

    try
    {
        Wt::Dbo::Transaction transaction(*session);

        Wt::Dbo::ptr<Information2> infoPtr = session->find<Information2>()
                .where("\"SEA_ID\" = ?").bind(seaId)
                .where("\"SRC_ID\" = ?").bind(srcId)
                .where("\"PLG_ID_PLG_ID\" = ?").bind(plgId)
                .where("\"INF_VALUE_NUM\" = ?").bind(infValNum)
                .where("\"INU_ID_INU_ID\" = ?").bind(inuId);

        Wt::Dbo::ptr<AlertCriteria> critPtr = session->find<AlertCriteria>().where("\"ACR_ID\" = ?").bind(acrId);

        Wt::Dbo::ptr<Asset> assetPtr = session->find<Asset>().where("\"AST_ID\" = ?").bind(astId);


        ava->information = infoPtr;
        ava->alertCriteria = critPtr;
        ava->value = alertValue;
        ava->keyValue = keyVal;
        ava->asset = assetPtr;
        Wt::Dbo::ptr<AlertValue> avaPtr = session->add<AlertValue>(ava);

        alert->alertValue = avaPtr;
        alert->name = alertName;
        alert->creaDate = Wt::WDateTime::currentDateTime();
        alert->threadSleep = threadSleep;

        Wt::Dbo::ptr<Alert> alePtr = session->add<Alert>(alert);

        for (Wt::Json::Array::const_iterator idx2 = amsId.begin() ; idx2 < amsId.end(); idx2++)
        {
            Wt::WString tmp = idx2->toString();
            Wt::Dbo::ptr<AlertMediaSpecialization> amsPtr = session->find<AlertMediaSpecialization>().where("\"AMS_ID\" = ?").bind(tmp);
            amsPtr.modify()->alert = alePtr;
            
            AlertMessageDefinition *amd = new AlertMessageDefinition();
            amd->pk.alert = alePtr;
            amd->pk.media = amsPtr->mediaValue->media;
            amd->pk.userRole = amsPtr->mediaValue->user->userRole;
            amd->isCustom = false;
            
            Wt::Dbo::ptr<AlertMessageAliasAsset> aliasAsset = this->session->find<AlertMessageAliasAsset>()
                    .where("\"AAA_DELETE\" IS NULL")
                    .where("\"URO_ID_URO_ID\" = ?").bind(amsPtr->mediaValue->user->userRole)
                    .where("\"MED_ID_MED_ID\" = ?").bind(amsPtr->mediaValue->media)
                    .where("\"AST_ID_AST_ID\" = ?").bind(assetPtr.id());
            
            Wt::Dbo::ptr<AlertMessageAliasPlugin> aliasPlugin = this->session->find<AlertMessageAliasPlugin>()
                .where("\"AAP_DELETE\" IS NULL")
                .where("\"URO_ID_URO_ID\" = ?").bind(amsPtr->mediaValue->user->userRole)
                .where("\"MED_ID_MED_ID\" = ?").bind(amsPtr->mediaValue->media)
                .where("\"PLG_ID_PLG_ID\" = ?").bind(infoPtr->pk.search->pk.source->pk.plugin.id());
            
            Wt::Dbo::ptr<AlertMessageAliasInformation> aliasInformation = this->session->find<AlertMessageAliasInformation>()
                .where("\"AAI_DELETE\" IS NULL")
                .where("\"URO_ID_URO_ID\" = ?").bind(amsPtr->mediaValue->user->userRole)
                .where("\"MED_ID_MED_ID\" = ?").bind(amsPtr->mediaValue->media)
                .where("\"PLG_ID_PLG_ID\" = ?").bind(infoPtr->pk.search->pk.source->pk.plugin.id())
                .where("\"SRC_ID\" = ?").bind(infoPtr->pk.search->pk.source->pk.id)
                .where("\"SEA_ID\" = ?").bind(infoPtr->pk.search->pk.id)
                .where("\"INF_VALUE_NUM\" = ?").bind(infoPtr->pk.subSearchNumber)
                .where("\"INU_ID_INU_ID\" = ?").bind(infoPtr->pk.unit);
            
            Wt::Dbo::ptr<AlertMessageAliasInformationCriteria> aliasCriteria = this->session->find<AlertMessageAliasInformationCriteria>()
                .where("\"AIC_DELETE\" IS NULL")
                .where("\"URO_ID_URO_ID\" = ?").bind(amsPtr->mediaValue->user->userRole)
                .where("\"MED_ID_MED_ID\" = ?").bind(amsPtr->mediaValue->media)
                .where("\"PLG_ID_PLG_ID\" = ?").bind(infoPtr->pk.search->pk.source->pk.plugin.id())
                .where("\"SRC_ID\" = ?").bind(infoPtr->pk.search->pk.source->pk.id)
                .where("\"SEA_ID\" = ?").bind(infoPtr->pk.search->pk.id)
                .where("\"INF_VALUE_NUM\" = ?").bind(infoPtr->pk.subSearchNumber)
                .where("\"INU_ID_INU_ID\" = ?").bind(infoPtr->pk.unit)
                .where("\"ACR_ID_ACR_ID\" = ?").bind(alePtr->alertValue->alertCriteria.id());
                    
            switch (amsPtr->mediaValue->media.id())
            {
                case Enums::SMS:
                    amd->message = "[EA][%detection-time%] : ";

                    //TODO: à revoir pour les alertes complexes !!
//                    for (Wt::Dbo::collection<Wt::Dbo::ptr<InformationValue>>::const_iterator i = ivaPtrCollection.begin(); i != ivaPtrCollection.end(); ++i)
//                    {
                    
                    if (aliasAsset)
                    {
                        amd->message += " Serveur : " + aliasAsset->alias;
                    }
                    else
                    {
                        amd->message += " Serveur " + assetPtr->name;
                    }

                        //we check if there is a key and get it if it's the case to put in the sms
                    //    if (!boost::lexical_cast<Wt::WString,boost::optional<Wt::WString> >(alertPtr.get()->alertValue.get()->keyValue).empty())
                    if (alePtr->alertValue->keyValue.is_initialized() && alePtr->alertValue->keyValue.get() != "N/A")
                    {
                       amd->message += " Cle : " + alePtr->alertValue->keyValue.get();
                    }
                    
                    if (aliasPlugin)
                    {
                        amd->message += " Application : " + aliasPlugin->alias;
                    }
                    else
                    {
                        amd->message += " Application : " + infoPtr->pk.search->pk.source->pk.plugin->name;
                    }
                    
                    if (aliasInformation)
                    {
                         amd->message += " Information : " + aliasInformation->alias;
                    }
                    else
                    {
                        amd->message += " Information : " + infoPtr->name;
                    }
                    
                    if (aliasCriteria)
                    {
                        amd->message += " Critere : " + aliasCriteria->alias;
                    }
                    else
                    {
                        std::string comp;
                        if(!alePtr->alertValue->alertCriteria->name.toUTF8().compare("lt"))
                        {
                            comp="Inférieur";
                        }
                        else if(!alePtr->alertValue->alertCriteria->name.toUTF8().compare("le"))
                        {
                            comp="Inférieur ou égal";
                        }
                        else if(!alePtr->alertValue->alertCriteria->name.toUTF8().compare("eq"))
                        {
                            comp="Egal";
                        }
                        else if(!alePtr->alertValue->alertCriteria->name.toUTF8().compare("ne"))
                        {
                            comp="Différent";
                        }
                        else if(!alePtr->alertValue->alertCriteria->name.toUTF8().compare("ge"))
                        {
                            comp="Supérieur ou égal";
                        }
                        else if(!alePtr->alertValue->alertCriteria->name.toUTF8().compare("gt"))
                        {
                            comp="Supérieur";
                        }
                        amd->message += " Critere : %value% " + comp + " %threshold% " + infoPtr->pk.unit->name.toUTF8();
//                        amd->message += " Critere : %value% " + Wt::WString::tr("Alert.alert.operator."+alePtr->alertValue->alertCriteria->name.toUTF8()).toUTF8() + " %threshold% " + infoPtr->pk.unit->name.toUTF8();
                    }
//                    }
                    break;
                case Enums::MAIL:
                    amd->message = "[EA][%detection-time%] : <br />";

                    //TODO: à revoir pour les alertes complexes !!
//                    for (Wt::Dbo::collection<Wt::Dbo::ptr<InformationValue>>::const_iterator i = ivaPtrCollection.begin(); i != ivaPtrCollection.end(); ++i)
//                    {
                    
                    if (aliasAsset)
                    {
                        amd->message += " Serveur : " + aliasAsset->alias + "<br />";
                    }
                    else
                    {
                        amd->message += " Serveur " + assetPtr->name + "<br />";
                    }

                        //we check if there is a key and get it if it's the case to put in the sms
                    //    if (!boost::lexical_cast<Wt::WString,boost::optional<Wt::WString> >(alertPtr.get()->alertValue.get()->keyValue).empty())
                    if (alePtr->alertValue->keyValue.is_initialized() && alePtr->alertValue->keyValue.get() != "N/A")
                    {
                       amd->message += " Cle : " + alePtr->alertValue->keyValue.get() + "<br />";
                    }
                    
                    if (aliasPlugin)
                    {
                        amd->message += " Application : " + aliasPlugin->alias + "<br />";
                    }
                    else
                    {
                        amd->message += " Application : " + infoPtr->pk.search->pk.source->pk.plugin->name + "<br />";
                    }
                    
                    if (aliasInformation)
                    {
                         amd->message += " Information : " + aliasInformation->alias + "<br />";
                    }
                    else
                    {
                        amd->message += " Information : " + infoPtr->name + "<br />";
                    }
                    
                    if (aliasCriteria)
                    {
                        amd->message += " Critere : " + aliasCriteria->alias + " " + infoPtr->pk.unit->name.toUTF8() + "<br />";
                    }
                    else
                    {
                        std::string comp;
                        if(!alePtr->alertValue->alertCriteria->name.toUTF8().compare("lt"))
                        {
                            comp="Inférieur";
                        }
                        else if(!alePtr->alertValue->alertCriteria->name.toUTF8().compare("le"))
                        {
                            comp="Inférieur ou égal";
                        }
                        else if(!alePtr->alertValue->alertCriteria->name.toUTF8().compare("eq"))
                        {
                            comp="Egal";
                        }
                        else if(!alePtr->alertValue->alertCriteria->name.toUTF8().compare("ne"))
                        {
                            comp="Différent";
                        }
                        else if(!alePtr->alertValue->alertCriteria->name.toUTF8().compare("ge"))
                        {
                            comp="Supérieur ou égal";
                        }
                        else if(!alePtr->alertValue->alertCriteria->name.toUTF8().compare("gt"))
                        {
                            comp="Supérieur";
                        }
                        amd->message += " Critere : %value% " + comp + " %threshold% " + infoPtr->pk.unit->name.toUTF8() + "<br />";
//                        amd->message += " Critere : %value% " + Wt::WString::tr("Alert.alert.operator."+alePtr->alertValue->alertCriteria->name.toUTF8()).toUTF8() + " %threshold% " + infoPtr->pk.unit->name.toUTF8() + "<br />";
                    }

                    amd->message += "Plus d'informations sur https://alert.echoes-tech.com";
                    break;
                default:
                    Wt::log("error") << "[Alert Resource] Unknown ID Media: " << amsPtr->mediaValue->media.id();
                    break;
            }

            Wt::Dbo::ptr<AlertMessageDefinition> amdPtr = session->add<AlertMessageDefinition>(amd);
            amdPtr.flush();
        }
        alePtr.flush();
        alePtr.modify()->setId(alePtr.id());
        responseMsg = alePtr.modify()->toJSON();
        transaction.commit();

        res = 201;
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    
    return res;
}

unsigned short AlertResource::sendMAIL
(
 Wt::Dbo::collection<Wt::Dbo::ptr<InformationValue>> ivaPtrCollection,
 Wt::Dbo::ptr<Alert> alePtr,
 Wt::Dbo::ptr<AlertMessageDefinition> amdPtr,
 Wt::Dbo::ptr<AlertTracking> atrPtr,
 Wt::Dbo::ptr<AlertMediaSpecialization> amsPtr,
 bool overSMSQuota
)
{
    unsigned short res = Enums::INTERNAL_SERVER_ERROR;

    Wt::WString mailRecipient;
    const Wt::WString mailRecipientName = amsPtr->mediaValue->user->firstName + " " + amsPtr->mediaValue->user->lastName ;
    string mailBody = "";
    const Wt::WDateTime now = Wt::WDateTime::currentDateTime(); //for setting the send date of the alert
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

    mailBody += amdPtr->message.toUTF8();

    //TODO: à revoir pour les alertes complexes !!
    for (Wt::Dbo::collection<Wt::Dbo::ptr<InformationValue>>::const_iterator i = ivaPtrCollection.begin(); i != ivaPtrCollection.end(); ++i)
    {
          boost::replace_all(mailBody, "%value%", i->get()->value.toUTF8());
          boost::replace_all(mailBody, "%threshold%", alePtr->alertValue->value.toUTF8());
          boost::replace_all(mailBody, "%detection-time%", i->get()->creationDate.toString().toUTF8());
          boost::replace_all(mailBody, "%alerting-time%", now.toString().toUTF8());
    }

    Wt::log("info") << " [Alert Resource] " << mailBody;     
    
    mailMessage.setFrom(Wt::Mail::Mailbox("alert@echoes-tech.com", "ECHOES Alert"));
    mailMessage.addRecipient(Wt::Mail::To, Wt::Mail::Mailbox(mailRecipient.toUTF8(), mailRecipientName));
    mailMessage.setSubject("[ECHOES Alert] " + alePtr->name);
    mailMessage.addHtmlBody(mailBody);
    mailClient.connect("hermes.gayuxweb.fr");
    mailClient.send(mailMessage);

    Wt::log("info") << " [Class:AlertSender] " << "insert date of last send in db : " << now.toString();
    amsPtr.modify()->lastSend = now;

    atrPtr.modify()->sendDate = now;

    res = Enums::OK;
    
    return res;
}

unsigned short AlertResource::sendSMS
(
 Wt::Dbo::collection<Wt::Dbo::ptr<InformationValue>> ivaPtrCollection,
 Wt::Dbo::ptr<Alert> alePtr,
 Wt::Dbo::ptr<AlertMessageDefinition> amdPtr,
 Wt::Dbo::ptr<AlertTracking> atrPtr,
 Wt::Dbo::ptr<AlertMediaSpecialization> amsPtr
)
{
    unsigned short res = Enums::INTERNAL_SERVER_ERROR;
    
    const Wt::WDateTime now = Wt::WDateTime::currentDateTime(); //for setting the send date of the alert
    string sms = amdPtr->message.toUTF8();

    //TODO: à revoir pour les alertes complexes !!
    for (Wt::Dbo::collection<Wt::Dbo::ptr<InformationValue>>::const_iterator i = ivaPtrCollection.begin(); i != ivaPtrCollection.end(); ++i)
    {
          boost::replace_all(sms, "%value%", i->get()->value.toUTF8());
          boost::replace_all(sms, "%threshold%", alePtr->alertValue->value.toUTF8());
          boost::replace_all(sms, "%detection-time%", i->get()->creationDate.toString().toUTF8());
          boost::replace_all(sms, "%alerting-time%", now.toString().toUTF8());
    }

    Wt::log("info") << " [Alert Resource] New SMS for " << amsPtr->mediaValue->value << " : " << sms;

    ItookiSMSSender itookiSMSSender(amsPtr->mediaValue->value.toUTF8(), sms, this);
    itookiSMSSender.setAlertTrackingPtr(atrPtr);
    if(!itookiSMSSender.send())
    {
        amsPtr.modify()->lastSend = now;
        res = Enums::OK;
    }

    return res;
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

            Wt::Json::Object result;

            Wt::Json::parse(sRequest, result);
            
            Wt::Json::Array array = result.get("alert_ids");

            for (Wt::Json::Array::const_iterator i = array.begin(); i != array.end(); ++i)
            {
                long long id = i->toNumber();
                
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
                        long long medID = i->get()->mediaValue->media.id();

                        newAlertTracking->alert = alertPtr;
                        newAlertTracking->mediaValue = i->get()->mediaValue;

                        // WARNING : SendDate must be set by the API when the alert was sent, not before
                        newAlertTracking->sendDate = *(new Wt::WDateTime());

                        Wt::Dbo::ptr<AlertTracking> alertTrackingPtr = session->add<AlertTracking>(newAlertTracking);
                        alertTrackingPtr.flush();

                        Wt::log("info") << " [Alert Ressource] " << "Alert tracking number creation : " << alertTrackingPtr.id();

                        Wt::log("debug") << " [Alert Ressource] " << "snooze = " << i->get()->snoozeDuration;
                        
                        Wt::Dbo::ptr<AlertMessageDefinition> amdPtr = session->find<AlertMessageDefinition>()
                                .where("\"ALE_ID_ALE_ID\" = ?").bind(alertPtr.id())
                                .where("\"URO_ID_URO_ID\" = ?").bind(i->get()->mediaValue->user->userRole.id())
                                .where("\"MED_ID_MED_ID\" = ?").bind(medID)
                                .where("\"AMD_DELETE\" IS NULL")
                                .limit(1);
                        
                        switch (medID)
                        {
                            case Enums::SMS:
                            {
                                Wt::log("info") << " [Alert Ressource] " << "Media value SMS choosed for the alert : " << alertPtr->name;

                                // Verifying the quota of sms
                                Wt::Dbo::ptr<OptionValue> optionValuePtr = session->find<OptionValue>()
                                        .where("\"OPT_ID_OPT_ID\" = ?").bind(Enums::QUOTA_SMS)
                                        .where("\"ORG_ID_ORG_ID\" = ?").bind(i->get()->mediaValue->user->currentOrganization.id())
                                        .limit(1);

                                try
                                {
                                    int smsQuota = boost::lexical_cast<int>(optionValuePtr->value); 
                                    if (smsQuota == 0)
                                    {

                                        Wt::log("info") << " [Alert Ressource] " << "SMS quota 0 for alert : " <<  alertPtr->name;
                                        Wt::log("info") << " [Alert Ressource] " << "Sending e-mail instead." ;

                                        sendMAIL(ivaPtrCollection, alertPtr, amdPtr, alertTrackingPtr, *i, true);
                                    }
                                    else
                                    {
                                        Wt::log("debug") << " [Alert Ressource] " << "We send a SMS, quota : "<< smsQuota;
                                        optionValuePtr.modify()->value = boost::lexical_cast<string>(smsQuota - 1);
                                        optionValuePtr.flush();                        
                                        sendSMS(ivaPtrCollection, alertPtr, amdPtr, alertTrackingPtr, *i); 
                                    }
                                }
                                catch(boost::bad_lexical_cast &)
                                {
                                    res = 503;
                                    responseMsg = "{\n\t\"message\": \"Service Unavailable\"\n}";
                                }
                                break;
                            }
                            case Enums::MAIL:
                                Wt::log("info") << " [Alert Ressource] " << "Media value MAIL choosed for the alert : " << alertPtr->name;
                                sendMAIL(ivaPtrCollection, alertPtr, amdPtr, alertTrackingPtr, *i);
                                break;
                            default:
                                Wt::log("error") << "[Alert Resource] Unknown ID Media: " << medID;
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
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();
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
        string queryStr = "SELECT ale FROM \"T_ALERT_ALE\" ale "
                " WHERE \"ALE_ID\" IN"
                " (SELECT \"AMS_ALE_ALE_ID\" FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" "
                " WHERE \"AMS_MEV_MEV_ID\" IN "
                " (SELECT \"MEV_ID\" FROM \"T_MEDIA_VALUE_MEV\" "
                " WHERE \"MEV_USR_USR_ID\" IN "
                " ( Select \"T_USER_USR_USR_ID\" "
                " FROM \"TJ_USR_ORG\" "
                " WHERE \"T_ORGANIZATION_ORG_ORG_ID\" = " + boost::lexical_cast<string>(this->session->user()->currentOrganization.id()) +
                " )))"
                " AND \"ALE_ID\" = " + this->vPathElements[1];
        
        Wt::Dbo::Query<Wt::Dbo::ptr<Alert>> resQuery = session->query<Wt::Dbo::ptr<Alert>>(queryStr);
        Wt::Dbo::ptr<Alert> alertPtr = resQuery.resultValue();
       // Wt::Dbo::ptr<Alert> alertPtr = session->find<Alert>().where("\"ALE_ID\" = ?").bind(boost::lexical_cast<int>(this->vPathElements[1])); 
        if (alertPtr)
        {
            alertPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
            transaction.commit();

            res = 204;
            responseMsg = "";
        }
        else
        {
            res = 404;
            responseMsg = "{\"message\" : \"Alert not found\" }";
        }
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
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
            boost::lexical_cast<unsigned long long>(nextElement);

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

