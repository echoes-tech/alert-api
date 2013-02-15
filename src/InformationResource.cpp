/* 
 * API InformationResource
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


#include "InformationResource.h"

using namespace std;

InformationResource::InformationResource(){
}

string InformationResource::getKeyValueForInformation()
{
    string res = "";
    /*int seaId, srcId, plgId, valNum, astId;
    try 
    {
        seaId = boost::lexical_cast<int>(splitPath[2]);
        srcId = boost::lexical_cast<int>(splitPath[3]);
        plgId = boost::lexical_cast<int>(splitPath[4]);
        valNum = boost::lexical_cast<int>(splitPath[5]);
        astId = boost::lexical_cast<int>(splitPath[6]);
    }   
    catch (boost::bad_lexical_cast &) 
    {
        response.setStatus(422);
        response.out() << "{\"message\":\"Validation Failed\"}";
        return;
    } */
    try
    {
        Wt::Dbo::Transaction transaction(*session);

        Wt::Dbo::ptr<Information2> ptrInfoKey = session->find<Information2>().where("\"INF_NAME\" = ?").bind(this->vPathElements[1])
                                    .limit(1);

        if (!ptrInfoKey)
        {
            this->statusCode = 404;
            res = "{\"message\":\"Information not found\"}";
            return res;
        }

        std::string queryString = 
        "SELECT iva FROM \"T_INFORMATION_VALUE_IVA\" iva WHERE \"IVA_ID\" IN ( SELECT \"IVA_ID\" FROM"
        "("
        "SELECT DISTINCT ON (\"IVA_VALUE\") \"IVA_VALUE\", \"IVA_ID\" FROM"
        "(" "SELECT iva.\"IVA_VALUE\", iva.\"IVA_ID\" FROM \"T_INFORMATION_VALUE_IVA\" iva"
        " WHERE \"SEA_ID\" = " + boost::lexical_cast<std::string>(ptrInfoKey.get()->pk.search.get()->pk.id/*seaId*/) + 
        " AND \"SRC_ID\" = " + boost::lexical_cast<std::string>(ptrInfoKey.get()->pk.search.get()->pk.source.get()->pk.id/*srcId*/) + /////////
        " AND \"PLG_ID_PLG_ID\" = " + boost::lexical_cast<std::string>(ptrInfoKey.get()->pk.search.get()->pk.source.get()->pk.plugin.id()/*plgId*/) + 
        /*" AND \"INF_VALUE_NUM\" = " + boost::lexical_cast<std::string>(ptrInfoKey.get()->pk.search.get()./*valNum*//*) + */
        " AND \"IVA_AST_AST_ID\" = " + boost::lexical_cast<std::string>(this->vPathElements[3]) + " ORDER BY \"IVA_ID\" DESC LIMIT 50) sr"
        " ) sr_sr"
        ")";

        Wt::Dbo::collection<Wt::Dbo::ptr<InformationValue> > collPtrIva = session->query<Wt::Dbo::ptr<InformationValue> >(queryString);

        if(collPtrIva.size() > 0)
        {
            for (Wt::Dbo::collection<Wt::Dbo::ptr<InformationValue> >::const_iterator i = collPtrIva.begin(); i != collPtrIva.end(); i++)
            {
                res += "{\n\"";
                res += "  \"iva_value\" : \"" + boost::lexical_cast<std::string>(i->id()) + "\"\n\"";
                res += "}\n";
            }
            this->statusCode = 200;
        }
        else
        {
            this->statusCode = 404;
            res = "{\"message\":\"Information value not found\"}";
            return res;
        }

        /////
    }
    catch (Wt::Dbo::Exception const &e)
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}


string InformationResource::getUnitForInformation()
{
    string res = "";
    try
    {
        Wt::Dbo::Transaction transaction(*this->session);

        Wt::Dbo::collection<Wt::Dbo::ptr<InformationSubUnit> > infoSubUnit = session->find<InformationSubUnit>()
                                                                             .where("\"ISU_INU_INU_ID\" = ?").bind(this->vPathElements[2]);

        Wt::Dbo::ptr<InformationUnit> informationUnit = session->find<InformationUnit>()
                                                          .where("\"INU_ID\" = ?").bind(this->vPathElements[2]);
        if (!informationUnit)
        {
            this->statusCode = 404;
            res = "{\"message\":\"Unit not found\"}";
            return res;
        }
        else
        {
            res = "{\n\"";
            res += "  \"inu_name\" : \"" + boost::lexical_cast<std::string > (informationUnit.get()->name.toUTF8()) + "\"\n\"";
            for (Wt::Dbo::collection<Wt::Dbo::ptr<InformationSubUnit> >::const_iterator i = infoSubUnit.begin(); i != infoSubUnit.end(); i++)
            {
                res += "\t{\n\"";
                res += "  \t\"isu_name\" : \"" + boost::lexical_cast<std::string > (i->get()->name.toUTF8()) + "\"\n\"";
                res += "\t}\n";
            }  
            res += "}\n";
            this->statusCode = 200;
        }

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const &e)
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}


string InformationResource::getCriteriaForInformation()
{
    string res = "";
    try
    {
        Wt::Dbo::Transaction transaction(*session);
        Wt::Dbo::collection<Wt::Dbo::ptr<AlertCriteria> > alertCriterias = session->find<AlertCriteria>();
        for (Wt::Dbo::collection<Wt::Dbo::ptr<AlertCriteria> >::const_iterator i = alertCriterias.begin(); i != alertCriterias.end(); ++i)
        {
            res += "{\n\"";
            res += "  \"id\" : \"" + boost::lexical_cast<std::string > (i->id()) + "\"\n\"";
            res += "  \"name\" : \"" + boost::lexical_cast<std::string > (i->get()->name) + "\"\n\"";
            res += "}\n";
        }
        this->statusCode = 200;
        transaction.commit();
    }
    catch (Wt::Dbo::Exception e)
    {
        Wt::log("error") << e.what();
        this->statusCode = 503;
        res = "{\"message\":\"Service Unavailable\"}";
        return res;
    }
    return res;
}

void InformationResource::processGetRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    string responseMsg = "", nextElement = "";
    
    nextElement = getNextElementFromPath();
    if(!nextElement.compare(""))
    {
        this->statusCode = 400;
        responseMsg = "{\n\t\"message\":\"Bad Request\"\n}";
    }
    else
    {
        if(!nextElement.compare("criterias"))
        {
            responseMsg = getCriteriaForInformation();
        }
        else if(!nextElement.compare("units"))
        {
            try
            {
                nextElement = getNextElementFromPath();
                boost::lexical_cast<unsigned int>(nextElement);

                nextElement = getNextElementFromPath();

                if(!nextElement.compare(""))
                {
                    responseMsg = getUnitForInformation();
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
            nextElement = getNextElementFromPath();
                if(!nextElement.compare("assets"))
                {
                    nextElement = getNextElementFromPath();
                    boost::lexical_cast<unsigned int>(nextElement);
                    nextElement = getNextElementFromPath();
                    if(!nextElement.compare(""))
                    {
                    responseMsg = getKeyValueForInformation();
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
    }

    response.setStatus(this->statusCode);
    response.out() << responseMsg;
    return;
}


void InformationResource::processPostRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{   
    return ;
}


void InformationResource::processPutRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}


void InformationResource::processPatchRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    return;
}



void InformationResource::processDeleteRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{    
    return;
}


void InformationResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    // Create Session and Check auth
    PublicApiResource::handleRequest(request, response);

    return;
}

InformationResource::~InformationResource()
{
    beingDeleted();
}