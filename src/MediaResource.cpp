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
    try
    {
        Wt::Dbo::Transaction transaction(*this->session);
        Wt::Dbo::collection<Wt::Dbo::ptr<MediaValue> > medias = session->find<MediaValue>()
                                                                .where("\"MEV_USR_USR_ID\" = ?").bind(boost::lexical_cast<std::string>(this->session->user().id()))
                                                                .where("\"MEV_MED_MED_ID\" = ?").bind(this->vPathElements[1]);
        if (medias.size() > 0)
        {
            res = "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<MediaValue> >::const_iterator i = medias.begin(); i != medias.end(); ++i)
            {
                res += i->modify()->toJSON();
                /*
                res += "{\n\"";
                res +="  \"id\" : \"" + boost::lexical_cast<std::string > (i->id()) + "\",\n\"";
                res += "  \"mev_value\" : \"" + boost::lexical_cast<std::string > ((*i).get()->value)+ "\"\n\"";
                res += "}\n";*/
            }
            res += "]";
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
            res = "[\n";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Media> >::const_iterator i = media.begin(); i != media.end(); i++) 
            {
                res += "\t" + i->modify()->toJSON();
                /*
                res += "{\n\"";
                res +="  \"id\" : \"" + boost::lexical_cast<std::string > (i->id()) + "\",\n\"";
                res += "  \"med_name\" : \"" + boost::lexical_cast<std::string > ((*i).get()->name) + "\"\n\"";
                res += "}\n";*/
            }
            res += "]";
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

void MediaResource::processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "", sRequest = "";

    sRequest = request2string(request);
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        responseMsg = postMedia(sRequest);
    }
    else
    {
        this->statusCode = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
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

string MediaResource::deleteMedia(string sRequest)
{
    string res = "";

    Wt::WString medId, mevValue;

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
    }  
    try
    {
        Wt::Dbo::Transaction transaction2(*session);



        std::string qryString = "DELETE FROM \"T_ALERT_MEDIA_SPECIALIZATION_AMS\" "
                                " WHERE \"AMS_ALE_ALE_ID\" IS NULL"
                                " AND \"AMS_MEV_MEV_ID\" IN "
                                " (SELECT \"MEV_ID\" FROM \"T_MEDIA_VALUE_MEV\" WHERE \"MEV_USR_USR_ID\" = " + boost::lexical_cast<std::string > (this->session->user().id())  + ")";

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

        Wt::Dbo::ptr<MediaValue> mediaValue = session->find<MediaValue>().where("\"MEV_MED_MED_ID\" = ?").bind(medId)
                                                          .where("\"MEV_USR_USR_ID\" = ?").bind(boost::lexical_cast<std::string > (this->session->user().id()))
                                                          .where("\"MEV_VALUE\" = ?").bind(mevValue);

        if(!mediaValue)
        {
            this->statusCode = 404;
            res = "{\"message\":\"Media not found\"}";
            return res; 
        }

        session->execute("DELETE FROM \"T_MEDIA_VALUE_MEV\" WHERE \"MEV_VALUE\" = \'" + boost::lexical_cast<std::string>(mevValue) + "\'"
                         " AND \"MEV_MED_MED_ID\" = " + boost::lexical_cast<std::string>(medId) +
                         " AND \"MEV_USR_USR_ID\" = " + boost::lexical_cast<std::string>(boost::lexical_cast<std::string > (this->session->user().id())));

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

void MediaResource::processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "", sRequest = "";

    sRequest = request2string(request);
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        responseMsg = deleteMedia(sRequest);
    }
    else
    {
        this->statusCode = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
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