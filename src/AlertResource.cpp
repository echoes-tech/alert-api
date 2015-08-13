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

AlertResource::AlertResource(Echoes::Dbo::Session& session) : PublicApiResource::PublicApiResource(session)
{
}

AlertResource::~AlertResource()
{
}

Wt::Dbo::ptr<Echoes::Dbo::Alert> AlertResource::selectAlert(const long long &aleId, const long long &orgId, Echoes::Dbo::Session &session)
{
    return selectAlert(boost::lexical_cast<string>(aleId), boost::lexical_cast<string>(orgId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::Alert> AlertResource::selectAlert(const string &aleId, const long long &orgId, Echoes::Dbo::Session &session)
{
    return selectAlert(aleId, boost::lexical_cast<string>(orgId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::Alert> AlertResource::selectAlert(const string &aleId, const string &orgId, Echoes::Dbo::Session &session)
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
"                   FROM " QUOTE("T_MEDIA_MED")
"                   WHERE"
"                     " QUOTE(TRIGRAM_MEDIA SEP TRIGRAM_USER SEP TRIGRAM_USER ID) " IN"
"                       ("
"                         SELECT " QUOTE(TRIGRAM_USER ID)
"                           FROM " QUOTE("T_USER_USR")
"                           WHERE"
"                             " QUOTE(TRIGRAM_USER SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = " + orgId +
"                             AND " QUOTE(TRIGRAM_USER SEP "DELETE") " IS NULL"
"                       )"
"                     AND " QUOTE(TRIGRAM_MEDIA SEP "DELETE") " IS NULL"
"               )"
"             AND " QUOTE(TRIGRAM_ALERT_MEDIA_SPECIALIZATION SEP TRIGRAM_ALERT SEP TRIGRAM_ALERT ID) " = " + aleId +
"             AND " QUOTE(TRIGRAM_ALERT_MEDIA_SPECIALIZATION SEP "DELETE") " IS NULL"
"           LIMIT 1"
"       )"
"     AND " QUOTE(TRIGRAM_ALERT SEP "DELETE") " IS NULL";

    Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Alert>> queryRes = session.query<Wt::Dbo::ptr<Echoes::Dbo::Alert>>(queryStr);

    return queryRes.resultValue();
}

EReturnCode AlertResource::getAlertsList(map<string, long long> &parameters, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);
        string queryStr =
" SELECT ale"
"   FROM " QUOTE("T_ALERT_ALE") " ale"
"   WHERE"
"     " QUOTE(TRIGRAM_ALERT ID) " IN"
"       ("
"         SELECT " QUOTE(TRIGRAM_ALERT_MEDIA_SPECIALIZATION SEP TRIGRAM_ALERT SEP TRIGRAM_ALERT ID)
"           FROM " QUOTE("T_ALERT_MEDIA_SPECIALIZATION_AMS")
"           WHERE"
"             " QUOTE(TRIGRAM_ALERT_MEDIA_SPECIALIZATION SEP TRIGRAM_MEDIA SEP TRIGRAM_MEDIA ID) " IN"
"               ("
"                 SELECT " QUOTE(TRIGRAM_MEDIA ID)
"                   FROM " QUOTE("T_MEDIA_MED")
"                   WHERE"
"                     " QUOTE(TRIGRAM_MEDIA SEP TRIGRAM_USER SEP TRIGRAM_USER ID) " IN"
"                       ("
"                         SELECT " QUOTE(TRIGRAM_USER ID)
"                           FROM " QUOTE("T_USER_USR")
"                           WHERE"
"                             " QUOTE(TRIGRAM_USER SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = " + boost::lexical_cast<string>(orgId) +
"                             AND " QUOTE(TRIGRAM_USER SEP "DELETE") " IS NULL"
"                       )";

        if (parameters["media_type"] > 0)
        {
            queryStr +=
"                     AND " QUOTE(TRIGRAM_MEDIA SEP TRIGRAM_MEDIA_TYPE SEP TRIGRAM_MEDIA_TYPE ID) " = " + boost::lexical_cast<string>(parameters["media_type"]);
        }

        queryStr +=
"                     AND " QUOTE(TRIGRAM_MEDIA SEP "DELETE") " IS NULL"
"               )"
"             AND " QUOTE(TRIGRAM_ALERT_MEDIA_SPECIALIZATION SEP "DELETE") " IS NULL"
"       )"
"     AND " QUOTE(TRIGRAM_ALERT SEP "DELETE") " IS NULL"
"   ORDER BY " QUOTE(TRIGRAM_ALERT ID);
        cout << queryStr << endl;
        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Alert>> queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::Alert>>(queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Alert>> alePtrCol = queryRes.resultList();

        res = serialize(alePtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode AlertResource::getAlert(const vector<string> &pathElements, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr = selectAlert(pathElements[1], orgId, m_session);

        res = serialize(alePtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode AlertResource::getAlertEvent(map<string, long long> &parameters, const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg)
{
     EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        //ToDo(FPO): Check rights
        Wt::Dbo::ptr < Echoes::Dbo::AlertTrackingEvent > atrPtrCol = m_session.find<Echoes::Dbo::AlertTrackingEvent>()
                .where(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP TRIGRAM_ALERT SEP TRIGRAM_ALERT ID)" = ? ").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP ID)" = ? ").bind(pathElements[3])
                .where(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP "DELETE") " IS NULL");

        res = serialize(atrPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode AlertResource::getAlertEvents(map<string, long long> &parameters, const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        //ToDo(FPO): Check rights
        Wt::Dbo::collection < Wt::Dbo::ptr < Echoes::Dbo::AlertTrackingEvent >> atrPtrCol = m_session.find<Echoes::Dbo::AlertTrackingEvent>()
                .where(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP TRIGRAM_ALERT SEP TRIGRAM_ALERT ID)" = ? ").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP "DELETE") " IS NULL")
                .orderBy(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP "DATE") " ASC")
                .limit(20);

        res = serialize(atrPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;    
}

EReturnCode AlertResource::getAlertStatus(map<string, long long> &parameters, const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        //ToDo(FPO): Check rights
        Wt::Dbo::collection < Wt::Dbo::ptr < Echoes::Dbo::AlertTrackingEvent >> atrPtrCol = m_session.find<Echoes::Dbo::AlertTrackingEvent>()
                .where(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP TRIGRAM_ALERT SEP TRIGRAM_ALERT ID)" = ? ").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP "DELETE") " IS NULL")
                .orderBy(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP "DATE") " ASC")
                .limit(20);

        res = EReturnCode::OK;
        responseMsg = "{ \"\": \"" + AlertStatusToString(atrPtrCol.begin()->get()->statut->id) + "\" }";

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;  
}

std::string AlertResource::AlertStatusToString(int AlertStatusId)
{
    string returnValue = "error";
    
    switch(AlertStatusId)
    {
        case Echoes::Dbo::EAlertStatus::PENDING :
            returnValue = "pending";
            break;
        case Echoes::Dbo::EAlertStatus::SUPPORTED :
            returnValue = "supported";
            break;
        case Echoes::Dbo::EAlertStatus::FORWARDING :
            returnValue = "forwarding";
            break;
        case Echoes::Dbo::EAlertStatus::BACKTONORMAL :
            returnValue = "back to normal";
            break;
        default:
            returnValue = "error switch";
            break;
    }
    return returnValue;
}

EReturnCode AlertResource::getAlertMessages(map<string, long long> &parameters, const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        //ToDo(FPO): Check rights
        Wt::Dbo::collection < Wt::Dbo::ptr < Echoes::Dbo::Message >> atrPtrCol = m_session.find<Echoes::Dbo::Message>()
                .where(QUOTE(TRIGRAM_MESSAGE SEP TRIGRAM_ALERT SEP TRIGRAM_ALERT ID)" = ? ").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_MESSAGE SEP "DELETE") " IS NULL")
                .orderBy(QUOTE(TRIGRAM_MESSAGE SEP "DEST") " DESC")
                .limit(20);

        res = serialize(atrPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;    
}
 

//EReturnCode AlertResource::getTrackingsForAlertsList(map<string, long long> &parameters, const long long &orgId, std::string &responseMsg)
//{
//    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//
//    if (parameters["media_id"] <= 0)
//    {
//        res = EReturnCode::BAD_REQUEST;
//        const string err = "[Alert Resource] media is empty";
//        responseMsg = httpCodeToJSON(res, err);
//    }
//
//    try
//    {
//        Wt::Dbo::Transaction transaction(m_session, true);
//
//        //ToDo(FPO): Check rights
//        Wt::Dbo::collection < Wt::Dbo::ptr < Echoes::Dbo::Message >> atrPtrCol = m_session.find<Echoes::Dbo::Message>()
//                .where(QUOTE(TRIGRAM_MESSAGE SEP TRIGRAM_MEDIA SEP TRIGRAM_MEDIA ID)" = ? ").bind(parameters["media_id"])
//                .where(QUOTE(TRIGRAM_MESSAGE SEP "SEND_DATE") " IS NOT NULL")
//                .orderBy(QUOTE(TRIGRAM_MESSAGE SEP "SEND_DATE") " DESC")
//                .limit(20);
//
//        res = serialize(atrPtrCol, responseMsg);
//
//        transaction.commit();
//    }
//    catch (Wt::Dbo::Exception const& e)
//    {
//        res = EReturnCode::SERVICE_UNAVAILABLE;
//        responseMsg = httpCodeToJSON(res, e);
//    }
//    return res;
//}

EReturnCode AlertResource::processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    string nextElement = "";
    unsigned short indexPathElement = 1;
    vector<string> pathElements;
    map<string, long long> parameters;

    //TODO(FPO): Transform to media_type_id
    parameters["media_type"] = 0;
    parameters["media_id"] = 0;
    
    const string sRequest = processRequestParameters(request, pathElements, parameters);

    nextElement = getNextElementFromPath(indexPathElement, pathElements);

    if (nextElement.empty())
    {
        res = getAlertsList(parameters, orgId, responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);
            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.empty())
            {
                res = getAlert(pathElements, orgId, responseMsg);
            }
            else if(nextElement.compare("status") == 0)
            {
                res = getAlertStatus(parameters, pathElements, orgId, responseMsg);
            }
            else if(nextElement.compare("messages") == 0)
            {
                res = getAlertMessages(parameters, pathElements, orgId, responseMsg);
            }
            else if(nextElement.compare("events") == 0)
            {
                
                if (nextElement.empty())
                {
                    res = getAlertEvents(parameters, pathElements, orgId, responseMsg);
                }
                else
                {
                    boost::lexical_cast<unsigned long long>(nextElement);
                    nextElement = getNextElementFromPath(indexPathElement, pathElements);
                    if (nextElement.empty())
                    {
                        res = getAlertEvent(parameters, pathElements, orgId, responseMsg);
                    }
                    else
                    {
                        res = EReturnCode::BAD_REQUEST;
                        const string err = "[Alert Resource] bad nextElement";
                        responseMsg = httpCodeToJSON(res, err);
                    }
                }
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Alert Resource] bad nextElement";
                responseMsg = httpCodeToJSON(res, err);
            }
        }
        catch (boost::bad_lexical_cast const& e)
        {
            res = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(res, e);
        }
    }

    return res;
}

