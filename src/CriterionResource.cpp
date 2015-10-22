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

CriterionResource::CriterionResource(Echoes::Dbo::Session& session) : PublicApiResource::PublicApiResource(session)
{
    resourceClassName = "CriterionResource";
    
    functionMap["getCriteriaList"] = boost::bind(&CriterionResource::getCriteriaList, this, _1, _2, _3, _4, _5);
    functionMap["getCriteria"] = boost::bind(&CriterionResource::getCriteria, this, _1, _2, _3, _4, _5);
    functionMap["getAliasForCriteria"] = boost::bind(&CriterionResource::getAliasForCriteria, this, _1, _2, _3, _4, _5);
    functionMap["putAliasForCriteria"] = boost::bind(&CriterionResource::putAliasForCriteria, this, _1, _2, _3, _4, _5);
    
    calls = FillCallsVector();
    
    /*Call structFillTmp;
    
    structFillTmp.method = "GET";
    structFillTmp.path = "";
    structFillTmp.function = boost::bind(&CriterionResource::getCriteriaList, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "GET";
    structFillTmp.path = "/[0-9]+";
    structFillTmp.function = boost::bind(&CriterionResource::getCriterion, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "GET";
    structFillTmp.path = "/[0-9]+/alias";
    structFillTmp.parameters.push_back("media_type_id");
    structFillTmp.parameters.push_back("user_role_id");
    structFillTmp.parameters.push_back("information_id");
    structFillTmp.function = boost::bind(&CriterionResource::getAliasForCriterion, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "GET";
    structFillTmp.path = "/(\\D)*";
    structFillTmp.function = boost::bind(&CriterionResource::Error, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "POST";
    structFillTmp.path = ".*";
    structFillTmp.function = boost::bind(&CriterionResource::Error, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "PUT";
    structFillTmp.path = "";
    structFillTmp.function = boost::bind(&CriterionResource::Error, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "PUT";
    structFillTmp.path = "/[0-9]+/alias";
    structFillTmp.function = boost::bind(&CriterionResource::putAliasForCriterion, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "PUT";
    structFillTmp.path = ".*";
    structFillTmp.function = boost::bind(&CriterionResource::Error, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "DELETE";
    structFillTmp.path = ".*";
    structFillTmp.function = boost::bind(&CriterionResource::Error, this, _1, _2, _3, _4, _5);
    calls.push_back(structFillTmp);*/
}

CriterionResource::~CriterionResource()
{
}

