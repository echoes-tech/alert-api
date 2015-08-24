/* 
 * API MessageResource
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
#include "MessageResource.h"

using namespace std;

MessageResource::MessageResource(Echoes::Dbo::Session& session) : PublicApiResource::PublicApiResource(session)
{
}

MessageResource::~MessageResource()
{
}

Wt::Dbo::ptr<Echoes::Dbo::Message> MessageResource::selectMessage(const long long &msgId, const long long &orgId, Echoes::Dbo::Session &session)
{
    return selectMessage(boost::lexical_cast<string>(msgId), boost::lexical_cast<string>(orgId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::Message> MessageResource::selectMessage(const string &msgId, const long long &orgId, Echoes::Dbo::Session &session)
{
    return selectMessage(msgId, boost::lexical_cast<string>(orgId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::Message> MessageResource::selectMessage(const string &msgId, const string &orgId, Echoes::Dbo::Session &session)
{
    const string queryStr =
" SELECT msg"
"   FROM " QUOTE("T_MESSAGE_MSG") " msg"
"   WHERE"
"     " QUOTE(TRIGRAM_MESSAGE ID) " = " + msgId +
"   LIMIT 1";

    Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Message>> queryRes = session.query<Wt::Dbo::ptr<Echoes::Dbo::Message>> (queryStr);

    return queryRes.resultValue();
}

Wt::Dbo::ptr<Echoes::Dbo::Alert> MessageResource::selectAlert(const long long &aleId, const long long &orgId, Echoes::Dbo::Session &session)
{
    return selectAlert(boost::lexical_cast<string>(aleId), boost::lexical_cast<string>(orgId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::Alert> MessageResource::selectAlert(const string &aleId, const long long &orgId, Echoes::Dbo::Session &session)
{
    return selectAlert(aleId, boost::lexical_cast<string>(orgId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::Alert> MessageResource::selectAlert(const string &aleId, const string &orgId, Echoes::Dbo::Session &session)
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

EReturnCode MessageResource::getMessages(map<string, long long> &parameters, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    if (parameters["media_id"] > 0)
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session, true);

            //ToDo(FPO): Check rights
            Wt::Dbo::collection < Wt::Dbo::ptr < Echoes::Dbo::Message >> atrPtrCol = m_session.find<Echoes::Dbo::Message>()
                    .where(QUOTE(TRIGRAM_MESSAGE SEP TRIGRAM_MEDIA SEP TRIGRAM_MEDIA ID)" = ? ").bind(parameters["media_id"])
                    .orderBy(QUOTE(TRIGRAM_MESSAGE SEP "SEND_DATE") " DESC");

            res = serialize(atrPtrCol, responseMsg);

            transaction.commit();
        }
        catch (Wt::Dbo::Exception const& e)
        {
            res = EReturnCode::SERVICE_UNAVAILABLE;
            responseMsg = httpCodeToJSON(res, e);
        }
    }
    else if(parameters["alert_id"] > 0)
    {
            try
            {
                Wt::Dbo::Transaction transaction(m_session, true);
                string queryStr =
                        " SELECT msg"
                        "   FROM " QUOTE("T_MESSAGE" SEP TRIGRAM_MESSAGE) " msg"
                        "   WHERE"
                        "     " QUOTE(TRIGRAM_MESSAGE SEP TRIGRAM_ALERT SEP TRIGRAM_ALERT ID) " = " + boost::lexical_cast<string>(parameters["alert_id"]) +
                        "     AND " QUOTE(TRIGRAM_MESSAGE SEP "DELETE") " IS NULL";

                Wt::Dbo::Query<Wt::Dbo::ptr < Echoes::Dbo::Message>> queryRes = m_session.query<Wt::Dbo::ptr < Echoes::Dbo::Message >> (queryStr);

                Wt::Dbo::collection<Wt::Dbo::ptr < Echoes::Dbo::Message>> msgCol = queryRes.resultList();

                res = serialize(msgCol, responseMsg);

                transaction.commit();
            }
            catch (Wt::Dbo::Exception const& e)
            {
                res = EReturnCode::SERVICE_UNAVAILABLE;
                responseMsg = httpCodeToJSON(res, e);
            }
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Message Resource] media or alert is empty";
        responseMsg = httpCodeToJSON(res, err);
    }
    return res;
}

EReturnCode MessageResource::getMessage(const std::vector<std::string> &pathElements, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::Message> msgPtr = selectMessage(pathElements[1], orgId, m_session);

        res = serialize(msgPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode MessageResource::postAlertMessage(map<string, long long> parameters, const vector<string> &pathElements, const string &sRequest, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    vector<long long> ivaIds;
    int mediaSpecializationId;
    long long alertId;

    if ((parameters["alert_media_specialization_id"]) > 0)
    {
    
        mediaSpecializationId = boost::lexical_cast<int>(parameters["alert_media_specialization_id"]);

        if (!sRequest.empty())
        {
            try
            {
                Wt::Json::Object result;
                Wt::Json::parse(sRequest, result);

                Wt::Json::Array array = result.get("information_value_ids");
                alertId = result.get("alert_id");

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
            const string err = "[Alert Resource] sRequest is not empty";
            responseMsg = httpCodeToJSON(res, err);
        }

        if (responseMsg.empty())
        {
            try
            {
                Wt::Dbo::Transaction transaction(m_session, true);

                Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr = selectAlert(alertId, orgId, m_session);
                if (!alePtr)
                {
                    res = EReturnCode::NOT_FOUND;
                    responseMsg = httpCodeToJSON(res, alePtr);
                    return res;
                }
                else
                {
                    Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent>> AlertEventList = m_session.find<Echoes::Dbo::AlertTrackingEvent>()
                                    .where(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP TRIGRAM_ALERT SEP TRIGRAM_ALERT ID) " = ?").bind(alePtr.id())
                                    .where(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP "DELETE") " IS NULL")
                                    .orderBy(QUOTE(TRIGRAM_ALERT_TRACKING_EVENT SEP "DATE") " DESC");
                    if(AlertEventList.size() <= 0 ||
                            AlertEventList.begin()->get()->statut.id() == Echoes::Dbo::EAlertStatus::BACKTONORMAL)
                    {
                        Echoes::Dbo::AlertTrackingEvent *newStateAle = new Echoes::Dbo::AlertTrackingEvent();

                        newStateAle->date = Wt::WDateTime::currentDateTime();
                        newStateAle->alert = alePtr;
                        Wt::Dbo::ptr<Echoes::Dbo::AlertStatus> alsPtr = m_session.find<Echoes::Dbo::AlertStatus>()
                            .where(QUOTE(TRIGRAM_ALERT_STATUS ID) " = ?").bind(Echoes::Dbo::EAlertStatus::PENDING)
                            .where(QUOTE(TRIGRAM_ALERT_STATUS SEP "DELETE") " IS NULL");
                        newStateAle->statut = alsPtr;

                        Wt::Dbo::ptr<Echoes::Dbo::AlertTrackingEvent> newAleTrEv = m_session.add<Echoes::Dbo::AlertTrackingEvent>(newStateAle);

                    }

                }
                Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr;
                amsPtr = m_session.find<Echoes::Dbo::AlertMediaSpecialization>()
                                .where(QUOTE(TRIGRAM_ALERT_MEDIA_SPECIALIZATION ID) " = ?").bind(mediaSpecializationId)
                                .where(QUOTE(TRIGRAM_ALERT_MEDIA_SPECIALIZATION SEP "DELETE") " IS NULL");

                if (!amsPtr)
                {
                    res = EReturnCode::NOT_FOUND;
                    responseMsg = httpCodeToJSON(res, amsPtr);
                    return res;
                }

                vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrVector;
                for (vector<long long>::const_iterator it = ivaIds.begin(); it < ivaIds.end(); ++it)
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

                Wt::Dbo::ptr<Echoes::Dbo::AlertSequence> asePtr = alePtr->alertSequence;
                size_t cpt = 0;
                while(asePtr)
                {
                    if (ivaPtrVector.size() <= cpt || ivaPtrVector[cpt]->informationData != asePtr->alertValue->informationData ||
                            (!asePtr->alertSequence && ivaPtrVector.size() != cpt+1))
                    {
                        res = EReturnCode::BAD_REQUEST;
                        const string err = "[Alert Resource] bad size or content of ivaPtrVector";
                        responseMsg = httpCodeToJSON(res, err);
                        return res;
                    }                
                    asePtr = asePtr->alertSequence;
                    cpt++;
                }

                Wt::WDateTime now = Wt::WDateTime::currentDateTime();

                alePtr.modify()->lastAttempt = now;

                // it is the first time we send the alert there is no last send date filled
                // or date.now() - last_send = nb_secs then, if nb_secs >= snooze of the media, we send the alert
                if (!amsPtr->lastSend.isValid() || amsPtr->lastSend.secsTo(now) >= amsPtr->snoozeDuration)
                {
                    const long long mtyID = amsPtr->media->mediaType.id();
                    Echoes::Dbo::Message *newMsg = new Echoes::Dbo::Message();

                    newMsg->refAck = "";
                    string sms;
                    sms = amsPtr->message.toUTF8();
                    if (ivaPtrVector.size() > 0)
                    {
                        replaceVariablesInMessage(ivaPtrVector, alePtr, sms);
                    }
                    newMsg->content = sms;
                    newMsg->dest = amsPtr->media->value;
                    newMsg->receiverSrv = conf.getRouteurHost(); 
                    newMsg->receiverPort = boost::lexical_cast<std::string>(conf.getRouteurPort());
                    
                    char hostname[255];
                    gethostname(hostname, 255);
                    
                    newMsg->senderSrv = hostname;
                    newMsg->senderPort = boost::lexical_cast<std::string>(conf.getServerPort());

                    newMsg->user = amsPtr->media->user;
                    newMsg->alert = alePtr;
                    
                    Wt::Dbo::ptr<Echoes::Dbo::Message> newMsgPtr = m_session.add<Echoes::Dbo::Message>(newMsg);
                    newMsgPtr.flush();
                    
                    Echoes::Dbo::MessageTrackingEvent *newStateMsg = new Echoes::Dbo::MessageTrackingEvent();
                    
                    newStateMsg->date = now;
                    newStateMsg->message = newMsgPtr;
                    Wt::Dbo::ptr<Echoes::Dbo::MessageStatus> mstPtr = m_session.find<Echoes::Dbo::MessageStatus>()
                        .where(QUOTE(TRIGRAM_MESSAGE_STATUS ID) " = ?").bind(Echoes::Dbo::EMessageStatus::UNCREATED)
                        .where(QUOTE(TRIGRAM_MESSAGE_STATUS SEP "DELETE") " IS NULL");
                    newStateMsg->statut = mstPtr;
                    newStateMsg->text = "";
                    
                    Wt::Dbo::ptr<Echoes::Dbo::MessageTrackingEvent> newMsgTrEv = m_session.add<Echoes::Dbo::MessageTrackingEvent>(newStateMsg);

                    Wt::log("debug") << " [Alert Ressource] " << "Alert tracking number creation : " << newMsgPtr.id();

                    Wt::log("debug") << " [Alert Ressource] " << "snooze = " << amsPtr->snoozeDuration;

                    switch (mtyID)
                    {
                    case Echoes::Dbo::EMediaType::SMS:
                    {
                        Wt::log("debug") << " [Alert Ressource] " << "Media value SMS choosed for the alert : " << alePtr->name;

                        // Verifying the quota of sms
                        Wt::Dbo::ptr<Echoes::Dbo::Option> optPtr = m_session.find<Echoes::Dbo::Option>()
                                .where(QUOTE(TRIGRAM_OPTION SEP TRIGRAM_OPTION_TYPE SEP TRIGRAM_OPTION_TYPE ID) " = ?").bind(Echoes::Dbo::EOptionType::QUOTA_SMS)
                                .where(QUOTE(TRIGRAM_OPTION SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(orgId)
                                .limit(1);

                        try
                        {
                            const int smsQuota = boost::lexical_cast<int>(optPtr->value);
                            if (smsQuota == 0)
                            {
                                Wt::log("debug") << " [Alert Ressource] " << "SMS quota 0 for alert : " << alePtr->name;
                                Wt::log("debug") << " [Alert Ressource] " << "Sending e-mail instead.";

                                sendMAIL(newMsgPtr, true, ivaPtrVector, alePtr, amsPtr);
                            }
                            else
                            {
                                Wt::log("debug") << " [Alert Ressource] " << "We send a SMS, quota : " << smsQuota;
                                optPtr.modify()->value = boost::lexical_cast<string>(smsQuota - 1);
                                optPtr.flush();
                                sendSMS(newMsgPtr, ivaPtrVector, alePtr, amsPtr);
                            }
                        }
                        catch (boost::bad_lexical_cast const& e)
                        {
                            res = EReturnCode::SERVICE_UNAVAILABLE;
                            responseMsg = httpCodeToJSON(res, e);
                        }
                        break;
                    }
                    case Echoes::Dbo::EMediaType::MAIL:
                        Wt::log("debug") << " [Alert Ressource] " << "Media value MAIL choosed for the alert : " << alePtr->name;
                        sendMAIL(newMsgPtr, false, ivaPtrVector, alePtr, amsPtr);
                        break;
                    case Echoes::Dbo::EMediaType::MOBILE_APP:
                        Wt::log("debug") << " [Alert Ressource] " << "Media value MOBILEAPP choosed for the alert : " << alePtr->name;
                        sendMobileApp(newMsgPtr, ivaPtrVector, alePtr, amsPtr);
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
                            << "was : " << amsPtr->lastSend.toString()
                            << "the snooze for the media " << amsPtr->media->mediaType->name
                            << " is : " << amsPtr->snoozeDuration << "secs,  it's not the time to send the alert";
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
    }
    else
    {
        long long medId;
        Wt::WString message;

        if (!sRequest.empty())
        {
            try
            {
                Wt::Json::Object result;
                Wt::Json::parse(sRequest, result);
                medId = result.get("media_id");
                message = result.get("message");
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
            const string err = "[Message Resource] sRequest is not empty";
            responseMsg = httpCodeToJSON(res, err);
        }

        if (responseMsg.empty())
        {
            try
            {
                //message creation in DB
                Wt::Dbo::Transaction transaction(m_session, true);

                const Wt::WDateTime now = Wt::WDateTime::currentDateTime(); 
                
                Wt::Dbo::ptr<Echoes::Dbo::Media> medPtr = MediaResource::selectMedia(medId, orgId, m_session);
                if (medPtr)
                {
                    Echoes::Dbo::Message *newMsg = new Echoes::Dbo::Message();

                    newMsg->refAck = "";
                    newMsg->content = message;
                    newMsg->dest = medPtr->value;
                    newMsg->receiverSrv = conf.getRouteurHost(); 
                    newMsg->receiverPort = boost::lexical_cast<std::string>(conf.getRouteurPort());
                    
                    char hostname[255];
                    gethostname(hostname, 255);
                    
                    newMsg->senderSrv = hostname;
                    newMsg->senderPort = boost::lexical_cast<std::string>(conf.getServerPort());

                    newMsg->user = medPtr->user;
                    
                    Wt::Dbo::ptr<Echoes::Dbo::Message> newMsgPtr = m_session.add<Echoes::Dbo::Message>(newMsg);
                    newMsgPtr.flush();
                    
                    Echoes::Dbo::MessageTrackingEvent *newStateMsg = new Echoes::Dbo::MessageTrackingEvent();
                    
                    newStateMsg->date = now;
                    newStateMsg->message = newMsgPtr;
                    Wt::Dbo::ptr<Echoes::Dbo::MessageStatus> mstPtr = m_session.find<Echoes::Dbo::MessageStatus>()
                        .where(QUOTE(TRIGRAM_MESSAGE_STATUS ID) " = ?").bind(Echoes::Dbo::EMessageStatus::UNCREATED)
                        .where(QUOTE(TRIGRAM_MESSAGE_STATUS SEP "DELETE") " IS NULL");
                    newStateMsg->statut = mstPtr;
                    newStateMsg->text = "";
                    
                    Wt::Dbo::ptr<Echoes::Dbo::MessageTrackingEvent> newMsgTrEv = m_session.add<Echoes::Dbo::MessageTrackingEvent>(newStateMsg);
                    
                    res = serialize(newMsgPtr, responseMsg, EReturnCode::CREATED);

                    //message actual sending

                    const long long mtyId = medPtr->mediaType.id();
                    switch (mtyId)
                    {
                        case Echoes::Dbo::EMediaType::SMS:
                        {
                            Wt::log("debug") << " [Message Ressource] " << "Media SMS";

                            // Verifying the quota of sms
                            Wt::Dbo::ptr<Echoes::Dbo::Option> optPtr = m_session.find<Echoes::Dbo::Option>()
                                    .where(QUOTE(TRIGRAM_OPTION SEP TRIGRAM_OPTION_TYPE SEP TRIGRAM_OPTION_TYPE ID) " = ?").bind(Echoes::Dbo::EOptionType::QUOTA_SMS)
                                    .where(QUOTE(TRIGRAM_OPTION SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(orgId)
                                    .limit(1);

                            try
                            {
                                const int smsQuota = boost::lexical_cast<int>(optPtr->value);
                                if (smsQuota == 0)
                                {
                                    Wt::log("debug") << " [Message Ressource] " << "SMS quota 0";
                                    Wt::log("debug") << " [Message Ressource] " << "Sending e-mail instead.";

                                    sendMAIL(newMsgPtr, true);
                                }
                                else
                                {
                                    Wt::log("debug") << " [Message Ressource] " << "We send a SMS, quota : " << smsQuota;
                                    optPtr.modify()->value = boost::lexical_cast<string>(smsQuota - 1);
                                    optPtr.flush();
                                    sendSMS(newMsgPtr);
                                }
                            }
                            catch (boost::bad_lexical_cast const& e)
                            {
                                res = EReturnCode::SERVICE_UNAVAILABLE;
                                responseMsg = httpCodeToJSON(res, e);
                            }
                            break;
                        }
                        case Echoes::Dbo::EMediaType::MAIL:
                            Wt::log("debug") << " [Message Ressource] " << "Media MAIL";
                            sendMAIL(newMsgPtr);
                            break;
                        case Echoes::Dbo::EMediaType::MOBILE_APP:
                            Wt::log("debug") << " [Message Ressource] " << "Media MOBILE APP";
                            sendMobileApp(newMsgPtr);
                            break;
                        default:
                            Wt::log("error") << "[Message Resource] Unknown ID Media: " << mtyId;
                            break;
                    }


                    transaction.commit();
                }
                else
                {
                    res = EReturnCode::NOT_FOUND;
                    responseMsg = httpCodeToJSON(res, medPtr);
                }
            }
            catch (Wt::Dbo::Exception const& e)
            {
                res = EReturnCode::SERVICE_UNAVAILABLE;
                responseMsg = httpCodeToJSON(res, e);
            }
        } 
    }
    return res;
}

EReturnCode MessageResource::getMessageEvents(map<string, long long> &parameters, const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::collection < Wt::Dbo::ptr < Echoes::Dbo::MessageTrackingEvent >> atrPtrCol = m_session.find<Echoes::Dbo::MessageTrackingEvent>()
                .where(QUOTE(TRIGRAM_MESSAGE_TRACKING_EVENT SEP TRIGRAM_MESSAGE SEP TRIGRAM_MESSAGE ID)" = ? ").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_MESSAGE_TRACKING_EVENT SEP "DELETE") " IS NULL")
                .orderBy(QUOTE(TRIGRAM_MESSAGE_TRACKING_EVENT SEP "DATE") " DESC");

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

EReturnCode MessageResource::getMessageEvent(map<string, long long> &parameters, const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        //ToDo(FPO): Check rights
        Wt::Dbo::ptr < Echoes::Dbo::MessageTrackingEvent > atrPtrCol = m_session.find<Echoes::Dbo::MessageTrackingEvent>()
                .where(QUOTE(TRIGRAM_MESSAGE_TRACKING_EVENT SEP TRIGRAM_MESSAGE SEP TRIGRAM_MESSAGE ID)" = ? ").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_MESSAGE_TRACKING_EVENT ID)" = ? ").bind(pathElements[3])
                .where(QUOTE(TRIGRAM_MESSAGE_TRACKING_EVENT SEP "DELETE") " IS NULL");

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

EReturnCode MessageResource::getMessageStatus(map<string, long long> &parameters, const std::vector<std::string> &pathElements, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        //ToDo(FPO): Check rights
        Wt::Dbo::collection < Wt::Dbo::ptr < Echoes::Dbo::MessageTrackingEvent >> atrPtrCol = m_session.find<Echoes::Dbo::MessageTrackingEvent>()
                .where(QUOTE(TRIGRAM_MESSAGE_TRACKING_EVENT SEP TRIGRAM_MESSAGE SEP TRIGRAM_MESSAGE ID)" = ? ").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_MESSAGE_TRACKING_EVENT SEP "DELETE") " IS NULL")
                .orderBy(QUOTE(TRIGRAM_MESSAGE_TRACKING_EVENT SEP "DATE") " DESC");
        if(atrPtrCol.size() > 0)
        {
            Wt::Dbo::collection < Wt::Dbo::ptr < Echoes::Dbo::MessageTrackingEvent >>::iterator ptrTrackEvent = atrPtrCol.begin();
            res = EReturnCode::OK;
            
            responseMsg = "{";
            responseMsg += "\"message_id\" : " + boost::lexical_cast<std::string>(ptrTrackEvent->get()->message.id()) + ",";
            responseMsg += "\"state\" : " + boost::lexical_cast<std::string>(ptrTrackEvent->get()->statut.id()) + ",";
            responseMsg += "\"date\" : \"" + boost::lexical_cast<std::string>(ptrTrackEvent->get()->date.toString("dd/MM/yyyy hh:mm:ss")) + "\",";
            responseMsg += "\"dest\" : \"" + boost::lexical_cast<std::string>(ptrTrackEvent->get()->message->user->firstName) + " "
                            + boost::lexical_cast<std::string>(ptrTrackEvent->get()->message->user->lastName)
                            + " (" + boost::lexical_cast<std::string>(ptrTrackEvent->get()->message->dest) + ")" + "\"";
            responseMsg += "}";
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

EReturnCode MessageResource::processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    string nextElement = "";
    unsigned short indexPathElement = 1;
    vector<string> pathElements;
    map<string, long long> parameters;

    parameters["media_type_id"] = 0;
    parameters["media_id"] = 0;
    parameters["alert_id"] = 0;
    
    const string sRequest = processRequestParameters(request, pathElements, parameters);

    nextElement = getNextElementFromPath(indexPathElement, pathElements);

    if (nextElement.empty())
    {
        res = getMessages(parameters, orgId, responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);
            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.empty())
            {
                res = getMessage(pathElements, orgId, responseMsg);
            }
            else if(nextElement.compare("status") == 0)
            {
                res = getMessageStatus(parameters, pathElements, orgId, responseMsg);
            }
            else if(nextElement.compare("events") == 0)
            {
                nextElement = getNextElementFromPath(indexPathElement, pathElements);
                if (nextElement.empty())
                {
                    res = getMessageEvents(parameters, pathElements, orgId, responseMsg);
                }
                else
                {
                    boost::lexical_cast<unsigned long long>(nextElement);
                    nextElement = getNextElementFromPath(indexPathElement, pathElements);
                    if (nextElement.empty())
                    {
                        res = getMessageEvent(parameters, pathElements, orgId, responseMsg);
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
                const string err = "[Message Resource] bad nextElement";
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

EReturnCode MessageResource::processPostRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        res = postAlertMessage(parameters, pathElements, sRequest, orgId, responseMsg);
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Message Resource] bad nextElement";
        responseMsg = httpCodeToJSON(res, err);
    }

    return res;
}

EReturnCode MessageResource::sendMAIL
(
    Wt::Dbo::ptr<Echoes::Dbo::Message> msgPtr,
    bool overSMSQuota,
    vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrVector,
    Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr,
    Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr
 )
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    Wt::WString mailRecipientName = "";
    Wt::WString mailRecipient;
    if (amsPtr.isTransient() == 0)
    {
        mailRecipientName = amsPtr->media->user->firstName + " " + amsPtr->media->user->lastName;
    }
    string mailBody = "";
    const Wt::WDateTime now = Wt::WDateTime::currentDateTime(); 
    Wt::Mail::Message mailMessage;
    Wt::Mail::Client mailClient;

    if (amsPtr.isTransient() == 0)
    {
        // Normal case
        if (!overSMSQuota)
        {
            mailRecipient = amsPtr->media->value;
        }
        else
        {
            mailRecipient = amsPtr->media->user->eMail;

            mailBody += "MAIL sent instead of SMS (quota = 0) <br />";
        }
    }
    else
    {
        if (!overSMSQuota)
        {
            mailRecipient = amsPtr->media->value;
        }
        else
        {
            mailRecipient = amsPtr->media->user->eMail;

            mailBody += "MAIL sent instead of SMS (quota = 0) <br />";
        } 
    }

    if (amsPtr.isTransient() == 0)
    {
        mailBody += amsPtr->message.toUTF8();
    }
    else
    {
        mailBody += msgPtr->content.get().toUTF8();
    }
    
    if (ivaPtrVector.size() > 0)
    {
        replaceVariablesInMessage(ivaPtrVector, alePtr, mailBody);
    }
    
    Wt::log("debug") << " [Alert Resource] " << mailBody;

    mailMessage.setFrom(Wt::Mail::Mailbox(conf.getAlertMailSenderAddress(), conf.getAlertMailSenderName()));
    mailMessage.addRecipient(Wt::Mail::To, Wt::Mail::Mailbox(mailRecipient.toUTF8(), mailRecipientName));
    if (alePtr.isTransient() == 0)
    {
        mailMessage.setSubject("[Echoes Alert] " + alePtr->name);
    }
    else
    {
        mailMessage.setSubject("[Echoes Alert]");
    }
    
    mailMessage.addHtmlBody(mailBody);
    mailClient.connect(conf.getSMTPHost(), conf.getSMTPPort());
    mailClient.send(mailMessage);

    Wt::log("debug") << " [Class:AlertSender] " << "insert date of last send in db : " << now.toString();
    if (amsPtr.isTransient() == 0)
    {
        amsPtr.modify()->lastSend = now;
    }

    //create new state of message
    //msgPtr.modify()->sendDate = now;
    msgPtr.modify()->content = mailBody;

    res = EReturnCode::OK;

    return res;
}

EReturnCode MessageResource::sendSMS
(
 Wt::Dbo::ptr<Echoes::Dbo::Message> msgPtr,
 vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrVector,
 Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr,
 Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr
 )
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    const Wt::WDateTime now = Wt::WDateTime::currentDateTime();
    
    long long alertID;
    alertID = amsPtr->id;

    Wt::log("debug") << " [Alert Resource] New SMS for " << msgPtr->dest << " : " << msgPtr->content;

    ItookiSMSSender itookiSMSSender(m_session);

    if (!itookiSMSSender.send(msgPtr->dest.get().toUTF8(), msgPtr->content.get().toUTF8(), alertID, msgPtr))
    {
        if (amsPtr.isTransient() == 0)
        {
            amsPtr.modify()->lastSend = now;
        }
        
        res = EReturnCode::OK;
    }

    return res;
    
}

EReturnCode MessageResource::sendMobileApp
(
    Wt::Dbo::ptr<Echoes::Dbo::Message> msgPtr,
    vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue >> ivaPtrVector,
    Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr,
    Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr
)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    const Wt::WDateTime now = Wt::WDateTime::currentDateTime(); //for setting the send date of the alert
    
    string mobileApp;
    Wt::WString mobileName;
    
    if (amsPtr.isTransient() == 0)
    {
        mobileApp = amsPtr->message.toUTF8();
        replaceVariablesInMessage(ivaPtrVector, alePtr, mobileApp);
        mobileName = amsPtr->media->value;
        msgPtr.modify()->content = mobileApp;
        amsPtr.modify()->lastSend = now;
    }
    else
    {
        mobileApp = msgPtr->content.get().toUTF8();
        mobileName = amsPtr->media->value;
    }
    //create new state of message
    //msgPtr.modify()->sendDate = now;
    
    Wt::log("debug") << " [Alert Resource] New Alert for mobileApp : " << mobileName << " : " << mobileApp;
    
    res = EReturnCode::OK;

    return res;
}

void MessageResource::replaceVariablesInMessage(vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrVector, Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr, string &message)
{
    const Wt::WDateTime now = Wt::WDateTime::currentDateTime();
    
    Wt::Dbo::ptr<Echoes::Dbo::AlertSequence> asePtr = alePtr->alertSequence;
    int cpt = 0;
    while (asePtr)
    {
        for (vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue> >::const_iterator it = ivaPtrVector.begin(); it != ivaPtrVector.end(); ++it)
        {
            if (it->get()->informationData == asePtr->alertValue->informationData)
            {
                boost::replace_all(message, "%value" + boost::lexical_cast<string> (cpt) + "%", it->get()->value.toUTF8());
                boost::replace_all(message, "%threshold" + boost::lexical_cast<string> (cpt) + "%", asePtr->alertValue->value.toUTF8());
                boost::replace_all(message, "%detection-time" + boost::lexical_cast<string> (cpt) + "%", it->get()->creationDate.toString().toUTF8());
                boost::replace_all(message, "%alerting-time" + boost::lexical_cast<string> (cpt) + "%", now.toString().toUTF8());
                boost::replace_all(message, "%unit" + boost::lexical_cast<string> (cpt) + "%", it->get()->informationData->informationUnit->name.toUTF8());
            }
        }
        asePtr = asePtr->alertSequence;
        cpt++;
    }
}

