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

AlertResource::~AlertResource()
{
}

Wt::Dbo::ptr<Echoes::Dbo::Alert> AlertResource::selectAlert(const long long &aleId, Echoes::Dbo::Session &session)
{
    return selectAlert(boost::lexical_cast<string>(aleId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::Alert> AlertResource::selectAlert(const string &aleId, Echoes::Dbo::Session &session)
{
    string queryStr =
" SELECT ale"
"   FROM " QUOTE("T_ALERT_ALE") " ale"
"   WHERE"
"     " QUOTE(TRIGRAM_ALERT ID) " = "
"       ("
"         SELECT " QUOTE(TRIGRAM_ALERT_MEDIA_SPECIALIZATION SEP TRIGRAM_ALERT SEP TRIGRAM_ALERT ID)
"           FROM " QUOTE("T_ALERT_MEDIA_SPECIALIZATION_AMS")
"           WHERE"
"             " QUOTE(TRIGRAM_ALERT_MEDIA_SPECIALIZATION SEP TRIGRAM_MEDIA SEP TRIGRAM_MEDIA ID) " IN"
"               ("
"                 SELECT " QUOTE(TRIGRAM_MEDIA ID)
"                   FROM " QUOTE(TRIGRAM_MEDIA SEP "VALUE")
"                   WHERE"
"                     " QUOTE(TRIGRAM_MEDIA SEP TRIGRAM_USER SEP TRIGRAM_USER ID) " IN"
"                       ("
"                         SELECT " QUOTE(TRIGRAM_USER ID)
"                           FROM " QUOTE("T_USER_USR")
"                           WHERE"
"                             " QUOTE(TRIGRAM_USER SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = " + boost::lexical_cast<string>(session.user()->organization.id()) +
"                             AND " QUOTE(TRIGRAM_USER SEP "DELETE") " IS NULL"
"                       )"
"                     AND " QUOTE(TRIGRAM_MEDIA SEP "DELETE") " IS NULL"
"               )"
"             AND " QUOTE(TRIGRAM_ALERT_MEDIA_SPECIALIZATION  SEP TRIGRAM_ALERT SEP TRIGRAM_ALERT ID) " = " + aleId +
"             AND " QUOTE(TRIGRAM_ALERT_MEDIA_SPECIALIZATION SEP "DELETE") " IS NULL"
"           LIMIT 1"
"       )"
"     AND " QUOTE(TRIGRAM_ALERT SEP "DELETE") " IS NULL";
 
    Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Alert>> queryRes = session.query<Wt::Dbo::ptr<Echoes::Dbo::Alert>>(queryStr);

    return queryRes.resultValue();
}

// FIXME
EReturnCode AlertResource::getRecipientsForAlert(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    unsigned idx = 0;
//    try 
//    {
//        Wt::Dbo::Transaction transaction(m_session);
//        
//         string queryString = "SELECT mev, ams FROM \"T_MEDIA_VALUE_MEV\" mev," 
//                    " \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" ams "
//                    "WHERE ams.\"AMS_ALE_ALE_ID\" = " + m_pathElements[1] +
//                    " AND ams.\"AMS_MEV_MEV_ID\" = mev.\"MEV_ID\" "
//                    " AND mev.\"MEV_USR_USR_ID\" IN" 
//                 "( Select \"T_USER_USR_USR_ID\" "
//                 "FROM \"TJ_USR_ORG\" "
//                "WHERE \"T_ORGANIZATION_ORG_ORG_ID\" = " + boost::lexical_cast<string > (m_session.user()->organization.id()) +
//                 ")";
//
//        Wt::Dbo::Query
//                <
//                boost::tuple
//                <
//                Wt::Dbo::ptr<Echoes::Dbo::Media>,
//                Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization>
//                >
//                , Wt::Dbo::DynamicBinding
//                > resQuery = m_session.query
//                <
//                boost::tuple
//                <
//                Wt::Dbo::ptr<Echoes::Dbo::Media>,
//                Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization>
//                >, Wt::Dbo::DynamicBinding
//                > (queryString);
//
//        Wt::Dbo::collection<boost::tuple<
//                Wt::Dbo::ptr<Echoes::Dbo::Media>,
//                Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> > > listTuples = resQuery.resultList();
//        if (listTuples.size() > 0) 
//        {
//            responseMsg = "[";
//            for (Wt::Dbo::collection<boost::tuple<
//                    Wt::Dbo::ptr<Echoes::Dbo::Media>,
//                    Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> > >::const_iterator i = listTuples.begin(); i != listTuples.end(); ++i) 
//            {
//                i->get<0>().modify()->setId(i->get<0>().id());
//                i->get<1>().modify()->setId(i->get<1>().id());
//                responseMsg += "\n{\n";
//                responseMsg += "media_value :" + i->get<0>()->toJSON();
//                responseMsg += ",\nmedia_specialization :" + i->get<1>()->toJSON();
//                responseMsg += "\n}";
//                 ++idx;
//                if(listTuples.size()-idx > 0)
//                {
//                    responseMsg += ",\n";
//                }
//                
//            }
//            responseMsg += "]";
//            res = EReturnCode::OK; 
//        } 
//        else 
//        {
//            res = EReturnCode::NOT_FOUND;
//            responseMsg = "{\"message\":\"Alert not found\"}";
//        }
//        transaction.commit();
//    }                    
//    catch (Wt::Dbo::Exception const& e) 
//    {
//        Wt::log("error") << e.what();
//        res = EReturnCode::SERVICE_UNAVAILABLE;
//        responseMsg = "{\"message\":\"Service Unavailable\"}";
//    }
    return res;
}

EReturnCode AlertResource::getTrackingAlertMessage(std::string &responseMsg)
{
    responseMsg="";
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
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
            res = EReturnCode::OK;

        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = "{\"message\":\"Not found\"}";
        }

        transaction.commit();
        
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;
}

// FIXME
EReturnCode AlertResource::getTrackingAlertList(string &responseMsg)
{
    responseMsg="";
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    int idx = 0;
//    try
//    {
//        Wt::Dbo::Transaction transaction(m_session);
//
//        string queryString = "SELECT ale, mev, atr FROM \"T_ALERT_TRACKING_ATR\" atr, \"T_ALERT_ALE\" ale , \"T_MEDIA_VALUE_MEV\" mev "
//            " WHERE atr.\"ATR_ALE_ALE_ID\" = ale.\"ALE_ID\" "
//            " AND atr.\"ATR_MEV_MEV_ID\" = mev.\"MEV_ID\" "
//            " AND mev.\"MEV_USR_USR_ID\" IN"
//            "("
//                "SELECT \"T_USER_USR_USR_ID\" FROM \"TJ_USR_ORG\" WHERE \"T_ORGANIZATION_ORG_ORG_ID\" = " 
//                + boost::lexical_cast<string>(m_session.user()->organization.id()) + " "
//            ")"
//            " order by \"ATR_SEND_DATE\" desc "
//            " limit 50";
//        Wt::Dbo::Query
//                <
//                boost::tuple
//                <
//                Wt::Dbo::ptr<Echoes::Dbo::Alert>,
//                Wt::Dbo::ptr<Echoes::Dbo::Media>,
//                Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> 
//                >
//                ,Wt::Dbo::DynamicBinding
//                >resQuery = m_session.query
//                <
//                boost::tuple
//                <
//                Wt::Dbo::ptr<Echoes::Dbo::Alert>,
//                Wt::Dbo::ptr<Echoes::Dbo::Media>,
//                Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> 
//                >
//                ,Wt::Dbo::DynamicBinding>(queryString);
//
//        Wt::Dbo::collection<boost::tuple
//                <
//                Wt::Dbo::ptr<Echoes::Dbo::Alert>,
//                Wt::Dbo::ptr<Echoes::Dbo::Media>,
//                Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> 
//                > > listTuples = resQuery.resultList();
//
//        if (listTuples.size() > 0)
//        {
//            responseMsg = "[";
//            for (Wt::Dbo::collection<boost::tuple<
//                 Wt::Dbo::ptr<Echoes::Dbo::Alert>,
//                 Wt::Dbo::ptr<Echoes::Dbo::Media>,
//                 Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> > >::const_iterator i = listTuples.begin(); i != listTuples.end(); ++i) 
//            {
//                i->get<0>().modify()->setId(i->get<0>().id());
//                i->get<1>().modify()->setId(i->get<1>().id());
//                i->get<2>().modify()->setId(i->get<2>().id());
//                
//                responseMsg += "\n{\n";
//                responseMsg +="\"alert\" :" + i->get<0>()->toJSON();
//                responseMsg +=",\n\"media_value\" :" + i->get<1>()->toJSON();
//                responseMsg +=",\n\"alert_tracking\" :" + i->get<2>()->toJSON();
//                responseMsg += "\n}";
//                 ++idx;
//                if(listTuples.size()-idx > 0)
//                {
//                    responseMsg += ",\n";
//                }
//            }
//            responseMsg += "\n]\n";
//            res = EReturnCode::OK;
//
//        }
//        else
//        {
//            res = EReturnCode::NOT_FOUND;
//            responseMsg = "{\"message\":\"Tracking alert not found\"}";
//        }
//
//        transaction.commit();
//    }
//    catch (Wt::Dbo::Exception const& e) 
//    {
//        Wt::log("error") << e.what();
//        res = EReturnCode::SERVICE_UNAVAILABLE;
//        responseMsg = "{\"message\":\"Service Unavailable\"}";
//    }
    return res;
}

EReturnCode AlertResource::getAlerts(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
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

                ++idx;
                if(listTuples.size()-idx > 0)
                {
                    responseMsg += ",\n";
                }

            }
            responseMsg += "\n]";

            res = EReturnCode::OK; 
        } 
        else 
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = "{\"message\":\"Alert not found\"}";
        }
        transaction.commit();
    }                    
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
    }
    return res;
}