EReturnCode CriterionResource::getCriteriaList(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

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

EReturnCode CriterionResource::getCriteria(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::AlertCriteria> acrPtr = m_session.find<Echoes::Dbo::AlertCriteria>()
                .where(QUOTE(TRIGRAM_ALERT_CRITERIA ID) " = ?").bind(pathElements[1])
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

EReturnCode CriterionResource::getAliasForCriteria(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    if (parameters["media_type_id"] <= 0 || parameters["user_role_id"] <= 0 || parameters["information_id"] <= 0)
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Criterion Resource] media_types or/and user_role or/and information are empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session, true);

            Wt::Dbo::ptr<Echoes::Dbo::UserRole> uroPtr = m_session.find<Echoes::Dbo::UserRole>()
                    .where(QUOTE(TRIGRAM_USER_ROLE ID) " = ?").bind(parameters["user_role_id"])
                    .where(QUOTE(TRIGRAM_USER_ROLE SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = ?").bind(grpId)
                    .where(QUOTE(TRIGRAM_USER_ROLE SEP "DELETE") " IS NULL");
            if (uroPtr)
            {
                Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasInformationCriteria> aicPtr = m_session.find<Echoes::Dbo::AlertMessageAliasInformationCriteria>()
                        .where(QUOTE(TRIGRAM_ALERT_MESSAGE_ALIAS_INFORMATION_CRITERIA SEP "DELETE") " IS NULL")
                        .where(QUOTE(TRIGRAM_USER_ROLE ID SEP TRIGRAM_USER_ROLE ID) " = ?").bind(parameters["user_role_id"])
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE ID SEP TRIGRAM_MEDIA_TYPE ID) " = ?").bind(parameters["media_type_id"])
                        .where(QUOTE(TRIGRAM_INFORMATION ID SEP TRIGRAM_INFORMATION ID) " = ?").bind(parameters["information_id"])
                        .where(QUOTE(TRIGRAM_ALERT_CRITERIA ID SEP TRIGRAM_ALERT_CRITERIA ID) " = ?").bind(pathElements[1]);

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

EReturnCode CriterionResource::processGetRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    string nextElement = "";
    unsigned short indexPathElement = 1;
    vector<string> pathElements;
    map<string, long long> parameters;

    parameters["media_type_id"] = 0;
    parameters["user_role_id"] = 0;
    parameters["information_id"] = 0;
    
    const string sRequest = processRequestParameters(request, pathElements, parameters);

    nextElement = getNextElementFromPath(indexPathElement, pathElements);
    if (nextElement.empty())
    {
        res = getCriteriaList(grpId, responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.empty())
            {
                res = getCriteria(orgId, responseMsg, pathElements);
            }
            else if (!nextElement.compare("alias"))
            {
                res = getAliasForCriteria(orgId, responseMsg, pathElements, sRequest, parameters);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Criterion Resource] bad nextElement";
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

EReturnCode CriterionResource::putAliasForCriteria(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest, std::map<string, long long> parameters)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    long long uroId;
    long long mtyId;
    long long infId;
    Wt::WString value;

    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            uroId = result.get("user_role_id");
            mtyId = result.get("media_type_id");
            infId = result.get("information_id");
            value = result.get("value");
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
        const string err = "[Criterion Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }
    

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session, true);

            Wt::Dbo::ptr<Echoes::Dbo::AlertCriteria> acrPtr =  m_session.find<Echoes::Dbo::AlertCriteria>()
                .where(QUOTE(TRIGRAM_ALERT_CRITERIA ID) " = ?").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_ALERT_CRITERIA SEP "DELETE") " IS NULL");;
            if (acrPtr)
            {
                Wt::Dbo::ptr<Echoes::Dbo::Information> infPtr = m_session.find<Echoes::Dbo::Information>()
                         .where(QUOTE(TRIGRAM_INFORMATION ID) " = ?").bind(infId)
                         .where(QUOTE(TRIGRAM_INFORMATION SEP "DELETE") " IS NULL");
                if (!infPtr)
                {
                    res = EReturnCode::NOT_FOUND;
                    responseMsg = httpCodeToJSON(res, infPtr);
                    return res;
                }

                Wt::Dbo::ptr<Echoes::Dbo::UserRole> uroPtr = m_session.find<Echoes::Dbo::UserRole>()
                        .where(QUOTE(TRIGRAM_USER_ROLE ID) " = ?").bind(uroId)
                        .where(QUOTE(TRIGRAM_USER_ROLE SEP "DELETE") " IS NULL");
                if (!uroPtr)
                {
                    res = EReturnCode::NOT_FOUND;
                    responseMsg = httpCodeToJSON(res, uroPtr);
                    return res;
                }

                Wt::Dbo::ptr<Echoes::Dbo::MediaType> mtyPtr = m_session.find<Echoes::Dbo::MediaType>()
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE ID) " = ?").bind(mtyId)
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE SEP "DELETE") " IS NULL");
                if (!mtyPtr)
                {
                    res = EReturnCode::NOT_FOUND;
                    responseMsg = httpCodeToJSON(res, mtyPtr);
                    return res;
                }

                Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasInformationCriteria> aicPtr = m_session.find<Echoes::Dbo::AlertMessageAliasInformationCriteria>()
                        .where(QUOTE(TRIGRAM_USER_ROLE ID SEP TRIGRAM_USER_ROLE ID) " = ?").bind(uroId)
                        .where(QUOTE(TRIGRAM_INFORMATION ID SEP TRIGRAM_INFORMATION ID) " = ?").bind(infId)
                        .where(QUOTE(TRIGRAM_ALERT_CRITERIA ID SEP TRIGRAM_ALERT_CRITERIA ID) " = ?").bind(pathElements[1])
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE ID SEP TRIGRAM_MEDIA_TYPE ID) " = ?").bind(mtyId);
                if (aicPtr)
                {
                    aicPtr.modify()->alias = value;
                }
                else
                {
                    Echoes::Dbo::AlertMessageAliasInformationCriteria *newAic = new Echoes::Dbo::AlertMessageAliasInformationCriteria();
                    newAic->pk.alertCriteria = acrPtr;
                    newAic->pk.information = infPtr;
                    newAic->pk.userRole = uroPtr;
                    newAic->pk.mediaType = mtyPtr;
                    newAic->alias = value;
                    aicPtr = m_session.add<Echoes::Dbo::AlertMessageAliasInformationCriteria>(newAic);
                }
                res = EReturnCode::OK;
            }
            else
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, acrPtr);
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

EReturnCode CriterionResource::processPutRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
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
        const string err = "[Criterion Resource] bad nextElement";
        responseMsg = httpCodeToJSON(res, err);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);

            if (!nextElement.compare("alias"))
            {
                res = putAliasForCriteria(orgId, responseMsg, pathElements, sRequest);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Criterion Resource] bad nextElement";
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

