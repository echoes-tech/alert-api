                
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
#include "tools/XmlSerializer.h"
#include "AlertResource.h"

using namespace std;

AlertResource::AlertResource() : PublicApiResource::PublicApiResource()
{
}

AlertResource::~AlertResource()
{
}

unsigned short AlertResource::getRecipientsForAlert(string &responseMsg)
{
    unsigned short res = Echoes::Dbo::EReturnCode::INTERNAL_SERVER_ERROR;
    unsigned idx = 0;
    try 
    {
        Wt::Dbo::Transaction transaction(m_session);
        
         string queryString = "SELECT mev, ams FROM \"T_MEDIA_VALUE_MEV\" mev," 
                    " \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" ams "
                    "WHERE ams.\"AMS_ALE_ALE_ID\" = " + m_pathElements[1] +
                    " AND ams.\"AMS_MEV_MEV_ID\" = mev.\"MEV_ID\" "
                    " AND mev.\"MEV_USR_USR_ID\" IN" 
                 "( Select \"T_USER_USR_USR_ID\" "
                 "FROM \"TJ_USR_ORG\" "
                "WHERE \"T_ORGANIZATION_ORG_ORG_ID\" = " + boost::lexical_cast<string > (m_session.user()->organization.id()) +
                 ")";

        Wt::Dbo::Query
                <
                boost::tuple
                <
                Wt::Dbo::ptr<Echoes::Dbo::MediaValue>,
                Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization>
                >
                , Wt::Dbo::DynamicBinding
                > resQuery = m_session.query
                <
                boost::tuple
                <
                Wt::Dbo::ptr<Echoes::Dbo::MediaValue>,
                Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization>
                >, Wt::Dbo::DynamicBinding
                > (queryString);

        Wt::Dbo::collection<boost::tuple<
                Wt::Dbo::ptr<Echoes::Dbo::MediaValue>,
                Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> > > listTuples = resQuery.resultList();
        if (listTuples.size() > 0) 
        {
            responseMsg = "[";
            for (Wt::Dbo::collection<boost::tuple<
                    Wt::Dbo::ptr<Echoes::Dbo::MediaValue>,
                    Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> > >::const_iterator i = listTuples.begin(); i != listTuples.end(); ++i) 
            {
                i->get<0>().modify()->setId(i->get<0>().id());
                i->get<1>().modify()->setId(i->get<1>().id());
                responseMsg += "\n{\n";
                responseMsg += "media_value :" + i->get<0>()->toJSON();
                responseMsg += ",\nmedia_specialization :" + i->get<1>()->toJSON();
                responseMsg += "\n}";
                 ++idx;
                if(listTuples.size()-idx > 0)
                {
                    responseMsg += ",\n";
                }
                
            }
            responseMsg += "]";
            res = Echoes::Dbo::EReturnCode::OK; 
        } 
        else 
        {
            res = Echoes::Dbo::EReturnCode::NOT_FOUND;
            responseMsg = "{\"message\":\"Alert not found\"}";
        }
        transaction.commit();
    }                    
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = Echoes::Dbo::EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;
}

unsigned short AlertResource::getTrackingAlertMessage(std::string &responseMsg)
{
    responseMsg="";
    unsigned short res = Echoes::Dbo::EReturnCode::INTERNAL_SERVER_ERROR;
    int idx = 0;
    
    try 
    {
        Wt::Dbo::Transaction transaction(m_session);
        
        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> > aleTrackingPtr = m_session.find<Echoes::Dbo::AlertTracking>()
                .where("\"ATR_MEV_MEV_ID\" = ? ").bind(m_pathElements[2])
                .where("\"ATR_SEND_DATE\" IS NOT NULL")
                .orderBy("\"ATR_SEND_DATE\" DESC")
                .limit(20);

        if (aleTrackingPtr.size() > 0)
        {
            responseMsg = "[\n"; 
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> >::const_iterator i = aleTrackingPtr.begin(); i != aleTrackingPtr.end(); ++i) 
            {
                i->modify()->setId(i->id());
                
                responseMsg += i->get()->toJSON();
                 ++idx;
                if(aleTrackingPtr.size()-idx > 0)
                {
                    responseMsg += ",\n";
                }
            }
            responseMsg += "\n]\n";
            res = Echoes::Dbo::EReturnCode::OK;

        }
        else
        {
            res = Echoes::Dbo::EReturnCode::NOT_FOUND;
            responseMsg = "{\"message\":\"Not found\"}";
        }

        transaction.commit();
        
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = Echoes::Dbo::EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;
}