EReturnCode AlertResource::getAlert(std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
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
            res = EReturnCode::OK; 
        } 
        else 
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = "{\"message\":\"Alert not found\"}";
        }
        transaction.commit();
    }                    
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        res = EReturnCode::SERVICE_UNAVAILABLE;
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
                   m_statusCode = EReturnCode::BAD_REQUEST;
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
                 m_statusCode = EReturnCode::BAD_REQUEST;
                 responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                }
            }
            else
            {
               m_statusCode = EReturnCode::BAD_REQUEST;
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
                    m_statusCode = EReturnCode::BAD_REQUEST;
                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                }
            }
            catch (boost::bad_lexical_cast &)
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
    }
   
    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}


EReturnCode AlertResource::postAlert(string &responseMsg, const string &sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    // ALE attributs
    Wt::WString name;
    Wt::WString value;
    int threadSleep;

    // AVA attributs
    Wt::WString keyValue;
    long long idaId;
    long long acrId;

    // AMS attributs
    struct AmsStruct {
        long long medId;
        int snooze;
        Wt::WString message;
    };
    vector<AmsStruct> amsStructs;

    if(!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;                   
            Wt::Json::parse(sRequest, result);

            // ALE attributs
            name = result.get("name");
            value = result.get("value");
            threadSleep = result.get("thread_sleep");

            // AVA attributs
            keyValue = result.get("key_value");   
            idaId = result.get("information_data_id");
            acrId = result.get("alert_criteria_id");

            Wt::Json::Array amsAttributs = result.get("alert_media_specialization");
            for (Wt::Json::Array::const_iterator it = amsAttributs.begin() ; it < amsAttributs.end(); it++)
            {
                Wt::Json::Object tmp = *it;
                amsStructs.push_back(
                {
                    tmp.get("media_id"),
                    tmp.get("snooze"),
                    tmp.get("message")
                }
                );
            }
        }
        catch (Wt::Json::ParseError const& e)
        {
            res = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(res, e);
        }
        catch (Wt::Json::TypeException const& e)
        {
            res = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(res, e);
        }
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        responseMsg = httpCodeToJSON(res, "");
    }

    if(responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session);

            Wt::Dbo::ptr<Echoes::Dbo::InformationData> idaPtr = m_session.find<Echoes::Dbo::InformationData>()
                    .where(QUOTE(TRIGRAM_INFORMATION_DATA ID) " = ?").bind(idaId)
                    .where(QUOTE(TRIGRAM_INFORMATION_DATA SEP "DELETE") " IS NULL");
            if (!idaPtr)
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, idaPtr);
                return res;
            }

            Wt::Dbo::ptr<Echoes::Dbo::AlertCriteria> acrPtr = m_session.find<Echoes::Dbo::AlertCriteria>()
                    .where(QUOTE(TRIGRAM_ALERT_CRITERIA ID) " = ?").bind(acrId)
                    .where(QUOTE(TRIGRAM_ALERT_CRITERIA SEP "DELETE") " IS NULL");
            if (!acrPtr)
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, acrPtr);
                return res;
            }

            vector<Wt::Dbo::ptr<Echoes::Dbo::Media>> medPtrVector;
            for (vector<AmsStruct>::const_iterator it = amsStructs.begin() ; it < amsStructs.end(); it++)
            {
                Wt::Dbo::ptr<Echoes::Dbo::Media> medPtr = MediaResource::selectMedia(it->medId, m_session);
                if (!medPtr)
                {
                    res = EReturnCode::NOT_FOUND;
                    responseMsg = httpCodeToJSON(res, medPtr);
                    return res;
                }
                medPtrVector.push_back(medPtr);
            }

            Echoes::Dbo::AlertValue *ava = new Echoes::Dbo::AlertValue();

            ava->informationData = idaPtr;
            ava->alertCriteria = acrPtr;
            ava->value = value;
            ava->keyValue = keyValue;

            Wt::Dbo::ptr<Echoes::Dbo::AlertValue> newAvaPtr = m_session.add<Echoes::Dbo::AlertValue>(ava);
            newAvaPtr.flush();

            Echoes::Dbo::Alert *ale = new Echoes::Dbo::Alert();
            ale->alertValue = newAvaPtr;
            ale->name = name;
            ale->creaDate = Wt::WDateTime::currentDateTime();
            ale->threadSleep = threadSleep;

            Wt::Dbo::ptr<Echoes::Dbo::Alert> newAlePtr = m_session.add<Echoes::Dbo::Alert>(ale);
            newAlePtr.flush();

            for (vector<AmsStruct>::iterator it = amsStructs.begin() ; it < amsStructs.end(); it++)
            {
                Echoes::Dbo::AlertMediaSpecialization *newAms = new Echoes::Dbo::AlertMediaSpecialization();
                newAms->media = medPtrVector[distance(amsStructs.begin(), it)];
                newAms->alert = newAlePtr;
                newAms->snoozeDuration = it->snooze;
                newAms->notifEndOfAlert = false;
                newAms->message = it->message;

                Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> newAmsPtr = m_session.add<Echoes::Dbo::AlertMediaSpecialization>(newAms);
                newAmsPtr.flush();
            }

            res = serialize(newAlePtr, responseMsg, EReturnCode::CREATED);

            transaction.commit();
        }
        catch (Wt::Dbo::Exception const& e)
        {
            res = EReturnCode::SERVICE_UNAVAILABLE;
            responseMsg = httpCodeToJSON(res, e);
        }
    }

    return res;
}