EReturnCode AlertResource::postAlert(const string &sRequest, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    // ALE attributs
    Wt::WString name;
    int threadSleep;

    // AVA attributs
    struct AvaStruct
    {
        Wt::WString value;
        Wt::WString keyValue;
        long long infId;
        long long astId;
        long long plgId;
        long long acrId;
        int booleanOperator;
        int flapping;
    };
    vector<AvaStruct> avaStructs;
    
    // ATS attributs
    struct AtsStruct
    {
        int start;
        int duration;
        Wt::WString days;
        Wt::WString months;
    };
    
    // AMS attributs
    struct AmsStruct
    {
        long long medId;
        int snooze;
        Wt::WString message;
        vector<AtsStruct> atsStructs;
    };
    vector<AmsStruct> amsStructs;
        
    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            // ALE attributs
            name = result.get("name");
            threadSleep = result.get("thread_sleep");

            // AVA attributs
            Wt::Json::Array avaAttributs = result.get("alert_values");
            for (Wt::Json::Array::const_iterator it = avaAttributs.begin(); it < avaAttributs.end(); ++it)
            {
                Wt::Json::Object tmp = *it;
                Wt::WString keyValue = "";
                if (!tmp.get("key_value").isNull())
                {
                    keyValue = tmp.get("key_value");
                }

                avaStructs.push_back(
                {
                    Wt::Utils::base64Decode(tmp.get("value")),
                    keyValue,
                    tmp.get("information_id"),
                    tmp.get("asset_id"),
                    tmp.get("plugin_id"),
                    tmp.get("alert_criterion_id"),
                    tmp.get("operator"),
                    tmp.get("flapping")
                }
                );
                            
            }
            
            // AMS attributs
            Wt::Json::Array amsAttributs = result.get("alert_media_specializations");
            for (Wt::Json::Array::const_iterator it = amsAttributs.begin(); it < amsAttributs.end(); ++it)
            {
                Wt::Json::Object tmp = *it;
                
                std::vector<struct AtsStruct> atsStructs;
                
                Wt::Json::Array atsArray = tmp.get("time_slots");
                for (unsigned int i = 0 ; i < atsArray.size() ; ++i)
                {
                    struct AtsStruct atsStruct;
                    
                    Wt::Json::Object timeSlot = atsArray[i];
                    atsStruct.start = timeSlot.get("start");
                    atsStruct.duration = timeSlot.get("duration");
                    atsStruct.days = timeSlot.get("days");
                    atsStruct.months = timeSlot.get("months");
                    atsStructs.push_back(atsStruct);
                }
                
                amsStructs.push_back(
                {
                    tmp.get("media_id"),
                    tmp.get("snooze"),
                    tmp.get("message"),
                    atsStructs
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
        const string err = "[Alert Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {            
            Wt::Dbo::Transaction transaction(m_session, true);

            Wt::Dbo::ptr<Echoes::Dbo::AlertSequence> lastAsePtr;
            Wt::Dbo::ptr<Echoes::Dbo::AlertSequence> firstAsePtr;
            for(vector<AvaStruct>::iterator it = avaStructs.begin(); it < avaStructs.end(); it++)
            {
                const string queryStr =
" SELECT ida"
"   FROM " QUOTE("T_INFORMATION_DATA" SEP TRIGRAM_INFORMATION_DATA) " ida"
"   WHERE"
"     " QUOTE(TRIGRAM_INFORMATION_DATA SEP TRIGRAM_FILTER SEP TRIGRAM_FILTER ID) " IN"
"       ("
"         SELECT " QUOTE(TRIGRAM_FILTER ID)
"           FROM " QUOTE("T_FILTER" SEP TRIGRAM_FILTER)
"           WHERE"
"             " QUOTE(TRIGRAM_FILTER SEP TRIGRAM_SEARCH SEP TRIGRAM_SEARCH ID) " IN"
"               ("
"                 SELECT " QUOTE(TRIGRAM_SEARCH ID)
"                   FROM " QUOTE("T_SEARCH" SEP TRIGRAM_SEARCH)
"                   WHERE"
"                     " QUOTE(TRIGRAM_SEARCH SEP TRIGRAM_SOURCE SEP TRIGRAM_SOURCE ID) " IN"
"                       ("
"                         SELECT " QUOTE(TRIGRAM_SOURCE ID)
"                           FROM " QUOTE("T_SOURCE" SEP TRIGRAM_SOURCE)
"                             WHERE"
"                               " QUOTE(TRIGRAM_SOURCE ID) " IN"
"                                 ("
"                                   SELECT " QUOTE("T_SOURCE" SEP TRIGRAM_SOURCE SEP TRIGRAM_SOURCE ID)
"                                     FROM " QUOTE("TJ" SEP TRIGRAM_PLUGIN SEP TRIGRAM_SOURCE)
"                                       WHERE " QUOTE("T_PLUGIN" SEP TRIGRAM_PLUGIN SEP TRIGRAM_PLUGIN ID) " IN"
"                                         ("
"                                           SELECT " QUOTE(TRIGRAM_PLUGIN ID)
"                                             FROM " QUOTE("T_PLUGIN" SEP TRIGRAM_PLUGIN)
"                                               WHERE"
"                                                 " QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = " + boost::lexical_cast<string>(orgId) +
"                                                 AND " QUOTE(TRIGRAM_PLUGIN ID) " = " + boost::lexical_cast<string>(it->plgId) +
"                                                 AND " QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL"
"                                         )"
"                                 )"
"                               AND " QUOTE(TRIGRAM_SOURCE SEP "DELETE") " IS NULL"
"                       )"
"                     AND " QUOTE(TRIGRAM_SEARCH SEP "DELETE") " IS NULL"
"               )"
"             AND " QUOTE(TRIGRAM_FILTER SEP "DELETE") " IS NULL"
"       )"
"     AND " QUOTE(TRIGRAM_INFORMATION_DATA SEP TRIGRAM_INFORMATION SEP TRIGRAM_INFORMATION ID) " = " + boost::lexical_cast<string>(it->infId) +
"     AND " QUOTE(TRIGRAM_INFORMATION_DATA SEP TRIGRAM_ASSET SEP TRIGRAM_ASSET ID) " = " + boost::lexical_cast<string>(it->astId) +
"     AND " QUOTE(TRIGRAM_INFORMATION_DATA SEP "DELETE") " IS NULL"
"   LIMIT 1";

                Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::InformationData>> queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::InformationData>>(queryStr);
                Wt::Dbo::ptr<Echoes::Dbo::InformationData> idaPtr = queryRes.resultValue();
                if (!idaPtr)
                {
                    res = EReturnCode::NOT_FOUND;
                    responseMsg = httpCodeToJSON(res, idaPtr);
                    return res;
                }

                Wt::Dbo::ptr<Echoes::Dbo::AlertCriteria> acrPtr = m_session.find<Echoes::Dbo::AlertCriteria>()
                        .where(QUOTE(TRIGRAM_ALERT_CRITERIA ID) " = ?").bind(it->acrId)
                        .where(QUOTE(TRIGRAM_ALERT_CRITERIA SEP "DELETE") " IS NULL");
                if (!acrPtr)
                {
                    res = EReturnCode::NOT_FOUND;
                    responseMsg = httpCodeToJSON(res, acrPtr);
                    return res;
                }
                Echoes::Dbo::AlertValue *ava = new Echoes::Dbo::AlertValue();

                ava->informationData = idaPtr;
                ava->alertCriteria = acrPtr;
                ava->flapping = it->flapping;
                ava->value = it->value;
                ava->keyValue = it->keyValue;

                Wt::Dbo::ptr<Echoes::Dbo::AlertValue> newAvaPtr = m_session.add<Echoes::Dbo::AlertValue>(ava);
                newAvaPtr.flush();     
                
                Echoes::Dbo::AlertSequence *ase = new Echoes::Dbo::AlertSequence(); 
                ase->alertValue = newAvaPtr;          
                ase->boolOperator = it->booleanOperator;    
                ase->firstParenthesis = false;  
                ase->secondParenthesis = false;               

                Wt::Dbo::ptr<Echoes::Dbo::AlertSequence> newAsePtr = m_session.add<Echoes::Dbo::AlertSequence>(ase);          
                newAsePtr.flush();   
                
                if(lastAsePtr)
                {
                    lastAsePtr.modify()->alertSequence = newAsePtr;
                }
                     
                if(!firstAsePtr)
                {
                    firstAsePtr = newAsePtr;
                }
                lastAsePtr = newAsePtr;   
            }

            vector<Wt::Dbo::ptr<Echoes::Dbo::Media>> medPtrVector;
            for (vector<AmsStruct>::const_iterator it = amsStructs.begin(); it < amsStructs.end(); ++it)
            {
                Wt::Dbo::ptr<Echoes::Dbo::Media> medPtr = MediaResource::selectMedia(it->medId, orgId, m_session);
                if (!medPtr)
                {
                    res = EReturnCode::NOT_FOUND;
                    responseMsg = httpCodeToJSON(res, medPtr);
                    return res;
                }
                medPtrVector.push_back(medPtr);
            }

            Echoes::Dbo::Alert *ale = new Echoes::Dbo::Alert();
            ale->alertSequence = firstAsePtr;
            ale->name = name;
            ale->creaDate = Wt::WDateTime::currentDateTime();
            ale->threadSleep = threadSleep;

            Wt::Dbo::ptr<Echoes::Dbo::Alert> newAlePtr = m_session.add<Echoes::Dbo::Alert>(ale);
            newAlePtr.flush();

            for (vector<AmsStruct>::iterator it = amsStructs.begin(); it < amsStructs.end(); ++it)
            {
                Echoes::Dbo::AlertMediaSpecialization *newAms = new Echoes::Dbo::AlertMediaSpecialization();
                newAms->media = medPtrVector[distance(amsStructs.begin(), it)];
                newAms->alert = newAlePtr;
                newAms->snoozeDuration = it->snooze;
                newAms->notifEndOfAlert = false;
                newAms->message = it->message;
                
                Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> newAmsPtr = m_session.add<Echoes::Dbo::AlertMediaSpecialization>(newAms);
                newAmsPtr.flush();
                
                for (vector<AtsStruct>::iterator itATS = it->atsStructs.begin() ; itATS != it->atsStructs.end() ; ++itATS)
                {
                    Echoes::Dbo::AlertTimeSlot *newAts = new Echoes::Dbo::AlertTimeSlot();
                    
                    newAts->start = itATS->start;
                    newAts->duration = itATS->duration;
                    newAts->days = itATS->days;
                    newAts->months = itATS->months;
                    newAts->mediaSpecialization = newAmsPtr;
                    
                    Wt::Dbo::ptr<Echoes::Dbo::AlertTimeSlot> newAtsPtr = m_session.add<Echoes::Dbo::AlertTimeSlot>(newAts);
                    newAtsPtr.flush();
                }
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

//void AlertResource::replaceVariablesInMessage(vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrVector, Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr, string &message)
//{
//    const Wt::WDateTime now = Wt::WDateTime::currentDateTime();
//    
//    Wt::Dbo::ptr<Echoes::Dbo::AlertSequence> asePtr = alePtr->alertSequence;
//    int cpt = 0;
//    while (asePtr)
//    {
//        for (vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue> >::const_iterator it = ivaPtrVector.begin(); it != ivaPtrVector.end(); ++it)
//        {
//            if (it->get()->informationData == asePtr->alertValue->informationData)
//            {
//                boost::replace_all(message, "%value" + boost::lexical_cast<string> (cpt) + "%", it->get()->value.toUTF8());
//                boost::replace_all(message, "%threshold" + boost::lexical_cast<string> (cpt) + "%", asePtr->alertValue->value.toUTF8());
//                boost::replace_all(message, "%detection-time" + boost::lexical_cast<string> (cpt) + "%", it->get()->creationDate.toString().toUTF8());
//                boost::replace_all(message, "%alerting-time" + boost::lexical_cast<string> (cpt) + "%", now.toString().toUTF8());
//                boost::replace_all(message, "%unit" + boost::lexical_cast<string> (cpt) + "%", it->get()->informationData->informationUnit->name.toUTF8());
//            }
//        }
//        asePtr = asePtr->alertSequence;
//        cpt++;
//    }
//}

//EReturnCode AlertResource::sendMAIL
//(
// vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrVector,
// Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr,
// Wt::Dbo::ptr<Echoes::Dbo::Message> atrPtr,
// Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr,
// bool overSMSQuota
// )
//{
//    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//
//    Wt::WString mailRecipient;
//    const Wt::WString mailRecipientName = amsPtr->media->user->firstName + " " + amsPtr->media->user->lastName;
//    string mailBody = "";
//    const Wt::WDateTime now = Wt::WDateTime::currentDateTime(); //for setting the send date of the alert
//    Wt::Mail::Message mailMessage;
//    Wt::Mail::Client mailClient;
//
//    // Normal case
//    if (overSMSQuota == 0)
//    {
//        mailRecipient = amsPtr->media->value;
//    }
//    else if (overSMSQuota == 1)
//    {
//        mailRecipient = amsPtr->media->user->eMail;
//
//        mailBody += "MAIL sent instead of SMS (quota = 0) <br />";
//    }
//
//    mailBody += amsPtr->message.toUTF8();
//
//    replaceVariablesInMessage(ivaPtrVector, alePtr, mailBody);
//    
//    Wt::log("debug") << " [Alert Resource] " << mailBody;
//
//    mailMessage.setFrom(Wt::Mail::Mailbox(conf.getAlertMailSenderAddress(), conf.getAlertMailSenderName()));
//    mailMessage.addRecipient(Wt::Mail::To, Wt::Mail::Mailbox(mailRecipient.toUTF8(), mailRecipientName));
//    mailMessage.setSubject("[Echoes Alert] " + alePtr->name);
//    mailMessage.addHtmlBody(mailBody);
//    mailClient.connect(conf.getSMTPHost(), conf.getSMTPPort());
//    mailClient.send(mailMessage);
//
//    Wt::log("debug") << " [Class:AlertSender] " << "insert date of last send in db : " << now.toString();
//    amsPtr.modify()->lastSend = now;
//
//    atrPtr.modify()->sendDate = now;
//    atrPtr.modify()->content = mailBody;
//
//    res = EReturnCode::OK;
//
//    return res;
//}
//
//EReturnCode AlertResource::sendSMS
//(
// vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrVector,
// Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr,
// Wt::Dbo::ptr<Echoes::Dbo::Message> atrPtr,
// Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr
// )
//{
//    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//
//    const Wt::WDateTime now = Wt::WDateTime::currentDateTime();
//    string sms = amsPtr->message.toUTF8();
//
//    replaceVariablesInMessage(ivaPtrVector, alePtr, sms);
//    
//    Wt::log("debug") << " [Alert Resource] New SMS for " << amsPtr->media->value << " : " << sms;
//
//    ItookiSMSSender itookiSMSSender(m_session, this);
//
//    if (!itookiSMSSender.send(amsPtr->media->value.toUTF8(), sms, atrPtr))
//    {
//        amsPtr.modify()->lastSend = now;
//        res = EReturnCode::OK;
//    }
//
//    return res;
//}
//
//EReturnCode AlertResource::sendMobileApp
//(
// vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue >> ivaPtrVector,
// Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr,
// Wt::Dbo::ptr<Echoes::Dbo::Message> atrPtr,
// Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr
// )
//{
//    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//
//    const Wt::WDateTime now = Wt::WDateTime::currentDateTime(); //for setting the send date of the alert
//    string mobileApp = amsPtr->message.toUTF8();
//
//    replaceVariablesInMessage(ivaPtrVector, alePtr, mobileApp);
//    
//    Wt::log("debug") << " [Alert Resource] New Alert for mobileApp : " << amsPtr->media->value << " : " << mobileApp;
//
//    atrPtr.modify()->content = mobileApp;
//    atrPtr.modify()->sendDate = now;
//
//    amsPtr.modify()->lastSend = now;
//    res = EReturnCode::OK;
//
//    return res;
//}

//EReturnCode AlertResource::postAlertTracking(map<string, long long> parameters, const vector<string> &pathElements, const string &sRequest, const long long &orgId, string &responseMsg)
//{
//    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    vector<long long> ivaIds;
//    int mediaSpecializationId;
//
//    mediaSpecializationId = boost::lexical_cast<int>(parameters["alert_media_specialization_id"]);
//    
//    if (!sRequest.empty())
//    {
//        try
//        {
//            Wt::Json::Object result;
//            Wt::Json::parse(sRequest, result);
//
//            Wt::Json::Array array = result.get("information_value_ids");
//            
//            
//            for (Wt::Json::Array::const_iterator it = array.begin(); it != array.end(); ++it)
//            {
//                ivaIds.push_back(it->toNumber());
//            }
//        }
//        catch (Wt::Json::ParseError const& e)
//        {
//            res = EReturnCode::BAD_REQUEST;
//            responseMsg = httpCodeToJSON(res, e);
//        }
//        catch (Wt::Json::TypeException const& e)
//        {
//            res = EReturnCode::BAD_REQUEST;
//            responseMsg = httpCodeToJSON(res, e);
//        }
//    }
//    else
//    {
//        res = EReturnCode::BAD_REQUEST;
//        const string err = "[Alert Resource] sRequest is not empty";
//        responseMsg = httpCodeToJSON(res, err);
//    }
//
//    if (responseMsg.empty())
//    {
//        try
//        {
//            Wt::Dbo::Transaction transaction(m_session, true);
//
//            Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr = selectAlert(pathElements[1], orgId, m_session);
//            if (!alePtr)
//            {
//                res = EReturnCode::NOT_FOUND;
//                responseMsg = httpCodeToJSON(res, alePtr);
//                return res;
//            }
//            
//            Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr;
//            amsPtr = alePtr->alertMediaSpecializations.find()
//                            .where(QUOTE(TRIGRAM_ALERT_MEDIA_SPECIALIZATION ID) " = ?")
//                            .bind(mediaSpecializationId);
//            
//            if (!amsPtr)
//            {
//                res = EReturnCode::NOT_FOUND;
//                responseMsg = httpCodeToJSON(res, amsPtr);
//                return res;
//            }
//
//            vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrVector;
//            for (vector<long long>::const_iterator it = ivaIds.begin(); it < ivaIds.end(); ++it)
//            {
//                Wt::Dbo::ptr<Echoes::Dbo::InformationValue> ivaPtr = m_session.find<Echoes::Dbo::InformationValue>()
//                        .where(QUOTE(TRIGRAM_INFORMATION_VALUE ID) " = ?").bind(*it)
//                        .where(QUOTE(TRIGRAM_INFORMATION_VALUE SEP "DELETE") " IS NULL");
//                if (!ivaPtr)
//                {
//                    res = EReturnCode::NOT_FOUND;
//                    responseMsg = httpCodeToJSON(res, ivaPtr);
//                    return res;
//                }
//                ivaPtrVector.push_back(ivaPtr);
//            }
//                    
//            Wt::Dbo::ptr<Echoes::Dbo::AlertSequence> asePtr = alePtr->alertSequence;
//            size_t cpt = 0;
//            while(asePtr)
//            {
//                if (ivaPtrVector.size() <= cpt || ivaPtrVector[cpt]->informationData != asePtr->alertValue->informationData ||
//                        (!asePtr->alertSequence && ivaPtrVector.size() != cpt+1))
//                {
//                    res = EReturnCode::BAD_REQUEST;
//                    const string err = "[Alert Resource] bad size or content of ivaPtrVector";
//                    responseMsg = httpCodeToJSON(res, err);
//                    return res;
//                }                
//                asePtr = asePtr->alertSequence;
//                cpt++;
//            }
//
//            Wt::WDateTime now = Wt::WDateTime::currentDateTime();
//
//            alePtr.modify()->lastAttempt = now;
//
//            // it is the first time we send the alert there is no last send date filled
//            // or date.now() - last_send = nb_secs then, if nb_secs >= snooze of the media, we send the alert
//            if (!amsPtr->lastSend.isValid() || amsPtr->lastSend.secsTo(now) >= amsPtr->snoozeDuration)
//            {
//                const long long mtyID = amsPtr->media->mediaType.id();
//                Echoes::Dbo::Message *newAtr = new Echoes::Dbo::Message();
//
//                newAtr->alert = alePtr;
//                newAtr->media = amsPtr->media;
//                // WARNING : SendDate must be set by the API when the alert was sent, not before
//                newAtr->sendDate = *(new Wt::WDateTime());
//
//                Wt::Dbo::ptr<Echoes::Dbo::Message> newAtrPtr = m_session.add<Echoes::Dbo::Message>(newAtr);
//                newAtrPtr.flush();
//
//                Wt::log("debug") << " [Alert Ressource] " << "Alert tracking number creation : " << newAtrPtr.id();
//
//                Wt::log("debug") << " [Alert Ressource] " << "snooze = " << amsPtr->snoozeDuration;
//
//                switch (mtyID)
//                {
//                case Echoes::Dbo::EMediaType::SMS:
//                {
//                    Wt::log("debug") << " [Alert Ressource] " << "Media value SMS choosed for the alert : " << alePtr->name;
//
//                    // Verifying the quota of sms
//                    Wt::Dbo::ptr<Echoes::Dbo::Option> optPtr = m_session.find<Echoes::Dbo::Option>()
//                            .where(QUOTE(TRIGRAM_OPTION SEP TRIGRAM_OPTION_TYPE SEP TRIGRAM_OPTION_TYPE ID) " = ?").bind(Echoes::Dbo::EOptionType::QUOTA_SMS)
//                            .where(QUOTE(TRIGRAM_OPTION SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(orgId)
//                            .limit(1);
//
//                    try
//                    {
//                        const int smsQuota = boost::lexical_cast<int>(optPtr->value);
//                        if (smsQuota == 0)
//                        {
//                            Wt::log("debug") << " [Alert Ressource] " << "SMS quota 0 for alert : " << alePtr->name;
//                            Wt::log("debug") << " [Alert Ressource] " << "Sending e-mail instead.";
//
//                            sendMAIL(ivaPtrVector, alePtr, newAtrPtr, amsPtr, true);
//                        }
//                        else
//                        {
//                            Wt::log("debug") << " [Alert Ressource] " << "We send a SMS, quota : " << smsQuota;
//                            optPtr.modify()->value = boost::lexical_cast<string>(smsQuota - 1);
//                            optPtr.flush();
//                            sendSMS(ivaPtrVector, alePtr, newAtrPtr, amsPtr);
//                        }
//                    }
//                    catch (boost::bad_lexical_cast const& e)
//                    {
//                        res = EReturnCode::SERVICE_UNAVAILABLE;
//                        responseMsg = httpCodeToJSON(res, e);
//                    }
//                    break;
//                }
//                case Echoes::Dbo::EMediaType::MAIL:
//                    Wt::log("debug") << " [Alert Ressource] " << "Media value MAIL choosed for the alert : " << alePtr->name;
//                    sendMAIL(ivaPtrVector, alePtr, newAtrPtr, amsPtr);
//                    break;
//                case Echoes::Dbo::EMediaType::MOBILE_APP:
//                    Wt::log("debug") << " [Alert Ressource] " << "Media value MOBILEAPP choosed for the alert : " << alePtr->name;
//                    sendMobileApp(ivaPtrVector, alePtr, newAtrPtr, amsPtr);
//                    break;
//                default:
//                    Wt::log("error") << "[Alert Resource] Unknown ID Media: " << mtyID;
//                    break;
//                }
//            }
//            else
//            {
//                Wt::log("debug") << "[Alert Resource] "
//                        << "Last time we send the alert : " << alePtr->name
//                        << "was : " << amsPtr->lastSend.toString()
//                        << "the snooze for the media " << amsPtr->media->mediaType->name
//                        << " is : " << amsPtr->snoozeDuration << "secs,  it's not the time to send the alert";
//            }
//
//            res = EReturnCode::CREATED;
//
//            transaction.commit();
//        }
//        catch (Wt::Dbo::Exception const& e)
//        {
//            res = EReturnCode::SERVICE_UNAVAILABLE;
//            responseMsg = httpCodeToJSON(res, e);
//        }
//    }
//
//    return res;
//}

EReturnCode AlertResource::startAlert(const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr = selectAlert(pathElements[1], orgId, m_session);
        
        

        Wt::Dbo::collection < Wt::Dbo::ptr < Echoes::Dbo::AlertTrackingEvent >> atrPtrCol = m_session.find<Echoes::Dbo::AlertTrackingEvent>()
                .where(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP TRIGRAM_ALERT SEP TRIGRAM_ALERT ID)" = ? ").bind(alePtr->id)
                .where(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP "DELETE") " IS NULL")
                .orderBy(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP "DATE") " ASC")
                .limit(20);

        if((atrPtrCol.size() == 0) 
                || (atrPtrCol.begin()->get()->statut->id == Echoes::Dbo::EAlertStatus::BACKTONORMAL))
        {
            Echoes::Dbo::AlertTrackingEvent *newStateAle = new Echoes::Dbo::AlertTrackingEvent();

            newStateAle->date = Wt::WDateTime::currentDateTime();
                        newStateAle->alert = alePtr;
            Wt::Dbo::ptr<Echoes::Dbo::AlertStatus> aleStPtr = m_session.find<Echoes::Dbo::AlertStatus>()
                        .where(QUOTE(TRIGRAM_ALERT_STATUS ID) " = ?").bind(Echoes::Dbo::EAlertStatus::PENDING)
                        .where(QUOTE(TRIGRAM_ALERT_STATUS SEP "DELETE") " IS NULL");
            newStateAle->statut = aleStPtr;

            Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent> newAleTrEv = m_session.add<Echoes::Dbo::AlertTrackingEvent>(newStateAle);

            res = EReturnCode::CREATED;

            transaction.commit();
        }
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode AlertResource::processPostRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    string nextElement = "";
    unsigned short indexPathElement = 1;
    vector<string> pathElements;
    map<string, long long> parameters;
    parameters["alert_media_specialization_id"] = 0;
    
    
    const string sRequest = processRequestParameters(request, pathElements, parameters);

    nextElement = getNextElementFromPath(indexPathElement, pathElements);
    if (nextElement.empty())
    {
        res = postAlert(sRequest,orgId, responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.compare("start") == 0)
            {
                res = startAlert(pathElements, orgId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Media Resource] bad nextElement";
                responseMsg = httpCodeToJSON(res, err);
            }
        }
        catch (boost::bad_lexical_cast const& e)
        {
            res = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(res, e);
        }
    }

    return res;
}

EReturnCode AlertResource::deleteAlert(const vector<string> &pathElements, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr = selectAlert(pathElements[1], orgId, m_session);

        if (alePtr)
        {
            alePtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
            res = EReturnCode::NO_CONTENT;
        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = httpCodeToJSON(res, alePtr);
        }

        transaction.commit();
    }
    catch (boost::bad_lexical_cast const& e)
    {
        res = EReturnCode::BAD_REQUEST;
        responseMsg = httpCodeToJSON(res, e);
    }

    return res;
}

EReturnCode AlertResource::processDeleteRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    string nextElement = "";
    unsigned short indexPathElement = 1;
    vector<string> pathElements;
    map<string, long long> parameters;

    const string sRequest = processRequestParameters(request, pathElements, parameters);

    nextElement = getNextElementFromPath(indexPathElement, pathElements);
    if (nextElement.empty())
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Alert Resource] bad nextElement";
        responseMsg = httpCodeToJSON(res, err);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);

            if (nextElement.empty())
            {
                res = deleteAlert(pathElements, orgId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Alert Resource] bad nextElement";
                responseMsg = httpCodeToJSON(res, err);
            }
        }
        catch (boost::bad_lexical_cast const& e)
        {
            res = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(res, e);
        }
    }

    return res;
}

