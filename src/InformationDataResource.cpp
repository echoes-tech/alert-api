/* 
 * API InformationDataResource
 * @author ECHOES Technologies (GDR)
 * @date 24/01/2014
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "InformationDataResource.h"

using namespace std;

InformationDataResource::InformationDataResource(Echoes::Dbo::Session& session) : PublicApiResource::PublicApiResource(session)
{
}

InformationDataResource::~InformationDataResource()
{
}

Wt::Dbo::ptr<Echoes::Dbo::InformationData> InformationDataResource::selectInformationData(const long long &idaId, const long long &orgId, Echoes::Dbo::Session &session)
{
    return selectInformationData(boost::lexical_cast<string>(idaId), boost::lexical_cast<string>(orgId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::InformationData> InformationDataResource::selectInformationData(const string &idaId, const long long &orgId, Echoes::Dbo::Session &session)
{
    return selectInformationData(idaId, boost::lexical_cast<string>(orgId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::InformationData> InformationDataResource::selectInformationData(const string &idaId, const std::string &orgId, Echoes::Dbo::Session &session)
{
    const string queryStr =
" SELECT ida"
"   FROM " QUOTE("T_INFORMATION_DATA_IDA") " ida"
"   WHERE"
"     " QUOTE(TRIGRAM_INFORMATION_DATA ID) " = " + idaId +
"     AND " QUOTE(TRIGRAM_INFORMATION_DATA SEP TRIGRAM_ASSET SEP TRIGRAM_ASSET ID) " IN"
"       ("
"         SELECT " QUOTE(TRIGRAM_ASSET ID)
"           FROM " QUOTE("T_ASSET_AST")
"           WHERE " QUOTE(TRIGRAM_ASSET SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = " + orgId +
"             AND " QUOTE(TRIGRAM_ASSET SEP "DELETE") " IS NULL"
"       )"
"     AND " QUOTE(TRIGRAM_INFORMATION_DATA SEP "DELETE") " IS NULL"
"   LIMIT 1";

    Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::InformationData>> queryRes = session.query<Wt::Dbo::ptr<Echoes::Dbo::InformationData>>(queryStr);

    return queryRes.resultValue();
}

EReturnCode InformationDataResource::getInformationDataList(map<string, long long> &parameters, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);
        string queryStr =
" SELECT ida"
"   FROM " QUOTE("T_INFORMATION_DATA_IDA") " ida"
"   WHERE"
"     " QUOTE(TRIGRAM_INFORMATION_DATA SEP TRIGRAM_ASSET SEP TRIGRAM_ASSET ID) " IN"
"       ("
"         SELECT " QUOTE(TRIGRAM_ASSET ID)
"           FROM " QUOTE("T_ASSET_AST")
"           WHERE " QUOTE(TRIGRAM_ASSET SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = " + boost::lexical_cast<string>(orgId) +
"             AND " QUOTE(TRIGRAM_ASSET SEP "DELETE") " IS NULL"
"       )";

        if (parameters["filter_id"] > 0)
        {
            queryStr += "     AND " QUOTE(TRIGRAM_INFORMATION_DATA SEP TRIGRAM_FILTER SEP TRIGRAM_FILTER ID) " = " + boost::lexical_cast<string>(parameters["filter_id"]);
        }

        queryStr +=
"     AND " QUOTE(TRIGRAM_INFORMATION_DATA SEP "DELETE") " IS NULL"
"   ORDER BY " QUOTE(TRIGRAM_INFORMATION_DATA ID);

        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::InformationData>> queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::InformationData>>(queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationData>> idaPtrCol = queryRes.resultList();

        res = serialize(idaPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode InformationDataResource::getInformationData(const std::vector<std::string> &pathElements, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::InformationData> idaPtr = selectInformationData(pathElements[1], orgId, m_session);

        res = serialize(idaPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode InformationDataResource::processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    string nextElement = "";
    unsigned short indexPathElement = 1;
    vector<string> pathElements;
    map<string, long long> parameters;
    
    parameters["filter_id"] = 0;

    const string sRequest = processRequestParameters(request, pathElements, parameters);

    nextElement = getNextElementFromPath(indexPathElement, pathElements);
    if (nextElement.empty())
    {
        res = getInformationDataList(parameters, orgId, responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.empty())
            {
                res = getInformationData(pathElements, orgId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Information Data Resource] bad nextElement";
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

EReturnCode InformationDataResource::postInformationData(const string& sRequest, const long long &orgId, string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    long long mtyId;
//    long long usrId;
//    Wt::WString value;
//
//    if (!sRequest.empty())
//    {
//        try
//        {
//            Wt::Json::Object result;
//            Wt::Json::parse(sRequest, result);
//
//            mtyId = result.get("type_id");
//            usrId = result.get("user_id");
//            value = result.get("value");
//        }
//        catch (Wt::Json::ParseError const& e)
//        {
//            res = EReturnCode::BAD_REQUEST;
//            responseMsg = httpCodeToJSON(res, e);
//        }
//        catch (Wt::Json::TypeException const& e)
//        {
//            res = EReturnCode::BAD_REQUEST;
//            responseMsg = httpCodeToJSON(res, e);
//        }
//    }
//    else
//    {
//        res = EReturnCode::BAD_REQUEST;
//        const string err = "[Information Data Resource] sRequest is not empty";
//        responseMsg = httpCodeToJSON(res, err);
//    }
//
//    if (responseMsg.empty())
//    {
//        try
//        {
//            Wt::Dbo::Transaction transaction(m_session, true);
//
//            Wt::Dbo::ptr<Echoes::Dbo::MediaType> mtyPtr = m_session.find<Echoes::Dbo::MediaType>()
//                    .where(QUOTE(TRIGRAM_MEDIA_TYPE ID) " = ?").bind(mtyId)
//                    .where(QUOTE(TRIGRAM_MEDIA_TYPE SEP "DELETE") " IS NULL");
//            if (!mtyPtr)
//            {
//                res = EReturnCode::NOT_FOUND;
//                responseMsg = httpCodeToJSON(res, mtyPtr);
//                return res;
//            }
//
//            Wt::Dbo::ptr<Echoes::Dbo::User> usrPtr = m_session.find<Echoes::Dbo::User>()
//                    .where(QUOTE(TRIGRAM_USER ID) " = ?").bind(usrId)
//                    .where(QUOTE(TRIGRAM_USER SEP "DELETE") " IS NULL")
//                    .where(QUOTE(TRIGRAM_USER SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(orgId);
//            if (!usrPtr)
//            {
//                res = EReturnCode::NOT_FOUND;
//                responseMsg = httpCodeToJSON(res, usrPtr);
//                return res;
//            }
//
//            Echoes::Dbo::Media *newMed = new Echoes::Dbo::Media();
//            newMed->user = usrPtr;
//            newMed->mediaType = mtyPtr;
//            newMed->value = value;
//            newMed->token = Wt::WRandom::generateId(25);
//            newMed->isConfirmed = false;
//            newMed->isDefault = false;
//            Wt::Dbo::ptr<Echoes::Dbo::Media> newMedPtr = m_session.add<Echoes::Dbo::Media>(newMed);
//            newMedPtr.flush();
//
//            res = serialize(newMedPtr, responseMsg, EReturnCode::CREATED);
//
//            transaction.commit();
//        }
//        catch (Wt::Dbo::Exception const& e)
//        {
//            res = EReturnCode::SERVICE_UNAVAILABLE;
//            responseMsg = httpCodeToJSON(res, e);
//        }
//    }
    
    res = EReturnCode::NOT_IMPLEMENTED;
    return res;
}

EReturnCode InformationDataResource::processPostRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    string nextElement = "";
//    unsigned short indexPathElement = 1;
//    vector<string> pathElements;
//    map<string, long long> parameters;
//
//    const string sRequest = processRequestParameters(request, pathElements, parameters);
//
//    nextElement = getNextElementFromPath(indexPathElement, pathElements);
//    if (nextElement.empty())
//    {
//        res = postInformationData(sRequest, orgId, responseMsg);
//    }
//    else
//    {
//        res = EReturnCode::BAD_REQUEST;
//        const string err = "[Information Data Resource] bad nextElement";
//        responseMsg = httpCodeToJSON(res, err);
//    }

    res = EReturnCode::NOT_IMPLEMENTED;
    return res;
}

EReturnCode InformationDataResource::putInformationData(const std::vector<std::string> &pathElements, const string &sRequest, const long long &orgId,  string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    Wt::WString token;
//    Wt::WString value;
//    bool isConfirmed;
//    bool isConfirmedIsPresent = false;
//    bool isDefault;
//    bool isDefaultIsPresent = false;
//
//    if (!sRequest.empty())
//    {
//        try
//        {
//            Wt::Json::Object result;
//            Wt::Json::parse(sRequest, result);
//
//            if (result.contains("token"))
//            {
//                token = result.get("token");
//            }
//            if (result.contains("value"))
//            {
//                value = result.get("value");
//            }
//            if (result.contains("is_confirmed"))
//            {
//                isConfirmed = result.get("is_confirmed");
//                isConfirmedIsPresent = true;
//            }
//            if (result.contains("is_default"))
//            {
//                isDefault = result.get("is_default");
//                isDefaultIsPresent = true;
//            }
//        }
//        catch (Wt::Json::ParseError const& e)
//        {
//            res = EReturnCode::BAD_REQUEST;
//            responseMsg = httpCodeToJSON(res, e);
//        }
//        catch (Wt::Json::TypeException const& e)
//        {
//            res = EReturnCode::BAD_REQUEST;
//            responseMsg = httpCodeToJSON(res, e);
//        }
//    }
//    else
//    {
//        res = EReturnCode::BAD_REQUEST;
//        const string err = "[Information Data Resource] sRequest is not empty";
//        responseMsg = httpCodeToJSON(res, err);
//    }
//
//    if (responseMsg.empty())
//    {
//        try
//        {
//            Wt::Dbo::Transaction transaction(m_session, true);
//
//            Wt::Dbo::ptr<Echoes::Dbo::InformationData> medPtr = selectInformationData(pathElements[1], orgId, m_session);
//
//            if (medPtr)
//            {
////                if (!token.empty())
////                {
////                    medPtr.modify()->token = token;
////                }
////
////                if (!value.empty())
////                {
////                    medPtr.modify()->value = value;
////                }
////
////                if (isConfirmedIsPresent)
////                {
////                    medPtr.modify()->isConfirmed = isConfirmed;
////                }
////
////                if (isDefaultIsPresent)
////                {
////                    medPtr.modify()->isDefault = isDefault;
////                }
//
//                res = serialize(medPtr, responseMsg);
//            }
//            else
//            {
//                res = EReturnCode::NOT_FOUND;
//                responseMsg = httpCodeToJSON(res, medPtr);
//            }
//
//            transaction.commit();
//        }
//        catch (Wt::Dbo::Exception const& e)
//        {
//            res = EReturnCode::SERVICE_UNAVAILABLE;
//            responseMsg = httpCodeToJSON(res, e);
//        }
//    }
    res = EReturnCode::NOT_IMPLEMENTED;
    return res;
}

EReturnCode InformationDataResource::processPutRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
//    string nextElement = "";
//    unsigned short indexPathElement = 1;
//    vector<string> pathElements;
//    map<string, long long> parameters;
//
//    const string sRequest = processRequestParameters(request, pathElements, parameters);
//
//    nextElement = getNextElementFromPath(indexPathElement, pathElements);
//    if (nextElement.empty())
//    {
//        res = EReturnCode::BAD_REQUEST;
//        const string err = "[Information Data Resource] bad nextElement";
//        responseMsg = httpCodeToJSON(res, err);
//    }
//    else
//    {
//        try
//        {
//            boost::lexical_cast<unsigned long long>(nextElement);
//
//            nextElement = getNextElementFromPath(indexPathElement, pathElements);
//
//            if (nextElement.empty())
//            {
//                res = putInformationData(pathElements, sRequest, orgId, responseMsg);
//            }
//            else
//            {
//                res = EReturnCode::BAD_REQUEST;
//                const string err = "[Media Resource] bad nextElement";
//                responseMsg = httpCodeToJSON(res, err);
//            }
//        }
//        catch (boost::bad_lexical_cast const& e)
//        {
//            res = EReturnCode::BAD_REQUEST;
//            responseMsg = httpCodeToJSON(res, e);
//        }
//    }

    res = EReturnCode::NOT_IMPLEMENTED;
    return res;
}

EReturnCode InformationDataResource::deleteInformationData(const std::vector<std::string> &pathElements, const long long &orgId,  string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::InformationData> idaPtr = selectInformationData(pathElements[1], orgId, m_session);

        if (idaPtr)
        {
            idaPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
            res = EReturnCode::NO_CONTENT;
        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = httpCodeToJSON(res, idaPtr);
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

EReturnCode InformationDataResource::processDeleteRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        const string err = "[Information Data Resource] bad nextElement";
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
                res = deleteInformationData(pathElements, orgId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Information Data Resource] bad nextElement";
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