EReturnCode AlertResource::sendMAIL
(
 vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrVector,
 Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr,
 Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> atrPtr,
 Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr,
 bool overSMSQuota
)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    Wt::WString mailRecipient;
    const Wt::WString mailRecipientName = amsPtr->media->user->firstName + " " + amsPtr->media->user->lastName ;
    string mailBody = "";
    const Wt::WDateTime now = Wt::WDateTime::currentDateTime(); //for setting the send date of the alert
    Wt::Mail::Message mailMessage;
    Wt::Mail::Client mailClient;

    // Normal case
    if (overSMSQuota == 0)
    {
        mailRecipient = amsPtr->media->value;
    }
    else if (overSMSQuota == 1)
    {
        mailRecipient = amsPtr->media->user->eMail;

        mailBody += "MAIL sent instead of SMS (quota = 0) <br />";
    }

    mailBody += amsPtr->message.toUTF8();

    // FIXME
    //TODO: à revoir pour les alertes complexes !!
//    for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationValue> >::const_iterator i = ivaPtrCollection.begin(); i != ivaPtrCollection.end(); ++i)
//    {
//        boost::replace_all(mailBody, "%value%", i->get()->value.toUTF8());
//        boost::replace_all(mailBody, "%threshold%", alePtr->alertValue->value.toUTF8());
//        boost::replace_all(mailBody, "%detection-time%", i->get()->creationDate.toString().toUTF8());
//        boost::replace_all(mailBody, "%alerting-time%", now.toString().toUTF8());
//        // FIXME
////        boost::replace_all(mailBody, "%unit%", i->get()->information->informationUnit->name.toUTF8());
//    }

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

    res = EReturnCode::OK;

    return res;
}

