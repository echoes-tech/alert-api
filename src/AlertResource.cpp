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
    resourceClassName = "AlertResource";
    
    functionMap["getAlertStatus"] = boost::bind(&AlertResource::getAlertStatus, this, _1, _2, _3, _4, _5);
    functionMap["getAlertMessages"] = boost::bind(&AlertResource::getAlertMessages, this, _1, _2, _3, _4, _5);
    functionMap["getAlertEvents"] = boost::bind(&AlertResource::getAlertEvents, this, _1, _2, _3, _4, _5);
    functionMap["getAlertEvent"] = boost::bind(&AlertResource::getAlertEvent, this, _1, _2, _3, _4, _5);
    functionMap["startAlert"] = boost::bind(&AlertResource::startAlert, this, _1, _2, _3, _4, _5);
    functionMap["forwardAlert"] = boost::bind(&AlertResource::forwardAlert, this, _1, _2, _3, _4, _5);
    functionMap["assignAlert"] = boost::bind(&AlertResource::assignAlert, this, _1, _2, _3, _4, _5);
    functionMap["resolveAlert"] = boost::bind(&AlertResource::resolveAlert, this, _1, _2, _3, _4, _5);
    functionMap["getAlertsList"] = boost::bind(&AlertResource::getAlertsList, this, _1, _2, _3, _4, _5);
    functionMap["getAlert"] = boost::bind(&AlertResource::getAlert, this, _1, _2, _3, _4, _5);
    functionMap["postAlert"] = boost::bind(&AlertResource::postAlert, this, _1, _2, _3, _4, _5);
    functionMap["deleteAlert"] = boost::bind(&AlertResource::deleteAlert, this, _1, _2, _3, _4, _5);
    
    calls = FillCallsVector();
}

AlertResource::~AlertResource()
{
}