unsigned short AlertResource::getTrackingAlertList(string &responseMsg)
{
    responseMsg="";
    unsigned short res = Echoes::Dbo::EReturnCode::INTERNAL_SERVER_ERROR;
    int idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        string queryString = "SELECT ale, mev, atr FROM \"T_ALERT_TRACKING_ATR\" atr, \"T_ALERT_ALE\" ale , \"T_MEDIA_VALUE_MEV\" mev "
            " WHERE atr.\"ATR_ALE_ALE_ID\" = ale.\"ALE_ID\" "
            " AND atr.\"ATR_MEV_MEV_ID\" = mev.\"MEV_ID\" "
            " AND mev.\"MEV_USR_USR_ID\" IN"
            "("
                "SELECT \"T_USER_USR_USR_ID\" FROM \"TJ_USR_ORG\" WHERE \"T_ORGANIZATION_ORG_ORG_ID\" = " 
                + boost::lexical_cast<string>(m_session.user()->organization.id()) + " "
            ")"
            " order by \"ATR_SEND_DATE\" desc "
            " limit 50";
        Wt::Dbo::Query
                <
                boost::tuple
                <
                Wt::Dbo::ptr<Echoes::Dbo::Alert>,
                Wt::Dbo::ptr<Echoes::Dbo::MediaValue>,
                Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> 
                >
                ,Wt::Dbo::DynamicBinding
                >resQuery = m_session.query
                <
                boost::tuple
                <
                Wt::Dbo::ptr<Echoes::Dbo::Alert>,
                Wt::Dbo::ptr<Echoes::Dbo::MediaValue>,
                Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> 
                >
                ,Wt::Dbo::DynamicBinding>(queryString);

        Wt::Dbo::collection<boost::tuple
                <
                Wt::Dbo::ptr<Echoes::Dbo::Alert>,
                Wt::Dbo::ptr<Echoes::Dbo::MediaValue>,
                Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> 
                > > listTuples = resQuery.resultList();

        if (listTuples.size() > 0)
        {
            responseMsg = "[";
            for (Wt::Dbo::collection<boost::tuple<
                 Wt::Dbo::ptr<Echoes::Dbo::Alert>,
                 Wt::Dbo::ptr<Echoes::Dbo::MediaValue>,
                 Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> > >::const_iterator i = listTuples.begin(); i != listTuples.end(); ++i) 
            {
                i->get<0>().modify()->setId(i->get<0>().id());
                i->get<1>().modify()->setId(i->get<1>().id());
                i->get<2>().modify()->setId(i->get<2>().id());
                
                responseMsg += "\n{\n";
                responseMsg +="\"alert\" :" + i->get<0>()->toJSON();
                responseMsg +=",\n\"media_value\" :" + i->get<1>()->toJSON();
                responseMsg +=",\n\"alert_tracking\" :" + i->get<2>()->toJSON();
                responseMsg += "\n}";
                 ++idx;
                if(listTuples.size()-idx > 0)
                {
                    responseMsg += ",\n";
                }
            }
            responseMsg += "\n]\n";
            res = Echoes::Dbo::EReturnCode::OK;

        }
        else
        {
            res = Echoes::Dbo::EReturnCode::NOT_FOUND;
            responseMsg = "{\"message\":\"Tracking alert not found\"}";
        }

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = Echoes::Dbo::EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;
}

unsigned short AlertResource::getAlerts(string &responseMsg)
{
    unsigned short res = Echoes::Dbo::EReturnCode::INTERNAL_SERVER_ERROR;
    int idx = 0;
    try 
    {
        Wt::Dbo::Transaction transaction(m_session);
        string queryString = "SELECT ale, acr, ava, inu FROM \"T_ALERT_ALE\" ale,"
                " \"T_ALERT_VALUE_AVA\" ava,"
                " \"T_ALERT_CRITERIA_ACR\" acr,"
                " \"T_INFORMATION_UNIT_INU\" inu "
                " WHERE \"ALE_ID\" IN "
                " ("
                " SELECT \"AMS_ALE_ALE_ID\" FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" WHERE \"AMS_MEV_MEV_ID\" IN "
                " ("
                " SELECT \"MEV_ID\" FROM \"T_MEDIA_VALUE_MEV\" WHERE \"MEV_USR_USR_ID\"  IN"
                " ( SELECT \"T_USER_USR_USR_ID\" "
                " FROM \"TJ_USR_ORG\" "
                " WHERE \"T_ORGANIZATION_ORG_ORG_ID\" = " + boost::lexical_cast<string > (m_session.user()->organization.id()) +
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
                Wt::Dbo::ptr<Echoes::Dbo::Alert>,
                Wt::Dbo::ptr<Echoes::Dbo::AlertCriteria>,
                Wt::Dbo::ptr<Echoes::Dbo::AlertValue>,
                Wt::Dbo::ptr<Echoes::Dbo::InformationUnit>
                >
                , Wt::Dbo::DynamicBinding
                > resQuery = m_session.query
                <
                boost::tuple
                <
                Wt::Dbo::ptr<Echoes::Dbo::Alert>,
                Wt::Dbo::ptr<Echoes::Dbo::AlertCriteria>,
                Wt::Dbo::ptr<Echoes::Dbo::AlertValue>,
                Wt::Dbo::ptr<Echoes::Dbo::InformationUnit>
                >, Wt::Dbo::DynamicBinding
                > (queryString);


        Wt::Dbo::collection<boost::tuple<
                Wt::Dbo::ptr<Echoes::Dbo::Alert>,
                Wt::Dbo::ptr<Echoes::Dbo::AlertCriteria>,
                Wt::Dbo::ptr<Echoes::Dbo::AlertValue>,
                Wt::Dbo::ptr<Echoes::Dbo::InformationUnit> > > listTuples = resQuery.resultList();
        if (listTuples.size() > 0) 
        {
            responseMsg = "[";
            for (Wt::Dbo::collection<boost::tuple<
                    Wt::Dbo::ptr<Echoes::Dbo::Alert>,
                    Wt::Dbo::ptr<Echoes::Dbo::AlertCriteria>,
                    Wt::Dbo::ptr<Echoes::Dbo::AlertValue>,
                    Wt::Dbo::ptr<Echoes::Dbo::InformationUnit> > >::const_iterator i = listTuples.begin(); i != listTuples.end(); ++i) 
            {        
                Wt::Dbo::ptr<Echoes::Dbo::AlertValue> aleVal(i->get<2>()); 
                Wt::Dbo::ptr<Echoes::Dbo::Alert> ale(i->get<0>()); 
                Wt::Dbo::ptr<Echoes::Dbo::AlertCriteria> aleCrit(i->get<1>());
                Wt::Dbo::ptr<Echoes::Dbo::InformationUnit> info(i->get<3>()); 
                
                ale.modify()->setId(ale.id());
                aleCrit.modify()->setId(aleCrit.id());
                aleVal.modify()->setId(aleVal.id());      
                info.modify()->setId(info.id());
                responseMsg += "\n{\n";
                responseMsg += "\"alert\" :" + ale->toJSON();
                responseMsg += ",\n\"criteria\" :" + aleCrit->toJSON();
                responseMsg += ",\n\"alert_value\" :" + aleVal->toJSON();
                responseMsg += ",\n\"information_unit\" :" + info->toJSON();
                responseMsg += "\n}";
                
                Wt::Dbo::XmlSerializer serializer(std::cout, m_session);
                serializer.Serialize(ale);


                ++idx;
                if(listTuples.size()-idx > 0)
                {
                    responseMsg += ",\n";
                }

            }
            responseMsg += "\n]";
            res = Echoes::Dbo::EReturnCode::OK; 
        } 
        else 
        {
            res = Echoes::Dbo::EReturnCode::NOT_FOUND;
            responseMsg = "{\"message\":\"Alert not found\"}";
        }
        transaction.commit();
    }                    
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = Echoes::Dbo::EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;
}