EReturnCode AlertResource::sendSMS
(
 vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrVector,
 Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr,
 Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> atrPtr,
 Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr
)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    
    const Wt::WDateTime now = Wt::WDateTime::currentDateTime(); //for setting the send date of the alert
    string sms = amsPtr->message.toUTF8();

    //FIXME
    //TODO: à revoir pour les alertes complexes !!
//    for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationValue> >::const_iterator i = ivaPtrCollection.begin(); i != ivaPtrCollection.end(); ++i)
//    {
//          boost::replace_all(sms, "%value%", i->get()->value.toUTF8());
//          boost::replace_all(sms, "%threshold%", alePtr->alertValue->value.toUTF8());
//          boost::replace_all(sms, "%detection-time%", i->get()->creationDate.toString().toUTF8());
//          boost::replace_all(sms, "%alerting-time%", now.toString().toUTF8());
//    }

    Wt::log("info") << " [Alert Resource] New SMS for " << amsPtr->media->value << " : " << sms;

    ItookiSMSSender itookiSMSSender(amsPtr->media->value.toUTF8(), sms, this);
    itookiSMSSender.setAlertTrackingPtr(atrPtr);
    
    atrPtr.modify()->content = sms;
    
    if(!itookiSMSSender.send())
    {
        amsPtr.modify()->lastSend = now;
        res = EReturnCode::OK;
    }

    return res;
}

