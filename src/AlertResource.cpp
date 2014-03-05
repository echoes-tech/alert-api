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
        Echoes::Dbo::SafeTransaction transaction(*m_session);
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
        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Alert>> queryRes = m_session->query<Wt::Dbo::ptr<Echoes::Dbo::Alert>>(queryStr);

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
        Echoes::Dbo::SafeTransaction transaction(*m_session);

        Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr = selectAlert(pathElements[1], orgId, *m_session);

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

EReturnCode AlertResource::getTrackingsForAlertsList(map<string, long long> &parameters, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    if (parameters["media_id"] <= 0)
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Alert Resource] media is empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    try
    {
        Echoes::Dbo::SafeTransaction transaction(*m_session);

        //ToDo(FPO): Check rights
        Wt::Dbo::collection < Wt::Dbo::ptr < Echoes::Dbo::AlertTracking >> atrPtrCol = m_session->find<Echoes::Dbo::AlertTracking>()
                .where(QUOTE(TRIGRAM_ALERT_TRACKING SEP TRIGRAM_MEDIA SEP TRIGRAM_MEDIA ID)" = ? ").bind(parameters["media_id"])
                .where(QUOTE(TRIGRAM_ALERT_TRACKING SEP "SEND_DATE") " IS NOT NULL")
                .orderBy(QUOTE(TRIGRAM_ALERT_TRACKING SEP "SEND_DATE") " DESC")
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
    else if (nextElement.compare("trackings") == 0)
    {
        res = getTrackingsForAlertsList(parameters, orgId,responseMsg);
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
    Wt::WString value;
    int threadSleep;

    // AVA attributs
    Wt::WString keyValue;
    long long infId;
    long long astId;
    long long plgId;
    long long acrId;

    // AMS attributs
    struct AmsStruct
    {
        long long medId;
        int snooze;
        Wt::WString message;
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
            value = result.get("value");
            threadSleep = result.get("thread_sleep");

            // AVA attributs
            if (!result.get("key_value").isNull())
            {
                keyValue = result.get("key_value");
            }
            else
            {
                keyValue = "";
            }
            
            infId = result.get("information_id");
            astId = result.get("asset_id");
            plgId = result.get("plugin_id");
            acrId = result.get("alert_criteria_id");

            Wt::Json::Array amsAttributs = result.get("alert_media_specialization");
            for (Wt::Json::Array::const_iterator it = amsAttributs.begin(); it < amsAttributs.end(); ++it)
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
        const string err = "[Alert Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Echoes::Dbo::SafeTransaction transaction(*m_session);

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
"                                                 AND " QUOTE(TRIGRAM_PLUGIN ID) " = " + boost::lexical_cast<string>(plgId) +
"                                                 AND " QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL"
"                                         )"
"                                 )"
"                               AND " QUOTE(TRIGRAM_SOURCE SEP "DELETE") " IS NULL"
"                       )"
"                     AND " QUOTE(TRIGRAM_SEARCH SEP "DELETE") " IS NULL"
"               )"
"             AND " QUOTE(TRIGRAM_FILTER SEP "DELETE") " IS NULL"
"       )"
"     AND " QUOTE(TRIGRAM_INFORMATION_DATA SEP TRIGRAM_INFORMATION SEP TRIGRAM_INFORMATION ID) " = " + boost::lexical_cast<string>(infId) +
"     AND " QUOTE(TRIGRAM_INFORMATION_DATA SEP TRIGRAM_ASSET SEP TRIGRAM_ASSET ID) " = " + boost::lexical_cast<string>(astId) +
"     AND " QUOTE(TRIGRAM_INFORMATION_DATA SEP "DELETE") " IS NULL"
"   LIMIT 1";

            Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::InformationData>> queryRes = m_session->query<Wt::Dbo::ptr<Echoes::Dbo::InformationData>>(queryStr);
    
            Wt::Dbo::ptr<Echoes::Dbo::InformationData> idaPtr = queryRes.resultValue();
            if (!idaPtr)
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, idaPtr);
                return res;
            }

            Wt::Dbo::ptr<Echoes::Dbo::AlertCriteria> acrPtr = m_session->find<Echoes::Dbo::AlertCriteria>()
                    .where(QUOTE(TRIGRAM_ALERT_CRITERIA ID) " = ?").bind(acrId)
                    .where(QUOTE(TRIGRAM_ALERT_CRITERIA SEP "DELETE") " IS NULL");
            if (!acrPtr)
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, acrPtr);
                return res;
            }

            vector<Wt::Dbo::ptr<Echoes::Dbo::Media>> medPtrVector;
            for (vector<AmsStruct>::const_iterator it = amsStructs.begin(); it < amsStructs.end(); ++it)
            {
                Wt::Dbo::ptr<Echoes::Dbo::Media> medPtr = MediaResource::selectMedia(it->medId, orgId, *m_session);
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

            Wt::Dbo::ptr<Echoes::Dbo::AlertValue> newAvaPtr = m_session->add<Echoes::Dbo::AlertValue>(ava);
            newAvaPtr.flush();

            Echoes::Dbo::Alert *ale = new Echoes::Dbo::Alert();
            ale->alertValue = newAvaPtr;
            ale->name = name;
            ale->creaDate = Wt::WDateTime::currentDateTime();
            ale->threadSleep = threadSleep;

            Wt::Dbo::ptr<Echoes::Dbo::Alert> newAlePtr = m_session->add<Echoes::Dbo::Alert>(ale);
            newAlePtr.flush();

            for (vector<AmsStruct>::iterator it = amsStructs.begin(); it < amsStructs.end(); ++it)
            {
                Echoes::Dbo::AlertMediaSpecialization *newAms = new Echoes::Dbo::AlertMediaSpecialization();
                newAms->media = medPtrVector[distance(amsStructs.begin(), it)];
                newAms->alert = newAlePtr;
                newAms->snoozeDuration = it->snooze;
                newAms->notifEndOfAlert = false;
                newAms->message = it->message;

                Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> newAmsPtr = m_session->add<Echoes::Dbo::AlertMediaSpecialization>(newAms);
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
    const Wt::WString mailRecipientName = amsPtr->media->user->firstName + " " + amsPtr->media->user->lastName;
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

    //TODO: à revoir pour les alertes complexes !!
    for (vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue> >::const_iterator it = ivaPtrVector.begin(); it != ivaPtrVector.end(); ++it)
    {
        boost::replace_all(mailBody, "%value%", it->get()->value.toUTF8());
        boost::replace_all(mailBody, "%threshold%", alePtr->alertValue->value.toUTF8());
        boost::replace_all(mailBody, "%detection-time%", it->get()->creationDate.toString().toUTF8());
        boost::replace_all(mailBody, "%alerting-time%", now.toString().toUTF8());
        boost::replace_all(mailBody, "%unit%", it->get()->informationData->informationUnit->name.toUTF8());
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

    //TODO: à revoir pour les alertes complexes !!
    for (vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue> >::const_iterator it = ivaPtrVector.begin(); it != ivaPtrVector.end(); ++it)
    {
        boost::replace_all(sms, "%value%", it->get()->value.toUTF8());
        boost::replace_all(sms, "%threshold%", alePtr->alertValue->value.toUTF8());
        boost::replace_all(sms, "%detection-time%", it->get()->creationDate.toString().toUTF8());
        boost::replace_all(sms, "%alerting-time%", now.toString().toUTF8());
    }

    Wt::log("info") << " [Alert Resource] New SMS for " << amsPtr->media->value << " : " << sms;

    ItookiSMSSender itookiSMSSender(this);

    if (!itookiSMSSender.send(amsPtr->media->value.toUTF8(), sms, atrPtr))
    {
        amsPtr.modify()->lastSend = now;
        res = EReturnCode::OK;
    }

    return res;
}

EReturnCode AlertResource::sendMobileApp
(
 vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue >> ivaPtrVector,
 Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr,
 Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> atrPtr,
 Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr
 )
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    const Wt::WDateTime now = Wt::WDateTime::currentDateTime(); //for setting the send date of the alert
    string mobileApp = amsPtr->message.toUTF8();

    //TODO: à revoir pour les alertes complexes !!
    for (vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue> >::const_iterator it = ivaPtrVector.begin(); it != ivaPtrVector.end(); ++it)
    {
        boost::replace_all(mobileApp, "%value%", it->get()->value.toUTF8());
        boost::replace_all(mobileApp, "%threshold%", alePtr->alertValue->value.toUTF8());
        boost::replace_all(mobileApp, "%detection-time%", it->get()->creationDate.toString().toUTF8());
        boost::replace_all(mobileApp, "%alerting-time%", now.toString().toUTF8());
        boost::replace_all(mobileApp, "%unit%", it->get()->informationData->informationUnit->name.toUTF8());
    }

    Wt::log("info") << " [Alert Resource] New Alerte for mobileApp : " << amsPtr->media->value << " : " << mobileApp;

    atrPtr.modify()->content = mobileApp;
    atrPtr.modify()->sendDate = now;

    amsPtr.modify()->lastSend = now;
    res = EReturnCode::OK;

    return res;
}

EReturnCode AlertResource::postAlertTracking(const vector<string> &pathElements, const string &sRequest, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    vector<long long> ivaIds;

    if (!sRequest.empty())
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
        const string err = "[Alert Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Echoes::Dbo::SafeTransaction transaction(*m_session);

            Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr = selectAlert(pathElements[1], orgId, *m_session);
            if (!alePtr)
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, alePtr);
                return res;
            }

            vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ivaPtrVector;
            for (vector<long long>::const_iterator it = ivaIds.begin(); it < ivaIds.end(); ++it)
            {
                Wt::Dbo::ptr<Echoes::Dbo::InformationValue> ivaPtr = m_session->find<Echoes::Dbo::InformationValue>()
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
                for (vector<Wt::Dbo::ptr<Echoes::Dbo::InformationValue>> ::const_iterator it = ivaPtrVector.begin(); it < ivaPtrVector.end(); ++it)
                {
                    //TODO(FPO): To Be Coded when we will implement the Alert Sequence
                }
            }
            else
            {
                if (ivaPtrVector.size() != 1 || ivaPtrVector[0]->informationData != alePtr->alertValue->informationData)
                {
                    res = EReturnCode::BAD_REQUEST;
                    const string err = "[Alert Resource] bad size or content of ivaPtrVector";
                    responseMsg = httpCodeToJSON(res, err);
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

                    Wt::Dbo::ptr<Echoes::Dbo::AlertTracking> newAtrPtr = m_session->add<Echoes::Dbo::AlertTracking>(newAtr);
                    newAtrPtr.flush();

                    Wt::log("info") << " [Alert Ressource] " << "Alert tracking number creation : " << newAtrPtr.id();

                    Wt::log("debug") << " [Alert Ressource] " << "snooze = " << it->get()->snoozeDuration;

                    switch (mtyID)
                    {
                        case Echoes::Dbo::EMediaType::SMS:
                        {
                            Wt::log("info") << " [Alert Ressource] " << "Media value SMS choosed for the alert : " << alePtr->name;

                            // Verifying the quota of sms
                            Wt::Dbo::ptr<Echoes::Dbo::Option> optPtr = m_session->find<Echoes::Dbo::Option>()
                                    .where(QUOTE(TRIGRAM_OPTION SEP TRIGRAM_OPTION_TYPE SEP TRIGRAM_OPTION_TYPE ID) " = ?").bind(Echoes::Dbo::EOptionType::QUOTA_SMS)
                                    .where(QUOTE(TRIGRAM_OPTION SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(orgId)
                                    .limit(1);

                            try
                            {
                                const int smsQuota = boost::lexical_cast<int>(optPtr->value);
                                if (smsQuota == 0)
                                {
                                    Wt::log("info") << " [Alert Ressource] " << "SMS quota 0 for alert : " << alePtr->name;
                                    Wt::log("info") << " [Alert Ressource] " << "Sending e-mail instead.";

                                    sendMAIL(ivaPtrVector, alePtr, newAtrPtr, *it, true);
                                }
                                else
                                {
                                    Wt::log("debug") << " [Alert Ressource] " << "We send a SMS, quota : " << smsQuota;
                                    optPtr.modify()->value = boost::lexical_cast<string>(smsQuota - 1);
                                    optPtr.flush();
                                    sendSMS(ivaPtrVector, alePtr, newAtrPtr, *it);
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
                            Wt::log("info") << " [Alert Ressource] " << "Media value MAIL choosed for the alert : " << alePtr->name;
                            sendMAIL(ivaPtrVector, alePtr, newAtrPtr, *it);
                            break;
                        case Echoes::Dbo::EMediaType::MOBILE_APP:
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

EReturnCode AlertResource::processPostRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        res = postAlert(sRequest,orgId, responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.compare("trackings") == 0)
            {
                res = postAlertTracking(pathElements, sRequest, orgId, responseMsg);
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
        Echoes::Dbo::SafeTransaction transaction(*m_session);

        Wt::Dbo::ptr<Echoes::Dbo::Alert> alePtr = selectAlert(pathElements[1], orgId, *m_session);

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