unsigned short AlertResource::getAlert(std::string &responseMsg)
{
    unsigned short res = Echoes::Dbo::EReturnCode::INTERNAL_SERVER_ERROR;
    try 
    {
        Wt::Dbo::Transaction transaction(m_session);
        string queryString = "SELECT ale, acr, ava, inu FROM \"T_ALERT_ALE\" ale,"
                " \"T_ALERT_VALUE_AVA\" ava,"
                " \"T_ALERT_CRITERIA_ACR\" acr,"
                " \"T_INFORMATION_UNIT_INU\" inu "
                " WHERE \"ALE_ID\" IN "
                " ("
                " SELECT \"AMS_ALE_ALE_ID\" FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" WHERE \"AMS_MEV_MEV_ID\" IN "
                " ("
                " SELECT \"MEV_ID\" FROM \"T_MEDIA_VALUE_MEV\" WHERE \"MEV_USR_USR_ID\"  IN"
                " ( SELECT \"T_USER_USR_USR_ID\" "
                " FROM \"TJ_USR_ORG\" "
                " WHERE \"T_ORGANIZATION_ORG_ORG_ID\" = " + boost::lexical_cast<string > (m_session.user()->organization.id()) +
                " )"
                " )"
                " AND \"AMS_ALE_ALE_ID\" = " + m_pathElements[1] +
                " ) "
                " AND ale.\"ALE_AVA_AVA_ID\" = ava.\"AVA_ID\" "
                " AND ale.\"ALE_DELETE\" IS NULL "
                " AND ava.\"AVA_ACR_ACR_ID\" = acr.\"ACR_ID\" "
                " AND inu.\"INU_ID\" = ava.\"INU_ID_INU_ID\" ";
                
         
        
        Wt::Dbo::Query
                <
                boost::tuple
                <
                Wt::Dbo::ptr<Echoes::Dbo::Alert>,
                Wt::Dbo::ptr<Echoes::Dbo::AlertCriteria>,
                Wt::Dbo::ptr<Echoes::Dbo::AlertValue>,
                Wt::Dbo::ptr<Echoes::Dbo::InformationUnit>
                >
                , Wt::Dbo::DynamicBinding
                > resQuery = m_session.query
                <
                boost::tuple
                <
                Wt::Dbo::ptr<Echoes::Dbo::Alert>,
                Wt::Dbo::ptr<Echoes::Dbo::AlertCriteria>,
                Wt::Dbo::ptr<Echoes::Dbo::AlertValue>,
                Wt::Dbo::ptr<Echoes::Dbo::InformationUnit>
                >, Wt::Dbo::DynamicBinding
                > (queryString);


        boost::tuple<
                Wt::Dbo::ptr<Echoes::Dbo::Alert>,
                Wt::Dbo::ptr<Echoes::Dbo::AlertCriteria>,
                Wt::Dbo::ptr<Echoes::Dbo::AlertValue>,
                Wt::Dbo::ptr<Echoes::Dbo::InformationUnit> >  tuples = resQuery;
        
        Wt::Dbo::ptr<Echoes::Dbo::AlertValue> aleVal(tuples.get<2>()); 
        Wt::Dbo::ptr<Echoes::Dbo::Alert> ale(tuples.get<0>()); 
        Wt::Dbo::ptr<Echoes::Dbo::AlertCriteria> aleCrit(tuples.get<1>());
        Wt::Dbo::ptr<Echoes::Dbo::InformationUnit> info(tuples.get<3>()); 
        if (aleVal) 
        {
            ale.modify()->setId(ale.id());
            aleCrit.modify()->setId(aleCrit.id());
            aleVal.modify()->setId(aleVal.id());      
            info.modify()->setId(info.id());
            responseMsg = "{\n";
            responseMsg += "\"alert\" :" + ale->toJSON();
            responseMsg += ",\n\"criteria\" :" + aleCrit->toJSON();
            responseMsg += ",\n\"alert_value\" :" + aleVal->toJSON();
            responseMsg += ",\n\"information_unit\" :" + info->toJSON();
            responseMsg += "\n}";
            res = Echoes::Dbo::EReturnCode::OK; 
        } 
        else 
        {
            res = Echoes::Dbo::EReturnCode::NOT_FOUND;
            responseMsg = "{\"message\":\"Alert not found\"}";
        }
        transaction.commit();
    }                    
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = Echoes::Dbo::EReturnCode::SERVICE_UNAVAILABLE;
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
        m_statusCode = getAlerts(responseMsg);
    }
    else
    {
        if(!nextElement.compare("trackings"))
        {
            nextElement = getNextElementFromPath();
             if(!nextElement.compare(""))
                {
                    m_statusCode = getTrackingAlertList(responseMsg) ;
                }
                else
                {
                   m_statusCode = Echoes::Dbo::EReturnCode::BAD_REQUEST;
                   responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                }
        }
        else if(!nextElement.compare("recipients"))
        {
            nextElement = getNextElementFromPath();
            boost::lexical_cast<unsigned long long>(nextElement);
            nextElement = getNextElementFromPath();
            if(!nextElement.compare("trackings"))
            {
                nextElement = getNextElementFromPath();
                 if(!nextElement.compare(""))
                 {
                    m_statusCode = getTrackingAlertMessage(responseMsg) ;
                 }
                 else
                {
                 m_statusCode = Echoes::Dbo::EReturnCode::BAD_REQUEST;
                 responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                }
            }
            else
            {
               m_statusCode = Echoes::Dbo::EReturnCode::BAD_REQUEST;
               responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        else
        {
            try
            {
                boost::lexical_cast<unsigned long long>(nextElement);

                nextElement = getNextElementFromPath();
                if (!nextElement.compare(""))
                {
                    m_statusCode = getAlert(responseMsg);
                }
                else if (!nextElement.compare("recipients"))
                {
                    m_statusCode = getRecipientsForAlert(responseMsg);
                }
                else
                {
                    m_statusCode = Echoes::Dbo::EReturnCode::BAD_REQUEST;
                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                }
            }
            catch (boost::bad_lexical_cast &)
            {
                m_statusCode = Echoes::Dbo::EReturnCode::BAD_REQUEST;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
    }
   
    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}


unsigned short AlertResource::postAlert(string &responseMsg, const string &sRequest)
{
    unsigned short res = Echoes::Dbo::EReturnCode::INTERNAL_SERVER_ERROR;
    Wt::WString alertName, keyVal, alertValue;
    long long astId, infId, plgId, acrId;
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
        infId = result.get("inf_id");
        
        //plugin
        plgId = result.get("plg_id");
        
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
    
    Wt::Dbo::ptr<Echoes::Dbo::Information> infoPtr;
    Wt::Dbo::ptr<Echoes::Dbo::Plugin> plgPtr;
    Wt::Dbo::ptr<Echoes::Dbo::AlertCriteria> critPtr;
    Wt::Dbo::ptr<Echoes::Dbo::Asset> assetPtr;
    
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        //info requête bonne?
        infoPtr = m_session.find<Echoes::Dbo::Information>().where("\"INF_ID\" = ?").bind(infId).limit(1);
        
        plgPtr = m_session.find<Echoes::Dbo::Plugin>().where("\"PLG_ID\" = ?").bind(plgId).limit(1);

        critPtr = m_session.find<Echoes::Dbo::AlertCriteria>().where("\"ACR_ID\" = ?").bind(acrId).limit(1);

        assetPtr = m_session.find<Echoes::Dbo::Asset>().where("\"AST_ID\" = ?").bind(astId).limit(1);

        for (Wt::Json::Array::const_iterator idx1 = amsId.begin() ; idx1 < amsId.end(); idx1++)
        {
            Wt::WString tmp1 = idx1->toString();
            Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr = m_session.find<Echoes::Dbo::AlertMediaSpecialization>()
                    .where("\"AMS_ID\" = ?").bind(tmp1)
                    .where("\"AMS_ALE_ALE_ID\" IS NULL");

            if (!infoPtr || !plgPtr || !critPtr || !assetPtr || !amsPtr)
            {
                if(!infoPtr)
                {
                    Wt::log("info") << "information not found";
                }
                if(!plgPtr)
                {
                    Wt::log("info") << "plugin not found";
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
                res = Echoes::Dbo::EReturnCode::NOT_FOUND;
                responseMsg = "{\"message\":\"Not found\"}";
                return res; 
            }
        }
        transaction.commit();
    }
    catch (Wt::Dbo::Exception e)
    {
        Wt::log("error") << "[AlertResource]" << e.what();
        res = Echoes::Dbo::EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = "{\"message\":\"Service Unavailable.\"}";
        return res;
    }

    //Create new alert
    Echoes::Dbo::Alert *alert = new Echoes::Dbo::Alert();
    Echoes::Dbo::AlertValue *ava = new Echoes::Dbo::AlertValue();

    try
    {
        Wt::Dbo::Transaction transaction(m_session);


        ava->information = infoPtr;
        ava->alertCriteria = critPtr;
        ava->value = alertValue;
        ava->keyValue = keyVal;
        ava->asset = assetPtr;
        Wt::Dbo::ptr<Echoes::Dbo::AlertValue> avaPtr = m_session.add<Echoes::Dbo::AlertValue>(ava);

        alert->alertValue = avaPtr;
        alert->name = alertName;
        alert->creaDate = Wt::WDateTime::currentDateTime();
        alert->threadSleep = threadSleep;

        Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr = m_session.add<Echoes::Dbo::Alert>(alert);

        for (Wt::Json::Array::const_iterator idx2 = amsId.begin() ; idx2 < amsId.end(); idx2++)
        {
            Wt::WString tmp = idx2->toString();
            Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr = m_session.find<Echoes::Dbo::AlertMediaSpecialization>().where("\"AMS_ID\" = ?").bind(tmp);
            
            
            Wt::Dbo::ptr<Echoes::Dbo::UserRole> uroPtr = m_session.find<Echoes::Dbo::UserRole>()
                .where("\"URO_ID\" = ?").bind(amsPtr->mediaValue->user->userRole);
        
            if(!uroPtr)
            {
                Wt::log("info") << "user_role not found or not available";
                res = Echoes::Dbo::EReturnCode::NOT_FOUND;
                responseMsg = "{\"message\":\"Role not found\"}";
                return res;
            }
            
            
            amsPtr.modify()->alert = alePtr;
            
            Echoes::Dbo::AlertMessageDefinition *amd = new Echoes::Dbo::AlertMessageDefinition();
            amd->pk.alert = alePtr;
            amd->pk.media = amsPtr->mediaValue->media;
            amd->pk.userRole = uroPtr;
            amd->isCustom = false;
                        
            Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasAsset> aliasAsset = m_session.find<Echoes::Dbo::AlertMessageAliasAsset>()
                    .where("\"AAA_DELETE\" IS NULL")
                    .where("\"URO_ID_URO_ID\" = ?").bind(amsPtr->mediaValue->user->userRole)
                    .where("\"MED_ID_MED_ID\" = ?").bind(amsPtr->mediaValue->media)
                    .where("\"AST_ID_AST_ID\" = ?").bind(astId);
            
            Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasPlugin> aliasPlugin = m_session.find<Echoes::Dbo::AlertMessageAliasPlugin>()
                .where("\"AAP_DELETE\" IS NULL")
                .where("\"URO_ID_URO_ID\" = ?").bind(amsPtr->mediaValue->user->userRole)
                .where("\"MED_ID_MED_ID\" = ?").bind(amsPtr->mediaValue->media)
                .where("\"PLG_ID_PLG_ID\" = ?").bind(plgId);
            
            Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasInformation> aliasInformation = m_session.find<Echoes::Dbo::AlertMessageAliasInformation>()
                .where("\"AAI_DELETE\" IS NULL")
                .where("\"URO_ID_URO_ID\" = ?").bind(amsPtr->mediaValue->user->userRole)
                .where("\"MED_ID_MED_ID\" = ?").bind(amsPtr->mediaValue->media)
                .where("\"ING_ID_INF_ID\" = ?").bind(infId);
            
            Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasInformationCriteria> aliasCriteria = m_session.find<Echoes::Dbo::AlertMessageAliasInformationCriteria>()
                .where("\"AIC_DELETE\" IS NULL")
                .where("\"URO_ID_URO_ID\" = ?").bind(amsPtr->mediaValue->user->userRole)
                .where("\"MED_ID_MED_ID\" = ?").bind(amsPtr->mediaValue->media)
                .where("\"ING_ID_INF_ID\" = ?").bind(infId)
                .where("\"ACR_ID_ACR_ID\" = ?").bind(acrId);
                    
            switch (amsPtr->mediaValue->media.id())
            {
                case Echoes::Dbo::EMedia::SMS:
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
                        amd->message += " Application : " + plgPtr->name;
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
                        if(!critPtr->name.toUTF8().compare("lt"))
                        {
                            comp="Inférieur";
                        }
                        else if(!critPtr->name.toUTF8().compare("le"))
                        {
                            comp="Inférieur ou égal";
                        }
                        else if(!critPtr->name.toUTF8().compare("eq"))
                        {
                            comp="Egal";
                        }
                        else if(!critPtr->name.toUTF8().compare("ne"))
                        {
                            comp="Différent";
                        }
                        else if(!critPtr->name.toUTF8().compare("ge"))
                        {
                            comp="Supérieur ou égal";
                        }
                        else if(!critPtr->name.toUTF8().compare("gt"))
                        {
                            comp="Supérieur";
                        }
                        amd->message += " Critere : %value% " + comp + " %threshold% " + infoPtr->informationUnit->name.toUTF8();
                    }
//                    }
                    break;
                case Echoes::Dbo::EMedia::MAIL:
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
                        amd->message += " Application : " + plgPtr->name + "<br />";
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
                        amd->message += " Critere : " + aliasCriteria->alias + "<br />";
                    }
                    else
                    {
                        std::string comp;
                        if(!critPtr->name.toUTF8().compare("lt"))
                        {
                            comp="Inférieur";
                        }
                        else if(!critPtr->name.toUTF8().compare("le"))
                        {
                            comp="Inférieur ou égal";
                        }
                        else if(!critPtr->name.toUTF8().compare("eq"))
                        {
                            comp="Egal";
                        }
                        else if(!critPtr->name.toUTF8().compare("ne"))
                        {
                            comp="Différent";
                        }
                        else if(!critPtr->name.toUTF8().compare("ge"))
                        {
                            comp="Supérieur ou égal";
                        }
                        else if(!critPtr->name.toUTF8().compare("gt"))
                        {
                            comp="Supérieur";
                        }
                        amd->message += " Critere : %value% %unit%" + comp + " %threshold% %unit%<br />";
                    }

                    amd->message += "Plus d'informations sur https://alert.echoes-tech.com";
                    break;
            case Echoes::Dbo::EMedia::MOBILE_APP:
//                    amd->message = "[EA][%detection-time%] : ";

                    //TODO: à revoir pour les alertes complexes !!
//                    for (Wt::Dbo::collection<Wt::Dbo::ptr<InformationValue>>::const_iterator i = ivaPtrCollection.begin(); i != ivaPtrCollection.end(); ++i)
//                    {
                    
                    if (aliasAsset)
                    {
                        amd->message = "Serveur : " + aliasAsset->alias;
                    }
                    else
                    {
                        amd->message = "Serveur : " + assetPtr->name;
                    }

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
                        amd->message += " Application : " + plgPtr->name;
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
                        if(!critPtr->name.toUTF8().compare("lt"))
                        {
                            comp="Inferieur";
                        }
                        else if(!critPtr->name.toUTF8().compare("le"))
                        {
                            comp="Inferieur ou egal";
                        }
                        else if(!critPtr->name.toUTF8().compare("eq"))
                        {
                            comp="Egal";
                        }
                        else if(!critPtr->name.toUTF8().compare("ne"))
                        {
                            comp="Different";
                        }
                        else if(!critPtr->name.toUTF8().compare("ge"))
                        {
                            comp="Superieur ou egal";
                        }
                        else if(!critPtr->name.toUTF8().compare("gt"))
                        {
                            comp="Superieur";
                        }
                        amd->message += " Critere : %value% %unit% " + comp + " %threshold% %unit%";
                      }
                    break;
                default:
                    Wt::log("error") << "[Alert Resource] Unknown ID Media: " << amsPtr->mediaValue->media.id();
                    break;
            }

            Wt::Dbo::ptr<Echoes::Dbo::AlertMessageDefinition> amdPtr = m_session.add<Echoes::Dbo::AlertMessageDefinition>(amd);
            amdPtr.flush();
        }
        alePtr.flush();
        alePtr.modify()->setId(alePtr.id());
        responseMsg = alePtr->toJSON();
        transaction.commit();

        res = 201;
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = Echoes::Dbo::EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    
    return res;
}

unsigned short AlertResource::sendMAIL
(
 Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrCollection,
 Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr,
 Wt::Dbo::ptr<Echoes::Dbo::AlertMessageDefinition> amdPtr,
 Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> atrPtr,
 Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr,
 bool overSMSQuota
)
{
    unsigned short res = Echoes::Dbo::EReturnCode::INTERNAL_SERVER_ERROR;

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
    for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationValue> >::const_iterator i = ivaPtrCollection.begin(); i != ivaPtrCollection.end(); ++i)
    {
        boost::replace_all(mailBody, "%value%", i->get()->value.toUTF8());
        boost::replace_all(mailBody, "%threshold%", alePtr->alertValue->value.toUTF8());
        boost::replace_all(mailBody, "%detection-time%", i->get()->creationDate.toString().toUTF8());
        boost::replace_all(mailBody, "%alerting-time%", now.toString().toUTF8());
        boost::replace_all(mailBody, "%unit%", i->get()->information->informationUnit->name.toUTF8());
    }

    Wt::log("info") << " [Alert Resource] " << mailBody;

    mailMessage.setFrom(Wt::Mail::Mailbox(conf.getAlertMailSenderAddress(), conf.getAlertMailSenderName()));
    mailMessage.addRecipient(Wt::Mail::To, Wt::Mail::Mailbox(mailRecipient.toUTF8(), mailRecipientName));
    mailMessage.setSubject("[ECHOES Alert] " + alePtr->name);
    mailMessage.addHtmlBody(mailBody);
    mailClient.connect(conf.getSMTPHost(), conf.getSMTPPort());
    mailClient.send(mailMessage);

    Wt::log("info") << " [Class:AlertSender] " << "insert date of last send in db : " << now.toString();
    amsPtr.modify()->lastSend = now;

    atrPtr.modify()->sendDate = now;
    atrPtr.modify()->content = mailBody;

    res = Echoes::Dbo::EReturnCode::OK;

    return res;
}

