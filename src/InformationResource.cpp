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

InformationResource::InformationResource() : PublicApiResource::PublicApiResource()
{
    m_parameters["media_type"] = 0;
    m_parameters["user_role"] = 0;
}

InformationResource::~InformationResource()
{
}

EReturnCode InformationResource::getInformationsList(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Information>> infPtrCol = m_session.find<Echoes::Dbo::Information>()
                .where(QUOTE(TRIGRAM_INFORMATION SEP "DELETE") " IS NULL")
                .orderBy(QUOTE(TRIGRAM_INFORMATION ID));

        res = serialize(infPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode InformationResource::getInformation(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::ptr<Echoes::Dbo::Information> acrPtr = m_session.find<Echoes::Dbo::Information>()
                .where(QUOTE(TRIGRAM_INFORMATION ID) " = ?").bind(m_pathElements[1])
                .where(QUOTE(TRIGRAM_INFORMATION SEP "DELETE") " IS NULL");

        res = serialize(acrPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode InformationResource::getAliasForInformation(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    if (m_parameters["media_type"] <= 0 || m_parameters["user_role"] <= 0)
    {
        res = EReturnCode::BAD_REQUEST;
        responseMsg = httpCodeToJSON(res, "");
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session);

            Wt::Dbo::ptr<Echoes::Dbo::UserRole> uroPtr = m_session.find<Echoes::Dbo::UserRole>()
                    .where(QUOTE(TRIGRAM_USER_ROLE ID) " = ?").bind(m_parameters["user_role"])
                    .where(QUOTE(TRIGRAM_USER_ROLE SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(m_session.user()->organization.id())
                    .where(QUOTE(TRIGRAM_USER_ROLE SEP "DELETE") " IS NULL");
            if (uroPtr)
            {
                Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasInformation> aaiPtr = m_session.find<Echoes::Dbo::AlertMessageAliasInformation>()
                        .where(QUOTE(TRIGRAM_ALERT_MESSAGE_ALIAS_INFORMATION SEP "DELETE") " IS NULL")
                        .where(QUOTE(TRIGRAM_USER_ROLE ID SEP TRIGRAM_USER_ROLE ID) " = ?").bind(m_parameters["user_role"])
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE ID SEP TRIGRAM_MEDIA_TYPE ID) " = ?").bind(m_parameters["media_type"])
                        .where(QUOTE(TRIGRAM_INFORMATION ID SEP TRIGRAM_INFORMATION ID) " = ?").bind(m_pathElements[1]);

                res = serialize(aaiPtr, responseMsg);
            }
            else
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, uroPtr);
            }

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

void InformationResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = getInformationsList(responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();
            if (nextElement.empty())
            {
                m_statusCode = getInformation(responseMsg);
            }
            else if (!nextElement.compare("alias"))
            {
                m_statusCode = getAliasForInformation(responseMsg);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                responseMsg = httpCodeToJSON(m_statusCode, "");
            }
        }
        catch (boost::bad_lexical_cast const& e)
        {
            m_statusCode = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(m_statusCode, e);
        }
    }

    response.setStatus(this->m_statusCode);
    response.out() << responseMsg;
    return;
}

void InformationResource::processPostRequest(Wt::Http::Response &response)
{
    return;
}

//EReturnCode PluginResource::putAliasForInformation(string &responseMsg, const string &sRequest)
//{
//    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    Wt::WString sRole;
//    Wt::WString sMedia;
//    Wt::WString sValue;
//    if((res = pluginIsAccessible(responseMsg)) == 200)
//    {
//        try
//        {
//            Wt::Json::Object result;                   
//            Wt::Json::parse(sRequest, result);
//
//            //information
//            Wt::Json::Object alias = result.get("alias");
//            sRole = alias.get("role");
//            sMedia = alias.get("media");
//            sValue = alias.get("value");
//
//            try
//            {
//                Wt::Dbo::Transaction transaction(m_session);
//                //FIXME
//                // Information exist?
////                Wt::Dbo::ptr<Echoes::Dbo::Information> infPtr = m_session.find<Echoes::Dbo::Information>()
////                        .where("\"PLG_ID_PLG_ID\" = ?").bind(vPathElements[1])
////                        .where("\"SRC_ID\" = ?").bind(vPathElements[3])
////                        .where("\"SEA_ID\" = ?").bind(vPathElements[5])
////                        .where("\"INF_VALUE_NUM\" = ?").bind(vPathElements[7])
////                        .where("\"INU_ID_INU_ID\" = ?").bind(vPathElements[9])
////                        ;
////                if(infPtr)
////                {
////                    //Relier une unité à l'info
////                    // unit exist?
////                    Wt::Dbo::ptr<Echoes::Dbo::UserRole> ptrRole = _session.find<Echoes::Dbo::UserRole>()
////                        .where("\"URO_ID\" = ?").bind(sRole)
////                        .where("\"URO_DELETE\" IS NULL");
////                
////                    if (!ptrRole)
////                    {
////                        res = EReturnCode::NOT_FOUND;
////                        responseMsg = "{\n\t\"message\":\"Role not found\"\n}";
////                        return res;
////                    }
////
////                    Wt::Dbo::ptr<Echoes::Dbo::MediaType> ptrMedia = _session.find<Echoes::Dbo::MediaType>()
////                            .where("\"MED_ID\" = ?").bind(sMedia)
////                            .where("\"MED_DELETE\" IS NULL");
////
////                    if (!ptrMedia)
////                    {
////                        res = EReturnCode::NOT_FOUND;
////                        responseMsg = "{\n\t\"message\":\"Media not found\"\n}";
////                        return res;
////                    }
////
////
////                    Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasInformation> ptrInformationAlias = _session.find<AlertMessageAliasInformation>()
////                            .where("\"URO_ID_URO_ID\" = ?").bind(sRole)
////                            .where("\"SEA_ID\" = ?").bind(infPtr->pk.search->pk.id)
////                            .where("\"SRC_ID\" = ?").bind(infPtr->pk.search->pk.source->pk.id)
////                            .where("\"PLG_ID_PLG_ID\" = ?").bind(infPtr->pk.search->pk.source->pk.plugin.id())
////                            .where("\"INF_VALUE_NUM\" = ?").bind(infPtr->pk.subSearchNumber)
////                            .where("\"INU_ID_INU_ID\" = ?").bind(infPtr->pk.unit.id())
////                            .where("\"MED_ID_MED_ID\" = ?").bind(sMedia);
////                    if (ptrInformationAlias) 
////                    {
////                        ptrInformationAlias.modify()->alias = sValue;
////                    }
////                    else
////                    {
////                        Echoes::Dbo::AlertMessageAliasInformation *newInformationAlias = new Echoes::Dbo::AlertMessageAliasInformation();
////                        newInformationAlias->pk.information = infPtr;
////                        newInformationAlias->pk.userRole = ptrRole;
////                        newInformationAlias->pk.media = ptrMedia;
////                        newInformationAlias->alias = sValue;
////                        ptrInformationAlias = _session.add<Echoes::Dbo::AlertMessageAliasInformation>(newInformationAlias);
////                    }
////                    res = EReturnCode::OK;
////                }
////                else
////                {
////                    res = EReturnCode::NOT_FOUND;
////                    responseMsg = "{\"message\":\"Information not found\"}";
////                }
//
//                transaction.commit();
//            }
//            catch (Wt::Dbo::Exception const& e) 
//            {
//                Wt::log("error") << e.what();
//                res = EReturnCode::SERVICE_UNAVAILABLE;
//                responseMsg = "{\"message\":\"Service Unavailable\"}";
//            }
//        }
//        catch (Wt::Json::ParseError const& e)
//        {
//            res = EReturnCode::BAD_REQUEST;
//            responseMsg = "{\"message\":\"Problems parsing JSON\"}";
//            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON:" << sRequest;
//        }
//        catch (Wt::Json::TypeException const& e)
//        {
//            res = EReturnCode::BAD_REQUEST;
//            responseMsg = "{\"message\":\"Problems parsing JSON.\"}";
//            Wt::log("warning") << "[Plugin Ressource] Problems parsing JSON.:" << sRequest;
//        }   
//    }
//    return res; 
//}

void InformationResource::processPutRequest(Wt::Http::Response &response)
{
    return;
}

void InformationResource::processDeleteRequest(Wt::Http::Response &response)
{
    return;
}