EReturnCode AlertResource::sendMobileApp
(
 vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrVector,
 Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr,
 Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> atrPtr,
 Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr
)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    
    const Wt::WDateTime now = Wt::WDateTime::currentDateTime(); //for setting the send date of the alert
    string mobileApp = amsPtr->message.toUTF8();

    //FIXME
    //TODO: à revoir pour les alertes complexes !!
//    for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationValue> >::const_iterator i = ivaPtrCollection.begin(); i != ivaPtrCollection.end(); ++i)
//    {
//          boost::replace_all(mobileApp, "%value%", i->get()->value.toUTF8());
//          boost::replace_all(mobileApp, "%threshold%", alePtr->alertValue->value.toUTF8());
//          boost::replace_all(mobileApp, "%detection-time%", i->get()->creationDate.toString().toUTF8());
//          boost::replace_all(mobileApp, "%alerting-time%", now.toString().toUTF8());
//          // FIXME
////          boost::replace_all(mobileApp, "%unit%", i->get()->information->informationUnit->name.toUTF8());
//    }

    Wt::log("info") << " [Alert Resource] New Alerte for mobileApp : " << amsPtr->media->value << " : " << mobileApp;
    
    atrPtr.modify()->content = mobileApp;
    atrPtr.modify()->sendDate = now;
    
    amsPtr.modify()->lastSend = now;
    res = EReturnCode::OK;
    

    return res;
}

