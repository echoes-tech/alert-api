/* 
 * API PluginResource
 * @author ECHOES Technologies (GDR)
 * @date 10/02/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */


#include "PluginResource.h"

PluginResource::PluginResource(){
}


unsigned short PluginResource::getKeyValueForInformation(std::string &responseMsg) const
{
    unsigned short res = 500;
    int idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(*session);

        Wt::Dbo::ptr<Information2> ptrInfoKey = session->find<Information2>().where("\"SRC_ID\" = ?").bind(this->vPathElements[3])
                                                                             .where("\"SEA_ID\" = ?").bind(this->vPathElements[5])
                                                                             .where("\"PLG_ID_PLG_ID\" = ?").bind(this->vPathElements[1])
                                                                             .where("\"INF_VALUE_NUM\" = ?").bind(this->vPathElements[7])
                                                                             .where("\"INU_ID_INU_ID\" = ?").bind(this->vPathElements[9])
                                                                             .limit(1);

        if (!ptrInfoKey)
        {
            res = 404;
            responseMsg = "{\"message\":\"Information not found\"}";
            return res;
        }

        std::string queryString = 
        "SELECT iva FROM \"T_INFORMATION_VALUE_IVA\" iva WHERE \"IVA_ID\" IN ( SELECT \"IVA_ID\" FROM"
        "("
        "SELECT DISTINCT ON (\"IVA_VALUE\") \"IVA_VALUE\", \"IVA_ID\" FROM"
        "(" "SELECT iva.\"IVA_VALUE\", iva.\"IVA_ID\" FROM \"T_INFORMATION_VALUE_IVA\" iva"
        " WHERE \"SEA_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[5]) + 
        " AND \"SRC_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[3]) + 
        " AND \"PLG_ID_PLG_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[1]) + 
        " AND \"INF_VALUE_NUM\" = " + boost::lexical_cast<std::string>(this->vPathElements[7]) + 
        " AND \"INU_ID_INU_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[9]) + 
        " ORDER BY \"IVA_ID\" DESC LIMIT 50) sr"
        " ) sr_sr"
        ")";

        Wt::Dbo::collection<Wt::Dbo::ptr<InformationValue> > collPtrIva = session->query<Wt::Dbo::ptr<InformationValue> >(queryString);

        if(collPtrIva.size() > 0)
        {
            if(collPtrIva.size() > 1)
            {
            responseMsg += "[\n";
            }
            for (Wt::Dbo::collection<Wt::Dbo::ptr<InformationValue> >::const_iterator i = collPtrIva.begin(); i != collPtrIva.end(); i++)
            { 
                i->modify()->setId(i->id());
                responseMsg += i->modify()->toJSON();
                 ++idx;
                if(collPtrIva.size()-idx > 0)
                {
                    responseMsg.replace(responseMsg.size()-1, 1, "");
                    responseMsg += ",\n";
                }
            }
            if (collPtrIva.size() > 1)
            {
            responseMsg += "]";
            }
            res = 200;
        }
        else
        {
            res = 404;
            responseMsg = "{\"message\":\"Information value not found\"}";
            return res;
        }
    }
    catch (Wt::Dbo::Exception const &e)
    {
        Wt::log("error") << e.what();
        res = 503;
        responseMsg = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}

unsigned short PluginResource::getInformationListForPlugin(std::string &responseMsg) const
{
    unsigned short res = 500;
    int idx = 0;
    try
    {
        Wt::Dbo::Transaction transaction(*this->session);
        std::string queryStr = " SELECT inf FROM \"T_INFORMATION_INF\" inf "
                            "WHERE \"INF_DISPLAY\" = TRUE AND \"PLG_ID_PLG_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[1]) +
                            " AND \"INF_DELETE\" IS NULL";

        Wt::Dbo::Query<Wt::Dbo::ptr<Information2> > queryRes = session->query<Wt::Dbo::ptr<Information2> >(queryStr);

       Wt::Dbo::collection<Wt::Dbo::ptr<Information2> > information = queryRes.resultList();

        if(information.size() > 0 )
        {
            if(information.size() > 1)
            {
                responseMsg += "[\n";
            }
            for (Wt::Dbo::collection<Wt::Dbo::ptr<Information2> >::const_iterator i = information.begin(); i != information.end(); i++) 
            {
                responseMsg +=  i->modify()->toJSON();
                idx++;
                if(information.size()-idx > 0)
                {
                    responseMsg.replace(responseMsg.size()-1, 1, "");
                    responseMsg += ",\n";
                }
            }
            if(information.size() > 1)
            {
                responseMsg += "]\n";
            }
            res = 200;
            transaction.commit();
        }
        else
        {
            res = 404;
            responseMsg = "{\"message\":\"Information not found\"}";
            return res;
        }
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

void PluginResource::processGetRequest(Wt::Http::Response &response)
{
    std::string responseMsg = "", nextElement = "";
    
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->statusCode = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned int>(nextElement);

            nextElement = getNextElementFromPath();

            if(!nextElement.compare("informations"))
            {
                this->statusCode = getInformationListForPlugin(responseMsg);
            }
            else if (!nextElement.compare("sources"))
            {
                nextElement = getNextElementFromPath();
                boost::lexical_cast<unsigned int>(nextElement);
                nextElement = getNextElementFromPath();
                if (!nextElement.compare("searches"))
                {
                    nextElement = getNextElementFromPath();
                    boost::lexical_cast<unsigned int>(nextElement);
                    nextElement = getNextElementFromPath();
                    
                    if (!nextElement.compare("inf_values"))
                    {
                        nextElement = getNextElementFromPath();
                        boost::lexical_cast<unsigned int>(nextElement);
                        nextElement = getNextElementFromPath();
                        if (!nextElement.compare("units"))
                        {
                            nextElement = getNextElementFromPath();
                            boost::lexical_cast<unsigned int>(nextElement);
                            nextElement = getNextElementFromPath();
                            if (!nextElement.compare("informations"))
                            {
                                this->statusCode = getKeyValueForInformation(responseMsg);
                            }
                            else
                            {
                                this->statusCode = 400;
                                responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                            }
                        }
                        else
                        {
                            this->statusCode = 400;
                            responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                        }
                    }
                    else
                    {
                        this->statusCode = 400;
                        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                    }
                }
                else
                {
                    this->statusCode = 400;
                    responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
                }
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


void PluginResource::processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    
    return ;
}


void PluginResource::processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}


void PluginResource::processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}



void PluginResource::processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    
    return;
}


void PluginResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);

    return;
}


PluginResource::~PluginResource()
{
    beingDeleted();
}
