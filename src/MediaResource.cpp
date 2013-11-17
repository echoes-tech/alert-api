/* 
 * API MediaResource
 * @author ECHOES Technologies (GDR)
 * @date 11/02/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */
#include "tools/JsonSerializer.h"
#include "MediaResource.h"

using namespace std;

MediaResource::MediaResource() : PublicApiResource::PublicApiResource()
{
}

MediaResource::~MediaResource()
{
}

unsigned short MediaResource::getListValueForMedia(string &responseMsg)
{
    unsigned short res = 500;
//    unsigned idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);
        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::MediaValue> > medias = m_session.find<Echoes::Dbo::MediaValue>()
                .where("\"MEV_USR_USR_ID\" = ?").bind(boost::lexical_cast<string>(this->m_session.user().id()))
                .where("\"MEV_MED_MED_ID\" = ?").bind(this->m_pathElements[1])
                .where("\"MEV_DELETE\" is NULL");
        if (medias.size() > 0)
        {
            responseMsg += this->toJSON(medias);
            res = 200;
        }
        else
        {
            res = 404;
            responseMsg = "{\"message\":\"Media not found\"}";
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

unsigned short MediaResource::getMedia(string &responseMsg)
{
    unsigned short res = 500;
//    unsigned idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);
        string queryStr = "SELECT med FROM \"TR_MEDIA_MED\" med where \"MED_ID\" IN"
                " ("
                " SELECT \"MEV_MED_MED_ID\" FROM \"T_MEDIA_VALUE_MEV\" "
                " WHERE \"MEV_USR_USR_ID\" = " + boost::lexical_cast<string>(this->m_session.user().id()) +
                " AND \"MEV_DELETE\" IS NULL"
                " )"
                " AND \"MED_DELETE\" IS NULL";
 
        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Media> > queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::Media> >(queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Media> > media = queryRes.resultList();
        
        if (media.size() > 0)
        {
            responseMsg += this->toJSON(media);
            res = 200;
        }
        else 
        {
            res = 404;
            responseMsg = "{\"message\":\"Media not found\"}";
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

void MediaResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";
    
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->m_statusCode = getMedia(responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned int>(nextElement);

            nextElement = getNextElementFromPath();

            if(!nextElement.compare(""))
            {
                this->m_statusCode = getListValueForMedia(responseMsg);
            }
            else
            {
                this->m_statusCode = 400;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            this->m_statusCode = 400;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(this->m_statusCode);
    response.out() << responseMsg;
    return;
}

unsigned short MediaResource::postMedia(string &responseMsg, const string &sRequest)
{
     Wt::log("info") << "[Alert Ressource] JSON received:" << sRequest; //TODO :: test pour l'appli mobile à retirer
    unsigned short res = 500;
    Wt::WString mevValue;
    long long medId;
    try
    {

        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);

        medId = result.get("med_id");
        mevValue = result.get("mev_value");
    }

    catch (Wt::Json::ParseError const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON\"}";
        Wt::log("warning") << "[Media Ressource] " << e.what();
        return res;
    }
    catch (Wt::Json::TypeException const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
        Wt::log("warning") << "[Media Ressource] " << e.what();
        return res;
    }  
    try
    {
        Wt::Dbo::Transaction transaction(m_session);
        Wt::Dbo::ptr<Echoes::Dbo::Media> media = m_session.find<Echoes::Dbo::Media>().where("\"MED_ID\" = ?").bind(medId);

        if(!media)
        {
            res = 404;
            responseMsg = "{\"message\":\"Media not found\"}";
            return res; 
        }

        Echoes::Dbo::MediaValue *mev = new Echoes::Dbo::MediaValue();
        mev->user= this->m_session.user();
        mev->media = media;
        mev->value = mevValue;
        mev->token = Wt::WRandom::generateId(25);
        Wt::Dbo::ptr<Echoes::Dbo::MediaValue> ptrMev = m_session.add<Echoes::Dbo::MediaValue>(mev);
        ptrMev.flush();
        ptrMev.modify()->setId(ptrMev.id());
        responseMsg = ptrMev->toJSON();        
        
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

 unsigned short MediaResource::postMediaValidation(std::string &responseMsg, const std::string &sRequest)
 {
     Wt::log("info") << "[Alert Ressource] JSON received:" << sRequest ; //TODO :: test pour l'appli mobile à retirer
    unsigned short res = 500;
    Wt::WString mevToken;
    bool mevValidate;
    try
    {

        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);

        mevValidate = result.get("mev_validation");
        mevToken = result.get("mev_token");
    }

    catch (Wt::Json::ParseError const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON\"}";
        Wt::log("warning") << "[Media Ressource]  " << e.what();
        return res;
    }
    catch (Wt::Json::TypeException const& e)
    {
        res = 400;
        responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
        Wt::log("warning") << "[Media Ressource] " << e.what();
        return res;
    }  
    try
    {
        Wt::Dbo::Transaction transaction(m_session);
        Wt::Dbo::ptr<Echoes::Dbo::MediaValue> mev = m_session.find<Echoes::Dbo::MediaValue>().where("\"MEV_MED_MED_ID\" = ?").bind(m_pathElements[1])
                                                               .where("\"MEV_ID\" = ?").bind(m_pathElements[3])
                                                               .where("\"MEV_USR_USR_ID\" = ?").bind(m_session.user().id())
                                                               .where("\"MEV_TOKEN\" = ?").bind(mevToken);

        if(!mev)
        {
            res = 404;
            responseMsg = "{\"message\":\"Media value Not found\"}";
        }
        else
        {
        mev.modify()->isConfirmed = mevValidate;
        mev.flush();
        mev.modify()->setId(mev.id());
        responseMsg = mev.modify()->toJSON();        
        
        transaction.commit();

        res = 200;
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

unsigned short MediaResource::postMediaSpecialization(string &responseMsg, const string &sRequest)
{
    unsigned short res = 500;
    long long mevId;
    int snooze;
    try
    {
        
        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);
        //descriptif
        mevId = result.get("mev_id");
        snooze = result.get("ams_snooze");
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
    try
    {
        Wt::Dbo::Transaction transaction(m_session);
        
        Echoes::Dbo::AlertMediaSpecialization *ams = new Echoes::Dbo::AlertMediaSpecialization();
        Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr;

        Wt::Dbo::ptr<Echoes::Dbo::MediaValue> mevPtr = m_session.find<Echoes::Dbo::MediaValue>().where("\"MEV_ID\" = ?").bind(mevId);
        if (mevPtr)
        {
            ams->snoozeDuration = snooze;
            ams->mediaValue = mevPtr;
            ams->notifEndOfAlert = false;
            amsPtr = m_session.add<Echoes::Dbo::AlertMediaSpecialization>(ams);
            amsPtr.flush();
            amsPtr.modify()->setId(amsPtr.id());
            responseMsg = amsPtr->toJSON();
            res = 200;
        }
        else 
        {
            res = 404;
            responseMsg = "{\"message\":\"Media Value not found\"}";
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

void MediaResource::processPostRequest(Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";

    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->m_statusCode = postMedia(responseMsg, m_requestData);
    }
    else if (!nextElement.compare("specializations"))
    {
         nextElement = getNextElementFromPath();
        if (!nextElement.compare(""))
        {
            this->m_statusCode = postMediaSpecialization(responseMsg, m_requestData);
        }
        else
        {
            this->m_statusCode = 400;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();
            if(!nextElement.compare("media_values"))
            {
                nextElement = getNextElementFromPath();
                 boost::lexical_cast<unsigned long long>(nextElement);

                nextElement = getNextElementFromPath();
                if(!nextElement.compare("validate"))
                {
                        this->m_statusCode = postMediaValidation(responseMsg, m_requestData);
                }
                else
                {
                     this->m_statusCode = 400;
                     responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                }
            }
            else
            {
                 this->m_statusCode = 400;
                 responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }catch(boost::bad_lexical_cast &)
        {
            this->m_statusCode = 400;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }            
    }

    response.setStatus(this->m_statusCode);
    response.out() << responseMsg;
    return ;
}


void MediaResource::processPutRequest(Wt::Http::Response &response)
{
    return;
}


void MediaResource::processPatchRequest(Wt::Http::Response &response)
{
    return;
}

unsigned short MediaResource::deleteMedia(string &responseMsg)
{
    unsigned short res = 500;

    try
    {
        
        Wt::Dbo::Transaction transaction2(m_session);
        string qryString = "DELETE FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" "
                                " WHERE \"AMS_ALE_ALE_ID\" IS NULL"
                                " AND \"AMS_MEV_MEV_ID\" = " + boost::lexical_cast<string > (this->m_pathElements[1]);  

        m_session.execute(qryString);

        transaction2.commit();
    }
    catch (Wt::Dbo::Exception e)
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    try
    {
        Wt::Dbo::Transaction transaction(m_session);


        string queryStr = "SELECT mev FROM \"T_MEDIA_VALUE_MEV\" mev"
                " WHERE \"MEV_USR_USR_ID\" IN "
                " (Select \"T_USER_USR_USR_ID\" "
                " FROM \"TJ_USR_ORG\" " 
                " WHERE \"T_ORGANIZATION_ORG_ORG_ID\" = " + boost::lexical_cast<string>(m_session.user()->organization.id()) +
                " )"
                "and \"MEV_ID\" = " + this->m_pathElements[1];

        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::MediaValue>> resQuery = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::MediaValue>>(queryStr);
        Wt::Dbo::ptr<Echoes::Dbo::MediaValue> mediaValPtr = resQuery.resultValue();

        if(mediaValPtr)
        {
            mediaValPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
            res = 204;
            responseMsg = "";
        }
        else
        {
            res = 404;
            responseMsg = "{\"message\":\"Media not found\"}";
        }
        
        transaction.commit();
    }
    catch (Wt::Dbo::Exception e)
    {
        Wt::log("error") << e.what();
        res = 409;
        responseMsg = "{\"message\":\"Conflict, an alert use this media\"}";
    }
    return res;
}

unsigned short MediaResource::deleteMediaSpecialization(string &responseMsg)
{
    unsigned short res = 500;
    try 
    {
        Wt::Dbo::Transaction transaction(m_session);
        Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization> amsPtr = m_session.find<Echoes::Dbo::AlertMediaSpecialization>()
                .where("\"AMS_ID\" = ?").bind(m_pathElements[2])
                .where("\"AMS_ALE_ALE_ID\" IS NULL");

        if(!amsPtr)
        {
            res = 404;
            responseMsg = "{\"message\":\"Media not found\"}";
            return res; 
        }

        string executeString = " DELETE FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" "
                " WHERE \"AMS_ID\" = " + boost::lexical_cast<string>(this->m_pathElements[2]) +
                " AND \"AMS_ALE_ALE_ID\" IS NULL";
        m_session.execute(executeString);
        transaction.commit();

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

void MediaResource::processDeleteRequest(Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";

    nextElement = getNextElementFromPath();

    if (!nextElement.compare("specializations"))
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();
            if(!nextElement.compare(""))
            {
                responseMsg = deleteMediaSpecialization(responseMsg);
            }
            else
            {
                this->m_statusCode = 400;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            this->m_statusCode = 400;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }
    else 
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();
            if(!nextElement.compare(""))
            {
                responseMsg = deleteMedia(responseMsg);
            }
            else
            {
                this->m_statusCode = 400;
                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
            }
        }
        catch(boost::bad_lexical_cast &)
        {
            this->m_statusCode = 400;
            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
        }
    }

    response.setStatus(this->m_statusCode);
    response.out() << responseMsg;
    return;
}


void MediaResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);

    return;
}