unsigned short AlertResource::sendSMS
(
 Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrCollection,
 Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr,
 Wt::Dbo::ptr<Echoes::Dbo::AlertMessageDefinition> amdPtr,
 Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> atrPtr,
 Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr
)
{
    unsigned short res = Echoes::Dbo::EReturnCode::INTERNAL_SERVER_ERROR;
    
    const Wt::WDateTime now = Wt::WDateTime::currentDateTime(); //for setting the send date of the alert
    string sms = amdPtr->message.toUTF8();

    //TODO: à revoir pour les alertes complexes !!
    for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationValue> >::const_iterator i = ivaPtrCollection.begin(); i != ivaPtrCollection.end(); ++i)
    {
          boost::replace_all(sms, "%value%", i->get()->value.toUTF8());
          boost::replace_all(sms, "%threshold%", alePtr->alertValue->value.toUTF8());
          boost::replace_all(sms, "%detection-time%", i->get()->creationDate.toString().toUTF8());
          boost::replace_all(sms, "%alerting-time%", now.toString().toUTF8());
    }

    Wt::log("info") << " [Alert Resource] New SMS for " << amsPtr->mediaValue->value << " : " << sms;

    ItookiSMSSender itookiSMSSender(amsPtr->mediaValue->value.toUTF8(), sms, this);
    itookiSMSSender.setAlertTrackingPtr(atrPtr);
    
    atrPtr.modify()->content = sms;
    
    if(!itookiSMSSender.send())
    {
        amsPtr.modify()->lastSend = now;
        res = Echoes::Dbo::EReturnCode::OK;
    }

    return res;
}

