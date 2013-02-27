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


using namespace std;

MediaResource::MediaResource(){
}

string MediaResource::getListValueForMedia()
{
    string res = "";
    int idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(*this->session);
        Wt::Dbo::collection<Wt::Dbo::ptr<MediaValue> > medias = session->find<MediaValue>()
                                                                .where("\"MEV_USR_USR_ID\" = ?").bind(boost::lexical_cast<std::string>(this->session->user().id()))
                                                                .where("\"MEV_MED_MED_ID\" = ?").bind(this->vPathElements[1]);
        if (medias.size() > 0)
        {
            if(medias.size() > 1)
            {
                res = "[\n";
            }
            for (Wt::Dbo::collection<Wt::Dbo::ptr<MediaValue> >::const_iterator i = medias.begin(); i != medias.end(); ++i)
            {
                res += i->modify()->toJSON();
                 ++idx;
                if(medias.size()-idx > 0)
                {
                    res.replace(res.size()-1, 1, "");
                    res += ",\n";
                }
                /*
                res += "{\n\"";
                res +="  \"id\" : \"" + boost::lexical_cast<std::string > (i->id()) + "\",\n\"";
                res += "  \"mev_value\" : \"" + boost::lexical_cast<std::string > ((*i).get()->value)+ "\"\n\"";
                res += "}\n";*/
            }
            if(medias.size() > 1)
            {
                res += "]";
            }
            this->statusCode = 200;
        }
        else
        {
            this->statusCode = 404;
            res = "{\"message\":\"Media not found\"}";
            return res;
        }

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}

string MediaResource::getMedia()
{
    string res = "";
    int idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(*this->session);
        std::string queryStr = "SELECT med FROM \"T_MEDIA_MED\" med where \"MED_ID\" IN"
                        "("
                            "SELECT \"MEV_MED_MED_ID\" FROM \"T_MEDIA_VALUE_MEV\" "
                            "WHERE \"MEV_USR_USR_ID\" = " + boost::lexical_cast<std::string > (this->session->user().id()) +
                        ")"
                        "AND \"MED_DELETE\" IS NULL";

        Wt::Dbo::Query<Wt::Dbo::ptr<Media> > queryRes = session->query<Wt::Dbo::ptr<Media> >(queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<Media> > media = queryRes.resultList();

        if (media.size() > 0)
        {
            if(media.size() > 1)
            {
            res += "[\n";
            }
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Media> >::const_iterator i = media.begin(); i != media.end(); i++) 
            {
                res += "\t" + i->modify()->toJSON();
                ++idx;
                if(media.size()-idx > 0)
                {
                    res.replace(res.size()-1, 1, "");
                    res += ",\n";
                }
                /*
                res += "{\n\"";
                res +="  \"id\" : \"" + boost::lexical_cast<std::string > (i->id()) + "\",\n\"";
                res += "  \"med_name\" : \"" + boost::lexical_cast<std::string > ((*i).get()->name) + "\"\n\"";
                res += "}\n";*/
            }
            if(media.size() > 1)
            {
                res += "]";               
            }

            this->statusCode = 200;
        }
        else 
        {
            this->statusCode = 404;
            res = "{\"message\":\"Media not found\"}";
            return res;
        }

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}

void MediaResource::processGetRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";
    
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        responseMsg = getMedia();
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned int>(nextElement);

            nextElement = getNextElementFromPath();

            if(!nextElement.compare(""))
            {
                responseMsg = getListValueForMedia();
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

string MediaResource::postMedia(string sRequest)
{
    string res = "";
    Wt::WString usrId, medId, mevValue;

    try
    {
        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);

        //usrId = result.get("user_id");
        medId = result.get("med_id");
        mevValue = result.get("mev_value");
    }

    catch (Wt::Json::ParseError const& e)
    {
        this->statusCode = 400;
        res = "{\"message\":\"Problems parsing JSON\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON:" << sRequest;
        return res;
    }
    catch (Wt::Json::TypeException const& e)
    {
        this->statusCode = 400;
        res = "{\"message\":\"Problems parsing JSON.\"}";
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
            this->statusCode = 404;
            res = "{\"message\":\"Not found\"}";
            return res; 
        }

        MediaValue *mev = new MediaValue();
        mev->user= ptrUser;
        mev->media = media;
        mev->value = mevValue;
        Wt::Dbo::ptr<MediaValue> ptrMev = session->add<MediaValue>(mev);

        transaction.commit();

        this->statusCode = 200;
        res = "{\"message\":\"Media added\"}";

    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    
    return res;
}


string MediaResource::postMediaSpecialization(string sRequest)
{
    string res = "";
    Wt::WString snooze, mevId;
      
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
        this->statusCode = 400;
        res = "{\"message\":\"Problems parsing JSON\"}";
        Wt::log("warning") << "[Alert Ressource] Problems parsing JSON:" << sRequest;
        return res;
    }
    catch (Wt::Json::TypeException const& e)
    {
        this->statusCode = 400;
        res = "{\"message\":\"Problems parsing JSON.\"}";
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
        ams->snoozeDuration = boost::lexical_cast<int>(snooze);
        ams->mediaValue = mevPtr;
        ams->notifEndOfAlert = false;
        amsPtr = session->add<AlertMediaSpecialization>(ams);
        }
        else 
        {
            this->statusCode = 404;
            res = "{\"message\":\"Media Value not found\"}";
        }
        res = amsPtr.modify()->toJSON();
        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\"message\":\"Service Unavailable\"}";
    }
    this->statusCode = 200;
    return res;
}

