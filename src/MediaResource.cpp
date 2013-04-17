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


#include "MediaResource.h"

MediaResource::MediaResource(){
}

unsigned short MediaResource::getListValueForMedia(std::string &responseMsg) const
{
    unsigned short res = 500;
    int idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(*this->session);
        Wt::Dbo::collection<Wt::Dbo::ptr<MediaValue> > medias = session->find<MediaValue>()
                                                                .where("\"MEV_USR_USR_ID\" = ?").bind(boost::lexical_cast<std::string>(this->session->user().id()))
                                                                .where("\"MEV_MED_MED_ID\" = ?").bind(this->vPathElements[1]);
        if (medias.size() > 0)
        {
            responseMsg = "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<MediaValue> >::const_iterator i = medias.begin(); i != medias.end(); ++i)
            {
                i->modify()->setId(i->id());                
                responseMsg += i->modify()->toJSON();
                 ++idx;
                if(medias.size()-idx > 0)
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

unsigned short MediaResource::getMedia(std::string &responseMsg) const
{
    unsigned short res = 500;
    int idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(*this->session);
        std::string queryStr = "SELECT med FROM \"T_MEDIA_MED\" med where \"MED_ID\" IN"
                " ("
                " SELECT \"MEV_MED_MED_ID\" FROM \"T_MEDIA_VALUE_MEV\" "
                " WHERE \"MEV_USR_USR_ID\" IN "
                " (Select \"T_USER_USR_USR_ID\" "
                " FROM \"TJ_USR_ORG\" "
                " WHERE \"T_ORGANIZATION_ORG_ORG_ID\" = " + boost::lexical_cast<std::string > (session->user()->currentOrganization.id()) +
                " )"
                " )"
                " AND \"MED_DELETE\" IS NULL";
 
        Wt::Dbo::Query<Wt::Dbo::ptr<Media> > queryRes = session->query<Wt::Dbo::ptr<Media> >(queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<Media> > media = queryRes.resultList();
        
        if (media.size() > 0)
        {
            responseMsg += "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Media> >::const_iterator i = media.begin(); i != media.end(); i++) 
            {
                i->modify()->setId(i->id());
                responseMsg += "\t" + i->modify()->toJSON();
                ++idx;
                if(media.size()-idx > 0)
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
    std::string responseMsg = "", nextElement = "";
    
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->statusCode = getMedia(responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned int>(nextElement);

            nextElement = getNextElementFromPath();

            if(!nextElement.compare(""))
            {
                this->statusCode = getListValueForMedia(responseMsg);
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

unsigned short MediaResource::postMedia(std::string &responseMsg, const std::string &sRequest)
{
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
        Wt::Dbo::Transaction transaction(*session);
        Wt::Dbo::ptr<User> ptrUser = session->find<User>().where("\"USR_ID\" = ?").bind(boost::lexical_cast<std::string > (this->session->user().id()));
        Wt::Dbo::ptr<Media> media = session->find<Media>().where("\"MED_ID\" = ?").bind(medId);

        if(!ptrUser || !media)
        {
            res = 404;
            responseMsg = "{\"message\":\"Not found\"}";
            return res; 
        }

        MediaValue *mev = new MediaValue();
        mev->user= ptrUser;
        mev->media = media;
        mev->value = mevValue;
        Wt::Dbo::ptr<MediaValue> ptrMev = session->add<MediaValue>(mev);
        ptrMev.flush();
        ptrMev.modify()->setId(ptrMev.id());
        responseMsg = ptrMev.modify()->toJSON();        
        
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


unsigned short MediaResource::postMediaSpecialization(std::string &responseMsg, const std::string &sRequest)
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
        Wt::Dbo::Transaction transaction(*session);
        
        AlertMediaSpecialization *ams = new AlertMediaSpecialization();
        Wt::Dbo::ptr<AlertMediaSpecialization> amsPtr;

        Wt::Dbo::ptr<MediaValue> mevPtr = session->find<MediaValue>().where("\"MEV_ID\" = ?").bind(mevId);
        if (mevPtr)
        {
            ams->snoozeDuration = snooze;
            ams->mediaValue = mevPtr;
            ams->notifEndOfAlert = false;
            amsPtr = session->add<AlertMediaSpecialization>(ams);
            amsPtr.modify()->setId(amsPtr.id());
            responseMsg = amsPtr.modify()->toJSON();
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

void MediaResource::processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    std::string responseMsg = "", nextElement = "", sRequest = "";

    sRequest = request2string(request);
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->statusCode = postMedia(responseMsg, sRequest);
    }
    else if (!nextElement.compare("specializations"))
    {
         nextElement = getNextElementFromPath();
        if (!nextElement.compare(""))
        {
            this->statusCode = postMediaSpecialization(responseMsg, sRequest);
        }
         /////////// a supprimer
        else
        {
             try
        {
            boost::lexical_cast<unsigned int>(nextElement);

            nextElement = getNextElementFromPath();
            if(!nextElement.compare(""))
            {
                this->statusCode = deleteMediaSpecialization(responseMsg);
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
         /////////// a supprimer
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
                this->statusCode = deleteMedia(responseMsg);
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
        //this->statusCode = 400;
        //responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }

    response.setStatus(this->statusCode);
    response.out() << responseMsg;
    return ;
}


void MediaResource::processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}


void MediaResource::processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}

unsigned short MediaResource::deleteMedia(std::string &responseMsg)
{
    unsigned short res = 500;


    try
    {
        
        Wt::Dbo::Transaction transaction2(*session);
        std::string qryString = "DELETE FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" "
                                " WHERE \"AMS_ALE_ALE_ID\" IS NULL"
                                " AND \"AMS_MEV_MEV_ID\" = " + boost::lexical_cast<std::string > (this->vPathElements[1]);  

        session->execute(qryString);

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
        Wt::Dbo::Transaction transaction(*session);


        std::string queryStr = "SELECT mev FROM \"T_MEDIA_VALUE_MEV\" mev"
                " WHERE \"MEV_USR_USR_ID\" IN "
                " (Select \"T_USER_USR_USR_ID\" "
                " FROM \"TJ_USR_ORG\" " 
                " WHERE \"T_ORGANIZATION_ORG_ORG_ID\" = " + boost::lexical_cast<std::string>(session->user()->currentOrganization.id()) +
                " )"
                "and \"MEV_ID\" = " + this->vPathElements[1];

        Wt::Dbo::Query<Wt::Dbo::ptr<MediaValue>> resQuery = session->query<Wt::Dbo::ptr<MediaValue>>(queryStr);
        Wt::Dbo::ptr<MediaValue> mediaValPtr = resQuery.resultValue();

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

unsigned short MediaResource::deleteMediaSpecialization(std::string &responseMsg)
{
    unsigned short res = 500;
    try 
    {
        Wt::Dbo::Transaction transaction(*this->session);
        Wt::Dbo::ptr<AlertMediaSpecialization> amsPtr = session->find<AlertMediaSpecialization>().where("\"AMS_ID\" = ?").bind(vPathElements[2])
                                                                                                  .where("\"AMS_ALE_ALE_ID\" IS NULL");
        
        if(!amsPtr)
        {
            res = 409;
            responseMsg = "{\"message\":\"media not found\"}";
            return res; 
        }
        
        std::string executeString =  " DELETE FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" "
                                    " WHERE \"AMS_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[2]) +
                                    " AND \"AMS_ALE_ALE_ID\" IS NULL";
        session->execute(executeString);
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

void MediaResource::processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    std::string responseMsg = "", nextElement = "", sRequest = "";

    sRequest = request2string(request);
    nextElement = getNextElementFromPath();
    
    if (!nextElement.compare("specializations"))
    {
        try
        {
            boost::lexical_cast<unsigned int>(nextElement);

            nextElement = getNextElementFromPath();
            if(!nextElement.compare(""))
            {
                responseMsg = deleteMediaSpecialization(responseMsg);
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
    else 
    {
        try
        {
            boost::lexical_cast<unsigned int>(nextElement);

            nextElement = getNextElementFromPath();
            if(!nextElement.compare(""))
            {
                responseMsg = deleteMedia(responseMsg);
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


void MediaResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);

    return;
}


MediaResource::~MediaResource()
{
    beingDeleted();
}