unsigned short AlertResource::sendMobileApp
(
 Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrCollection,
 Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr,
 Wt::Dbo::ptr<Echoes::Dbo::AlertMessageDefinition> amdPtr,
 Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> atrPtr,
 Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr
)
{
    unsigned short res = Echoes::Dbo::EReturnCode::INTERNAL_SERVER_ERROR;
    
    const Wt::WDateTime now = Wt::WDateTime::currentDateTime(); //for setting the send date of the alert
    string mobileApp = amdPtr->message.toUTF8();

    //TODO: à revoir pour les alertes complexes !!
    for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationValue> >::const_iterator i = ivaPtrCollection.begin(); i != ivaPtrCollection.end(); ++i)
    {
          boost::replace_all(mobileApp, "%value%", i->get()->value.toUTF8());
          boost::replace_all(mobileApp, "%threshold%", alePtr->alertValue->value.toUTF8());
          boost::replace_all(mobileApp, "%detection-time%", i->get()->creationDate.toString().toUTF8());
          boost::replace_all(mobileApp, "%alerting-time%", now.toString().toUTF8());
          boost::replace_all(mobileApp, "%unit%", i->get()->information->informationUnit->name.toUTF8());
    }

    Wt::log("info") << " [Alert Resource] New Alerte for mobileApp : " << amsPtr->mediaValue->value << " : " << mobileApp;
    
    atrPtr.modify()->content = mobileApp;
    atrPtr.modify()->sendDate = now;
    
    amsPtr.modify()->lastSend = now;
    res = Echoes::Dbo::EReturnCode::OK;
    

    return res;
}

