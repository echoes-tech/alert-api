/* 
 * API CriterionResource
 * @author ECHOES Technologies (GDR)
 * @date 21/02/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "CriterionResource.h"

using namespace std;

CriterionResource::CriterionResource() : PublicApiResource::PublicApiResource()
{
    m_parameters["media_type"] = 0;
    m_parameters["user_role"] = 0;
    m_parameters["information"] = 0;
}

CriterionResource::~CriterionResource()
{
}

EReturnCode CriterionResource::getCriteriaList(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertCriteria>> acrPtrCol = m_session.find<Echoes::Dbo::AlertCriteria>()
                .where(QUOTE(TRIGRAM_ALERT_CRITERIA SEP "DELETE") " IS NULL")
                .orderBy(QUOTE(TRIGRAM_ALERT_CRITERIA ID));

        res = serialize(acrPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode CriterionResource::getCriterion(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::ptr<Echoes::Dbo::AlertCriteria> acrPtr = m_session.find<Echoes::Dbo::AlertCriteria>()
                .where(QUOTE(TRIGRAM_ALERT_CRITERIA ID) " = ?").bind(m_pathElements[1])
                .where(QUOTE(TRIGRAM_ALERT_CRITERIA SEP "DELETE") " IS NULL");

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

EReturnCode CriterionResource::getAliasForCriterion(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    if (m_parameters["media_type"] <= 0 || m_parameters["user_role"] <= 0 || m_parameters["information"] <= 0)
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
                Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasInformationCriteria> aicPtr = m_session.find<Echoes::Dbo::AlertMessageAliasInformationCriteria>()
                        .where(QUOTE(TRIGRAM_ALERT_MESSAGE_ALIAS_INFORMATION_CRITERIA SEP "DELETE") " IS NULL")
                        .where(QUOTE(TRIGRAM_USER_ROLE ID SEP TRIGRAM_USER_ROLE ID) " = ?").bind(m_parameters["user_role"])
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE ID SEP TRIGRAM_MEDIA_TYPE ID) " = ?").bind(m_parameters["media_type"])
                        .where(QUOTE(TRIGRAM_INFORMATION ID SEP TRIGRAM_INFORMATION ID) " = ?").bind(m_parameters["information"])
                        .where(QUOTE(TRIGRAM_ALERT_CRITERIA ID SEP TRIGRAM_ALERT_CRITERIA ID) " = ?").bind(m_pathElements[1]);

                res = serialize(aicPtr, responseMsg);
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

void CriterionResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = getCriteriaList(responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();
            if (nextElement.empty())
            {
                m_statusCode = getCriterion(responseMsg);
            }
            else if (!nextElement.compare("alias"))
            {
                m_statusCode = getAliasForCriterion(responseMsg);
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

//EReturnCode PluginResource::putAliasForCriterion(string &responseMsg, const string &sRequest)
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
//
//                // Information exist?
//                Wt::Dbo::ptr<Echoes::Dbo::Information> infPtr = m_session.find<Echoes::Dbo::Information>()
//                        .where("\"PLG_ID_PLG_ID\" = ?").bind(m_pathElements[1])
//                        .where("\"SRC_ID\" = ?").bind(m_pathElements[3])
//                        .where("\"SEA_ID\" = ?").bind(m_pathElements[5])
//                        .where("\"INF_VALUE_NUM\" = ?").bind(m_pathElements[7])
//                        .where("\"INU_ID_INU_ID\" = ?").bind(m_pathElements[9])
//                        ;
//                if(infPtr)
//                {
//                    //Relier une unité à l'info
//                    // unit exist?
//                    Wt::Dbo::ptr<Echoes::Dbo::UserRole> ptrRole = m_session.find<Echoes::Dbo::UserRole>()
//                        .where("\"URO_ID\" = ?").bind(sRole)
//                        .where("\"URO_DELETE\" IS NULL");
//                
//                    if (!ptrRole)
//                    {
//                        res = EReturnCode::NOT_FOUND;
//                        responseMsg = "{\n\t\"message\":\"Role not found\"\n}";
//                        return res;
//                    }
//
//                    Wt::Dbo::ptr<Echoes::Dbo::MediaType> ptrMedia = m_session.find<Echoes::Dbo::MediaType>()
//                            .where("\"MED_ID\" = ?").bind(sMedia)
//                            .where("\"MED_DELETE\" IS NULL");
//
//                    if (!ptrMedia)
//                    {
//                        res = EReturnCode::NOT_FOUND;
//                        responseMsg = "{\n\t\"message\":\"Media not found\"\n}";
//                        return res;
//                    }
//                    
//                    Wt::Dbo::ptr<Echoes::Dbo::AlertCriteria> ptrCriterion = m_session.find<Echoes::Dbo::AlertCriteria>()
//                            .where("\"ACR_ID\" = ?").bind(m_pathElements[11])
//                            .where("\"ACR_DELETE\" IS NULL");
//
//                    if (!ptrCriterion)
//                    {
//                        res = EReturnCode::NOT_FOUND;
//                        responseMsg = "{\n\t\"message\":\"Criterion not found\"\n}";
//                        return res;
//                    }
//
//                    //FIXME
////                    Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasInformationCriteria> ptrInformationCriteriaAlias = _session.find<Echoes::Dbo::AlertMessageAliasInformationCriteria>()
////                            .where("\"URO_ID_URO_ID\" = ?").bind(sRole)
////                            .where("\"SEA_ID\" = ?").bind(infPtr->pk.search->pk.id)
////                            .where("\"SRC_ID\" = ?").bind(infPtr->pk.search->pk.source->pk.id)
////                            .where("\"PLG_ID_PLG_ID\" = ?").bind(infPtr->pk.search->pk.source->pk.plugin.id())
////                            .where("\"INF_VALUE_NUM\" = ?").bind(infPtr->pk.subSearchNumber)
////                            .where("\"INU_ID_INU_ID\" = ?").bind(infPtr->pk.unit.id())
////                            .where("\"MED_ID_MED_ID\" = ?").bind(sMedia)
////                            .where("\"ACR_ID_ACR_ID\" = ?").bind(ptrCriterion.id());
////                    if (ptrInformationCriteriaAlias) 
////                    {
////                        ptrInformationCriteriaAlias.modify()->alias = sValue;
////                    }
////                    else
////                    {
////                        Echoes::Dbo::AlertMessageAliasInformationCriteria *newInformationCriteriaAlias = new Echoes::Dbo::AlertMessageAliasInformationCriteria();
////                        newInformationCriteriaAlias->pk.information = infPtr;
////                        newInformationCriteriaAlias->pk.userRole = ptrRole;
////                        newInformationCriteriaAlias->pk.media = ptrMedia;
////                        newInformationCriteriaAlias->pk.alertCriteria = ptrCriterion;
////                        newInformationCriteriaAlias->alias = sValue;
////                        ptrInformationCriteriaAlias = _session.add<Echoes::Dbo::AlertMessageAliasInformationCriteria>(newInformationCriteriaAlias);
////                    }
//                    res = EReturnCode::OK;
//                }
//                else
//                {
//                    res = EReturnCode::NOT_FOUND;
//                    responseMsg = "{\"message\":\"Information not found\"}";
//                }
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