void MediaResource::processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "", sRequest = "";

    sRequest = request2string(request);
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        responseMsg = postMedia(sRequest);
    }
    else if (!nextElement.compare("specializations"))
    {
         nextElement = getNextElementFromPath();
        if (!nextElement.compare(""))
        {
            responseMsg = postMediaSpecialization(sRequest);
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
                responseMsg = deleteMediaSpecialization();
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
                responseMsg = deleteMedia();
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

string MediaResource::deleteMedia()
{
    string res = "";

   /* Wt::WString medId, mevValue;

    try
    {
        Wt::Json::Object result;                   
        Wt::Json::parse(sRequest, result);

        medId = result.get("med_id");
        mevValue = result.get("mev_value");
    }
    catch (Wt::Json::ParseError const& e)
    {
        this->statusCode = 400;
        res = "{\"message\":\"Problems parsing JSON\"}";
        Wt::log("warning") << "[Media Ressource] Problems parsing JSON:" << sRequest;
        return res;
    }
    catch (Wt::Json::TypeException const& e)
    {
        this->statusCode = 400;
        res = "{\"message\":\"Problems parsing JSON.\"}";
        Wt::log("warning") << "[Media Ressource] Problems parsing JSON.:" << sRequest;
        return res;
    }  */
    try
    {
        Wt::Dbo::Transaction transaction2(*session);



        std::string qryString = "DELETE FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" "
                                " WHERE \"AMS_ALE_ALE_ID\" IS NULL"
                                " AND \"AMS_MEV_MEV_ID\" = " + boost::lexical_cast<std::string > (this->vPathElements[1]);
                                //" (SELECT \"MEV_ID\" FROM \"T_MEDIA_VALUE_MEV\" WHERE \"MEV_USR_USR_ID\" = " + boost::lexical_cast<std::string > (this->session->user().id())  + ")";

        session->execute(qryString);

        transaction2.commit();
    }
    catch (Wt::Dbo::Exception e)
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    try
    {
        Wt::Dbo::Transaction transaction(*session);

        Wt::Dbo::ptr<MediaValue> mediaValue = session->find<MediaValue>().where("\"MEV_ID\" = ?").bind(vPathElements[1]);
                                                        /*.where("\"MEV_MED_MED_ID\" = ?").bind(medId)
                                                          .where("\"MEV_USR_USR_ID\" = ?").bind(boost::lexical_cast<std::string > (this->session->user().id()))
                                                          .where("\"MEV_VALUE\" = ?").bind(mevValue);*/

        if(!mediaValue)
        {
            this->statusCode = 404;
            res = "{\"message\":\"Media not found\"}";
            return res; 
        }

        session->execute("DELETE FROM \"T_MEDIA_VALUE_MEV\" WHERE \"MEV_ID\" = " + boost::lexical_cast<std::string > (this->vPathElements[1]));
                /*"\"MEV_VALUE\" = \'" + boost::lexical_cast<std::string>(mevValue) + "\'"
                         " AND \"MEV_MED_MED_ID\" = " + boost::lexical_cast<std::string>(medId) +
                         " AND \"MEV_USR_USR_ID\" = " + boost::lexical_cast<std::string>(boost::lexical_cast<std::string > (this->session->user().id())));*/

        transaction.commit();
        this->statusCode = 204;
        res = ""; //{\"message\":\"Media deleted\"}";
    }
    catch (Wt::Dbo::Exception e)
    {
        Wt::log("error") << e.what();
        this->statusCode = 409;
        res = "{\"message\":\"Conflict, an alert use this media\"}";
        return res;
    }

    return res;
}

string MediaResource::deleteMediaSpecialization()
{
    string res = "";
    try 
    {
        Wt::Dbo::Transaction transaction(*this->session);
        Wt::Dbo::ptr<AlertMediaSpecialization> amsPtr = session->find<AlertMediaSpecialization>().where("\"AMS_ID\" = ?").bind(vPathElements[2])
                                                                                                  .where("\"AMS_ALE_ALE_ID\" IS NULL");
        
        if(!amsPtr)
        {
            this->statusCode = 409;
            res = "{\"message\":\"media not found\"}";
            return res; 
        }
        
        std::string executeString =  " DELETE FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" "
                                    " WHERE \"AMS_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[2]) +
                                    " AND \"AMS_ALE_ALE_ID\" IS NULL";
        session->execute(executeString);
        transaction.commit();

        this->statusCode = 204;
        res = "";
    }
    catch (Wt::Dbo::Exception const& e) 
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}

void MediaResource::processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "", sRequest = "";

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
                responseMsg = deleteMediaSpecialization();
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
                responseMsg = deleteMedia();
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