unsigned short AlertResource::postAlertTracking(string &responseMsg, const string &sRequest)
{
    unsigned short res = Echoes::Dbo::EReturnCode::INTERNAL_SERVER_ERROR;
    string ivaIDWhereString = "";
    Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrCollection;

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
            Wt::Dbo::Transaction transaction(m_session);

            //TODO: check if this alert is set for the current user organisation
            Wt::Dbo::ptr<Echoes::Dbo::Alert> alertPtr = m_session.find<Echoes::Dbo::Alert>()
                    .where("\"ALE_ID\" = ?").bind(m_pathElements[1])
                    .where("\"ALE_DELETE\" IS NULL");

            if (alertPtr)
            {
                Wt::WDateTime now = Wt::WDateTime::currentDateTime();

                alertPtr.modify()->lastAttempt = now;

                //TODO: verifier si les IVA correspondent bien aux INF de l'alerte
                ivaPtrCollection = m_session.find<Echoes::Dbo::InformationValue>()
                    .where(ivaIDWhereString)
                    .where("\"IVA_DELETE\" IS NULL");

                for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> >::const_iterator i = alertPtr->alertMediaSpecializations.begin(); i != alertPtr->alertMediaSpecializations.end(); ++i)
                {
                    // it is the first time we send the alert there is no last send date filled
                    // or date.now() - last_send = nb_secs then, if nb_secs >= snooze of the media, we send the alert
                    if (!i->get()->lastSend.isValid() || i->get()->lastSend.secsTo(now) >= i->get()->snoozeDuration)
                    {
                        Echoes::Dbo::AlertTracking *newAlertTracking = new Echoes::Dbo::AlertTracking();
                        long long medID = i->get()->mediaValue->media.id();

                        newAlertTracking->alert = alertPtr;
                        newAlertTracking->mediaValue = i->get()->mediaValue;

                        // WARNING : SendDate must be set by the API when the alert was sent, not before
                        newAlertTracking->sendDate = *(new Wt::WDateTime());

                        Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> alertTrackingPtr = m_session.add<Echoes::Dbo::AlertTracking>(newAlertTracking);
                        alertTrackingPtr.flush();

                        Wt::log("info") << " [Alert Ressource] " << "Alert tracking number creation : " << alertTrackingPtr.id();

                        Wt::log("debug") << " [Alert Ressource] " << "snooze = " << i->get()->snoozeDuration;
                        
                        Wt::Dbo::ptr<Echoes::Dbo::AlertMessageDefinition> amdPtr = m_session.find<Echoes::Dbo::AlertMessageDefinition>()
                                .where("\"ALE_ID_ALE_ID\" = ?").bind(alertPtr.id())
                                .where("\"URO_ID_URO_ID\" = ?").bind(i->get()->mediaValue->user->userRole.id())
                                .where("\"MED_ID_MED_ID\" = ?").bind(medID)
                                .where("\"AMD_DELETE\" IS NULL")
                                .limit(1);

                        if (amdPtr)
                        {
                            switch (medID)
                            {
                                case Echoes::Dbo::EMedia::SMS:
                                {
                                    Wt::log("info") << " [Alert Ressource] " << "Media value SMS choosed for the alert : " << alertPtr->name;

                                    // Verifying the quota of sms
                                    Wt::Dbo::ptr<Echoes::Dbo::OptionValue> optionValuePtr = m_session.find<Echoes::Dbo::OptionValue>()
                                            .where("\"OPT_ID_OPT_ID\" = ?").bind(Echoes::Dbo::EOption::QUOTA_SMS)
                                            .where("\"ORG_ID_ORG_ID\" = ?").bind(i->get()->mediaValue->user->organization.id())
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
                                        res = Echoes::Dbo::EReturnCode::SERVICE_UNAVAILABLE;
                                        responseMsg = "{\n\t\"message\": \"Service Unavailable\"\n}";
                                    }
                                    break;
                                }
                                case Echoes::Dbo::EMedia::MAIL:
                                    Wt::log("info") << " [Alert Ressource] " << "Media value MAIL choosed for the alert : " << alertPtr->name;
                                    sendMAIL(ivaPtrCollection, alertPtr, amdPtr, alertTrackingPtr, *i);
                                    break;
                                case Echoes::Dbo::EMedia::MOBILE_APP:
                                Wt::log("info") << " [Alert Ressource] " << "Media value MOBILEAPP choosed for the alert : " << alertPtr->name;
                                sendMobileApp(ivaPtrCollection, alertPtr, amdPtr, alertTrackingPtr, *i);
                                break;
                                default:
                                    Wt::log("error") << "[Alert Resource] Unknown ID Media: " << medID;
                                    break;
                            }
                        }
                        else
                        {
                            Wt::log("debug") << "[Alert Resource] Undefined AlertMessageDefition";
                        }
                    }
                    else
                    {
                        Wt::log("debug") << "[Alert Resource] "
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
                res = Echoes::Dbo::EReturnCode::NOT_FOUND;
            }

            transaction.commit();
        }
        catch (Wt::Dbo::Exception const& e) 
        {
            Wt::log("error") << e.what();
            res = Echoes::Dbo::EReturnCode::SERVICE_UNAVAILABLE;
            responseMsg = "{\"message\":\"Service Unavailable\"}";
        }
    }

    return res;
}