EReturnCode AlertResource::postAlertTracking(string &responseMsg, const string &sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    vector<long long> ivaIds;

    if(!sRequest.empty())
    {    
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);
            
            Wt::Json::Array array = result.get("information_value_ids"); 
            for (Wt::Json::Array::const_iterator it = array.begin(); it != array.end(); ++it)
            {
                ivaIds.push_back(it->toNumber());
            }
        }
        catch (Wt::Json::ParseError const& e)
        {
            res = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(res, e);
        }
        catch (Wt::Json::TypeException const& e)
        {
            res = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(res, e);
        }
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        responseMsg = httpCodeToJSON(res, "");
    }

    if(responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session);

            Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr = selectAlert(m_pathElements[1], m_session);
            if (!alePtr)
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, alePtr);
                return res;
            }

            vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrVector;
            for (vector<long long>::const_iterator it = ivaIds.begin() ; it < ivaIds.end(); it++)
            {
                Wt::Dbo::ptr<Echoes::Dbo::InformationValue> ivaPtr = m_session.find<Echoes::Dbo::InformationValue>()
                    .where(QUOTE(TRIGRAM_INFORMATION_VALUE ID) " = ?").bind(*it)
                    .where(QUOTE(TRIGRAM_INFORMATION_VALUE SEP "DELETE") " IS NULL");
                if (!ivaPtr)
                {
                    res = EReturnCode::NOT_FOUND;
                    responseMsg = httpCodeToJSON(res, ivaPtr);
                    return res;
                }
                ivaPtrVector.push_back(ivaPtr);
            }

            // Check whether the number and IDs of IVA concord with the alert definition
            if (alePtr->alertSequence)
            {
                for (vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>>::const_iterator it = ivaPtrVector.begin() ; it < ivaPtrVector.end(); it++)
                {
                    //TODO(FPO): To Be Coded when we will implement the Alert Sequence
                }
            }
            else
            {
                if (ivaPtrVector.size() != 1 || ivaPtrVector[0]->informationData != alePtr->alertValue->informationData)
                {
                    m_statusCode = EReturnCode::BAD_REQUEST;
                    responseMsg = httpCodeToJSON(m_statusCode, "");
                }
            }

            Wt::WDateTime now = Wt::WDateTime::currentDateTime();

            alePtr.modify()->lastAttempt = now;

            for (Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> >::const_iterator it = alePtr->alertMediaSpecializations.begin(); it != alePtr->alertMediaSpecializations.end(); ++it)
            {
                // it is the first time we send the alert there is no last send date filled
                // or date.now() - last_send = nb_secs then, if nb_secs >= snooze of the media, we send the alert
                if (!it->get()->lastSend.isValid() || it->get()->lastSend.secsTo(now) >= it->get()->snoozeDuration)
                {
                    const long long mtyID = it->get()->media->mediaType.id();
                    Echoes::Dbo::AlertTracking *newAtr = new Echoes::Dbo::AlertTracking();

                    newAtr->alert = alePtr;
                    newAtr->media = it->get()->media;
                    // WARNING : SendDate must be set by the API when the alert was sent, not before
                    newAtr->sendDate = *(new Wt::WDateTime());

                    Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> newAtrPtr = m_session.add<Echoes::Dbo::AlertTracking>(newAtr);
                    newAtrPtr.flush();

                    Wt::log("info") << " [Alert Ressource] " << "Alert tracking number creation : " << newAtrPtr.id();

                    Wt::log("debug") << " [Alert Ressource] " << "snooze = " << it->get()->snoozeDuration;

                    switch (mtyID)
                    {
                        case Echoes::Dbo::EMedia::SMS:
                        {
                            Wt::log("info") << " [Alert Ressource] " << "Media value SMS choosed for the alert : " << alePtr->name;

                            // Verifying the quota of sms
                            Wt::Dbo::ptr<Echoes::Dbo::Option> optPtr = m_session.find<Echoes::Dbo::Option>()
                                    .where(QUOTE(TRIGRAM_OPTION SEP TRIGRAM_OPTION_TYPE SEP TRIGRAM_OPTION_TYPE ID) " = ?").bind(Echoes::Dbo::EOption::QUOTA_SMS)
                                    .where(QUOTE(TRIGRAM_OPTION SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(it->get()->media->user->organization.id())
                                    .limit(1);

                            try
                            {
                                const int smsQuota = boost::lexical_cast<int>(optPtr->value); 
                                if (smsQuota == 0)
                                {

                                    Wt::log("info") << " [Alert Ressource] " << "SMS quota 0 for alert : " <<  alePtr->name;
                                    Wt::log("info") << " [Alert Ressource] " << "Sending e-mail instead." ;

                                    sendMAIL(ivaPtrVector, alePtr, newAtrPtr, *it, true);
                                }
                                else
                                {
                                    Wt::log("debug") << " [Alert Ressource] " << "We send a SMS, quota : "<< smsQuota;
                                    optPtr.modify()->value = boost::lexical_cast<string>(smsQuota - 1);
                                    optPtr.flush();                   
                                    sendSMS(ivaPtrVector, alePtr, newAtrPtr, *it); 
                                }
                            }
                            catch(boost::bad_lexical_cast const& e)
                            {
                                res = EReturnCode::SERVICE_UNAVAILABLE;
                                responseMsg = httpCodeToJSON(res, e);
                            }
                            break;
                        }
                        case Echoes::Dbo::EMedia::MAIL:
                            Wt::log("info") << " [Alert Ressource] " << "Media value MAIL choosed for the alert : " << alePtr->name;
                            sendMAIL(ivaPtrVector, alePtr, newAtrPtr, *it);
                            break;
                        case Echoes::Dbo::EMedia::MOBILE_APP:
                        Wt::log("info") << " [Alert Ressource] " << "Media value MOBILEAPP choosed for the alert : " << alePtr->name;
                        sendMobileApp(ivaPtrVector, alePtr, newAtrPtr, *it);
                        break;
                        default:
                            Wt::log("error") << "[Alert Resource] Unknown ID Media: " << mtyID;
                            break;
                    }
                }
                else
                {
                    Wt::log("debug") << "[Alert Resource] "
                            << "Last time we send the alert : " << alePtr->name
                            << "was : " << it->get()->lastSend.toString()
                            << "the snooze for the media " << it->get()->media->mediaType->name
                            << " is : " << it->get()->snoozeDuration << "secs,  it's not the time to send the alert";  
                }
            }
            res = EReturnCode::CREATED;

            transaction.commit();
        }
        catch (Wt::Dbo::Exception const& e) 
        {
            res = EReturnCode::SERVICE_UNAVAILABLE;
            responseMsg = httpCodeToJSON(res, e);
        }
    }

    return res;
}


