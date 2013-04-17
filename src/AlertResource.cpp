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

unsigned short AlertResource::getRecipientsForAlert(std::string &responseMsg) const
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
                    " AND mev.\"MEV_USR_USR_ID\" IN" 
                 "( Select \"T_USER_USR_USR_ID\" "
                 "FROM \"TJ_USR_ORG\" "
                "WHERE \"T_ORGANIZATION_ORG_ORG_ID\" = ?" + boost::lexical_cast<std::string > (this->session->user()->currentOrganization.id()) +
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

unsigned short AlertResource::getTrackingAlertList(std::string &responseMsg) const
{
    responseMsg="";
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


unsigned short AlertResource::getAlerts(std::string &responseMsg) const
{
    unsigned short res = 500;
    int idx = 0;
    try 
    {
        Wt::Dbo::Transaction transaction(*session);
        std::string queryString = "SELECT ale, acr, ava, inu FROM \"T_ALERT_ALE\" ale,"
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
                " WHERE \"T_ORGANIZATION_ORG_ORG_ID\" = ?" + boost::lexical_cast<std::string > (this->session->user()->currentOrganization.id()) +
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


unsigned short AlertResource::postAlert(std::string &responseMsg, const std::string &sRequest)
{
    unsigned short res = 500;
    Wt::WString alertName, keyVal, alertValue;
long long astId, seaId, 
        srcId, plgId, infValNum, inuId, acrId;
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
        std::string queryStr = "SELECT ale FROM \"T_ALERT_ALE\" ale "
                " WHERE \"ALE_ID\" IN"
                " (SELECT \"AMS_ALE_ALE_ID\" FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" "
                " WHERE \"AMS_MEV_MEV_ID\" IN "
                " (SELECT \"MEV_ID\" FROM \"T_MEDIA_VALUE_MEV\" "
                " WHERE \"MEV_USR_USR_ID\" IN "
                " ( Select \"T_USER_USR_USR_ID\" "
                " FROM \"TJ_USR_ORG\" "
                " WHERE \"T_ORGANIZATION_ORG_ORG_ID\" = " + boost::lexical_cast<std::string>(this->session->user()->currentOrganization.id()) +
                " )))"
                " AND \"ALE_ID\" = " + this->vPathElements[1];
        
        Wt::Dbo::Query<Wt::Dbo::ptr<Alert>> resQuery = session->query<Wt::Dbo::ptr<Alert>>(queryStr);
        Wt::Dbo::ptr<Alert> alertPtr = resQuery.resultValue();
       // Wt::Dbo::ptr<Alert> alertPtr = session->find<Alert>().where("\"ALE_ID\" = ?").bind(boost::lexical_cast<int>(this->vPathElements[1])); 
        if(alertPtr)
        {
                alertPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
        transaction.commit();
        std::cerr<<"after commit" << std::endl;

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