Wt::Dbo::ptr<Echoes::Dbo::Alert> AlertResource::selectAlert(const long long &aleId, const long long &grpId, Echoes::Dbo::Session &session)
{
    return selectAlert(boost::lexical_cast<string>(aleId), boost::lexical_cast<string>(grpId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::Alert> AlertResource::selectAlert(const string &aleId, const long long &grpId, Echoes::Dbo::Session &session)
{
    return selectAlert(aleId, boost::lexical_cast<string>(grpId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::Alert> AlertResource::selectAlert(const string &aleId, const string &grpId, Echoes::Dbo::Session &session)
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
"                             " QUOTE(TRIGRAM_USER SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = " + grpId +
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

EReturnCode AlertResource::getAlertsList(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
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
"                             " QUOTE(TRIGRAM_USER SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = " + boost::lexical_cast<string>(grpId) +
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

EReturnCode AlertResource::getAlert(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr = selectAlert(pathElements[1], grpId, m_session);

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

EReturnCode AlertResource::getAlertEvent(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
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

EReturnCode AlertResource::getAlertEvents(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
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

EReturnCode AlertResource::getAlertStatus(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        //ToDo(FPO): Check rights
        Wt::Dbo::collection < Wt::Dbo::ptr < Echoes::Dbo::AlertTrackingEvent >> atrPtrCol = m_session.find<Echoes::Dbo::AlertTrackingEvent>()
                .where(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP TRIGRAM_ALERT SEP TRIGRAM_ALERT ID)" = ? ").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP "DELETE") " IS NULL")
                .orderBy(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP "DATE") " DESC");
        
        if(atrPtrCol.size() > 0)
        {
            Wt::Dbo::collection < Wt::Dbo::ptr < Echoes::Dbo::AlertTrackingEvent >>::iterator ptrTrackEvent = atrPtrCol.begin();
            res = EReturnCode::OK;
        
            responseMsg = "{";
            responseMsg += "\n\"id\" : " + boost::lexical_cast<std::string>(ptrTrackEvent->get()->alert.id()) + ",";
            responseMsg += "\n\"state\" : " + boost::lexical_cast<std::string>(ptrTrackEvent->get()->statut.id()) + ",";
            responseMsg += "\n\"name\" : \"" + boost::lexical_cast<std::string>(ptrTrackEvent->get()->alert->name) + "\"";
            if(ptrTrackEvent->get()->user)
            {
                responseMsg += ",\n\"user\" : \"" + boost::lexical_cast<std::string>(ptrTrackEvent->get()->user->firstName) + " "
                                + boost::lexical_cast<std::string>(ptrTrackEvent->get()->user->lastName) + "\"";
            }
            responseMsg += "\n}";
        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = httpCodeToJSON(res, "value not found");
        }
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

EReturnCode AlertResource::getAlertMessages(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
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

EReturnCode AlertResource::processGetRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
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
        res = getAlertsList(grpId, responseMsg, pathElements, sRequest, parameters);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);
            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.empty())
            {
                res = getAlert(grpId, responseMsg, pathElements, sRequest, parameters);
            }
            else if(nextElement.compare("status") == 0)
            {
                res = getAlertStatus(grpId, responseMsg, pathElements, sRequest, parameters);
            }
            else if(nextElement.compare("messages") == 0)
            {
                res = getAlertMessages(grpId, responseMsg, pathElements, sRequest, parameters);
            }
            else if(nextElement.compare("events") == 0)
            {
                
                if (nextElement.empty())
                {
                    res = getAlertEvents(grpId, responseMsg, pathElements, sRequest, parameters);
                }
                else
                {
                    boost::lexical_cast<unsigned long long>(nextElement);
                    nextElement = getNextElementFromPath(indexPathElement, pathElements);
                    if (nextElement.empty())
                    {
                        res = getAlertEvent(grpId, responseMsg, pathElements, sRequest, parameters);
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

EReturnCode AlertResource::postAlert(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
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
"                                                 " QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = " + boost::lexical_cast<string>(grpId) +
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
                Wt::Dbo::ptr<Echoes::Dbo::Media> medPtr = MediaResource::selectMedia(it->medId, grpId, m_session);
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

EReturnCode AlertResource::forwardAlert(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    
    if (!sRequest.empty())
    {
        try
        {
            const Wt::WDateTime now = Wt::WDateTime::currentDateTime();
            
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);
            
            Wt::Dbo::Transaction transaction(m_session, true);
    
            long long id = result.get("user");
            long long idForward = result.get("userForward");
            
            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent>> atePtr = m_session.find<Echoes::Dbo::AlertTrackingEvent>()
                    .where(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP TRIGRAM_ALERT SEP TRIGRAM_ALERT ID) " = ?").bind(pathElements[1])
                    .where(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP "DELETE") " IS NULL")
                    .orderBy(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP "DATE") " DESC");
            
            if(atePtr.size() > 0)
            {
                Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent>>::iterator itAtePtr = atePtr.begin();
                if(itAtePtr->get()->statut.id() == Echoes::Dbo::EAlertStatus::PENDING
                        || (itAtePtr->get()->statut.id() == Echoes::Dbo::EAlertStatus::SUPPORTED
                            && itAtePtr->get()->user.id() == id))
                {
                    Echoes::Dbo::AlertTrackingEvent *newStateAle = new Echoes::Dbo::AlertTrackingEvent();

                    newStateAle->date = now;
                    newStateAle->alert = itAtePtr->get()->alert;
                    
                    newStateAle->statut = m_session.find<Echoes::Dbo::AlertStatus>()
                        .where(QUOTE(TRIGRAM_ALERT_STATUS ID) " = ?").bind(Echoes::Dbo::EAlertStatus::FORWARDING)
                        .where(QUOTE(TRIGRAM_ALERT_STATUS SEP "DELETE") " IS NULL");
                    
                    newStateAle->user = m_session.find<Echoes::Dbo::User>()
                        .where(QUOTE(TRIGRAM_USER ID) " = ?").bind(idForward)
                        .where(QUOTE(TRIGRAM_USER SEP "DELETE") " IS NULL");

                    Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent> newAleTrEv = m_session.add<Echoes::Dbo::AlertTrackingEvent>(newStateAle);
                    res = EReturnCode::CREATED;
                }
                else
                {
                    res = EReturnCode::BAD_REQUEST;
                    const string err = "[Alert Resource] Alert is not pending or user is not in charge";
                    responseMsg = httpCodeToJSON(res, err);
                }
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Alert Resource] Alert is not pending";
                responseMsg = httpCodeToJSON(res, err);
            }
            transaction.commit();
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
    
    return res;
}

EReturnCode AlertResource::assignAlert(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    
    if (!sRequest.empty())
    {
        try
        {
            const Wt::WDateTime now = Wt::WDateTime::currentDateTime();
            
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);
            
            Wt::Dbo::Transaction transaction(m_session, true);
    
            long long id = result.get("user");
            
            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent>> atePtr = m_session.find<Echoes::Dbo::AlertTrackingEvent>()
                    .where(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP TRIGRAM_ALERT SEP TRIGRAM_ALERT ID) " = ?").bind(pathElements[1])
                    .where(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP "DELETE") " IS NULL")
                    .orderBy(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP "DATE") " DESC");
            
            if(atePtr.size() > 0)
            {
                Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent>>::iterator itAtePtr = atePtr.begin();
                if(itAtePtr->get()->statut.id() == Echoes::Dbo::EAlertStatus::PENDING
                        || (itAtePtr->get()->statut.id() == Echoes::Dbo::EAlertStatus::FORWARDING
                            && itAtePtr->get()->user.id() == id))
                {
                    Echoes::Dbo::AlertTrackingEvent *newStateAle = new Echoes::Dbo::AlertTrackingEvent();

                    newStateAle->date = now;
                    newStateAle->alert = itAtePtr->get()->alert;
                    
                    newStateAle->statut = m_session.find<Echoes::Dbo::AlertStatus>()
                        .where(QUOTE(TRIGRAM_ALERT_STATUS ID) " = ?").bind(Echoes::Dbo::EAlertStatus::SUPPORTED)
                        .where(QUOTE(TRIGRAM_ALERT_STATUS SEP "DELETE") " IS NULL");
                    
                    newStateAle->user = m_session.find<Echoes::Dbo::User>()
                        .where(QUOTE(TRIGRAM_USER ID) " = ?").bind(id)
                        .where(QUOTE(TRIGRAM_USER SEP "DELETE") " IS NULL");

                    Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent> newAleTrEv = m_session.add<Echoes::Dbo::AlertTrackingEvent>(newStateAle);
                    res = EReturnCode::CREATED;
                }
                else
                {
                    res = EReturnCode::BAD_REQUEST;
                    const string err = "[Alert Resource] Alert is not pending or user is not forwarded";
                    responseMsg = httpCodeToJSON(res, err);
                }
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Alert Resource] Alert is not pending";
                responseMsg = httpCodeToJSON(res, err);
            }
            transaction.commit();
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
    
    return res;
}

EReturnCode AlertResource::resolveAlert(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    
    if (!sRequest.empty())
    {
        try
        {
            const Wt::WDateTime now = Wt::WDateTime::currentDateTime();
            
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);
            
            Wt::Dbo::Transaction transaction(m_session, true);
    
            long long id = result.get("user");
            
            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent>> atePtr = m_session.find<Echoes::Dbo::AlertTrackingEvent>()
                    .where(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP TRIGRAM_ALERT SEP TRIGRAM_ALERT ID) " = ?").bind(pathElements[1])
                    .where(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP "DELETE") " IS NULL")
                    .orderBy(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP "DATE") " DESC");
            
            if(atePtr.size() > 0)
            {
                Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent>>::iterator itAtePtr = atePtr.begin();
                if(itAtePtr->get()->statut.id() == Echoes::Dbo::EAlertStatus::SUPPORTED
                        && itAtePtr->get()->user.id() == id)
                {
                    Echoes::Dbo::AlertTrackingEvent *newStateAle = new Echoes::Dbo::AlertTrackingEvent();

                    newStateAle->date = now;
                    newStateAle->alert = itAtePtr->get()->alert;
                    Wt::Dbo::ptr<Echoes::Dbo::AlertStatus> alsPtr = m_session.find<Echoes::Dbo::AlertStatus>()
                        .where(QUOTE(TRIGRAM_ALERT_STATUS ID) " = ?").bind(Echoes::Dbo::EAlertStatus::BACKTONORMAL)
                        .where(QUOTE(TRIGRAM_ALERT_STATUS SEP "DELETE") " IS NULL");
                    newStateAle->statut = alsPtr;

                    Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent> newAleTrEv = m_session.add<Echoes::Dbo::AlertTrackingEvent>(newStateAle);
                    res = EReturnCode::CREATED;
                }
                else
                {
                    res = EReturnCode::BAD_REQUEST;
                    const string err = "[Alert Resource] Alert is not supported";
                    responseMsg = httpCodeToJSON(res, err);
                }
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Alert Resource] Alert is not supported";
                responseMsg = httpCodeToJSON(res, err);
            }
            transaction.commit();
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
    
    return res;
}

EReturnCode AlertResource::startAlert(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr = selectAlert(pathElements[1], grpId, m_session);
        
        

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

EReturnCode AlertResource::processPostRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
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
        res = postAlert(grpId, responseMsg, std::vector<std::basic_string<char>>(), sRequest);
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Alert Resource] bad nextElement";
        responseMsg = httpCodeToJSON(res, err);
    }

    return res;
}

EReturnCode AlertResource::processPutRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
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
        //res = putAlert(sRequest,orgId, responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.compare("start") == 0)
            {
                res = startAlert(grpId, responseMsg, pathElements, sRequest, parameters);
            }
            else if (nextElement.compare("forward") == 0)
            {
                res = forwardAlert(grpId, responseMsg, pathElements, sRequest, parameters);
            }
            else if (nextElement.compare("assign") == 0)
            {
                res = assignAlert(grpId, responseMsg, pathElements, sRequest, parameters);
            }
            else if (nextElement.compare("resolve") == 0)
            {
                res = resolveAlert(grpId, responseMsg, pathElements, sRequest, parameters);
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


EReturnCode AlertResource::deleteAlert(const long long &grpId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr = selectAlert(pathElements[1], grpId, m_session);

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

EReturnCode AlertResource::processDeleteRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
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
                res = deleteAlert(grpId, responseMsg, pathElements);
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