void AlertResource::processPostRequest(Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";

    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        m_statusCode = postAlert(responseMsg, m_requestData);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();
            if(!nextElement.compare("trackings"))
            {
                m_statusCode = postAlertTracking(responseMsg, m_requestData);
            }
            else
            {
                m_statusCode = Echoes::Dbo::EReturnCode::BAD_REQUEST;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            m_statusCode = Echoes::Dbo::EReturnCode::BAD_REQUEST;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return ;
}

void AlertResource::processPutRequest(Wt::Http::Response &response)
{
    return;
}

void AlertResource::processPatchRequest(Wt::Http::Response &response)
{
    return;
}

unsigned short AlertResource::deleteAlert(string &responseMsg)
{
    unsigned short res = Echoes::Dbo::EReturnCode::INTERNAL_SERVER_ERROR;
    try 
    {

        Wt::Dbo::Transaction transaction(m_session);
        string queryStr = "SELECT ale FROM \"T_ALERT_ALE\" ale "
                " WHERE \"ALE_ID\" IN"
                " (SELECT \"AMS_ALE_ALE_ID\" FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" "
                " WHERE \"AMS_MEV_MEV_ID\" IN "
                " (SELECT \"MEV_ID\" FROM \"T_MEDIA_VALUE_MEV\" "
                " WHERE \"MEV_USR_USR_ID\" IN "
                " ( Select \"T_USER_USR_USR_ID\" "
                " FROM \"TJ_USR_ORG\" "
                " WHERE \"T_ORGANIZATION_ORG_ORG_ID\" = " + boost::lexical_cast<string>(m_session.user()->organization.id()) +
                " )))"
                " AND \"ALE_ID\" = " + m_pathElements[1];
        
        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Alert>> resQuery = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::Alert>>(queryStr);
        Wt::Dbo::ptr<Echoes::Dbo::Alert> alertPtr = resQuery.resultValue();
       // Wt::Dbo::ptr<Alert> alertPtr = session.find<Alert>().where("\"ALE_ID\" = ?").bind(boost::lexical_cast<int>(vPathElements[1])); 
        if (alertPtr)
        {
            alertPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
            transaction.commit();

            res = 204;
            responseMsg = "";
        }
        else
        {
            res = Echoes::Dbo::EReturnCode::NOT_FOUND;
            responseMsg = "{\"message\" : \"Alert not found\" }";
        }
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = Echoes::Dbo::EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;
}

void AlertResource::processDeleteRequest(Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";

    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        m_statusCode = Echoes::Dbo::EReturnCode::BAD_REQUEST;
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
                m_statusCode = deleteAlert(responseMsg);
            }
            else
            {
                m_statusCode = Echoes::Dbo::EReturnCode::BAD_REQUEST;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            m_statusCode = Echoes::Dbo::EReturnCode::BAD_REQUEST;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}


void AlertResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    m_media = "";
    
    if (!request.getParameterValues("media").empty())
    {
        m_media = request.getParameterValues("media")[0];
    }
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);
    return;
}