void AlertResource::processPostRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if(nextElement.empty())
    {
        m_statusCode = postAlert(responseMsg, m_requestData);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();
            if(nextElement.compare("trackings") == 0)
            {
                m_statusCode = postAlertTracking(responseMsg, m_requestData);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                responseMsg = httpCodeToJSON(m_statusCode, "");
            }
        }
        catch (boost::bad_lexical_cast const& e)
        {
            m_statusCode = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(m_statusCode, e);
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

// FIXME
EReturnCode AlertResource::deleteAlert(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    try 
//    {
//
//        Wt::Dbo::Transaction transaction(m_session);
//        string queryStr = "SELECT ale FROM \"T_ALERT_ALE\" ale "
//                " WHERE \"ALE_ID\" IN"
//                " (SELECT \"AMS_ALE_ALE_ID\" FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" "
//                " WHERE \"AMS_MEV_MEV_ID\" IN "
//                " (SELECT \"MEV_ID\" FROM \"T_MEDIA_VALUE_MEV\" "
//                " WHERE \"MEV_USR_USR_ID\" IN "
//                " ( Select \"T_USER_USR_USR_ID\" "
//                " FROM \"TJ_USR_ORG\" "
//                " WHERE \"T_ORGANIZATION_ORG_ORG_ID\" = " + boost::lexical_cast<string>(m_session.user()->organization.id()) +
//                " )))"
//                " AND \"ALE_ID\" = " + m_pathElements[1];
//        
//        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Alert>> resQuery = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::Alert>>(queryStr);
//        Wt::Dbo::ptr<Echoes::Dbo::Alert> alertPtr = resQuery.resultValue();
//       // Wt::Dbo::ptr<Alert> alertPtr = session.find<Alert>().where("\"ALE_ID\" = ?").bind(boost::lexical_cast<int>(vPathElements[1])); 
//        if (alertPtr)
//        {
//            alertPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
//            transaction.commit();
//
//            res = EReturnCode::NO_CONTENT;
//            responseMsg = "";
//        }
//        else
//        {
//            res = EReturnCode::NOT_FOUND;
//            responseMsg = "{\"message\" : \"Alert not found\" }";
//        }
//    }
//    catch (Wt::Dbo::Exception const& e) 
//    {
//        Wt::log("error") << e.what();
//        res = EReturnCode::SERVICE_UNAVAILABLE;
//        responseMsg = "{\"message\":\"Service Unavailable\"}";
//    }
    return res;
}

void AlertResource::processDeleteRequest(Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";

    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
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
                m_statusCode = EReturnCode::BAD_REQUEST;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            m_statusCode = EReturnCode::BAD_REQUEST;
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

