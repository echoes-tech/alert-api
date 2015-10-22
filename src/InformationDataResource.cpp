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

Wt::Dbo::ptr<Echoes::Dbo::InformationData> InformationDataResource::selectInformationData(const long long &idaId, const long long &grpId, Echoes::Dbo::Session &session)
{
    return selectInformationData(boost::lexical_cast<string>(idaId), boost::lexical_cast<string>(grpId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::InformationData> InformationDataResource::selectInformationData(const string &idaId, const long long &grpId, Echoes::Dbo::Session &session)
{
    return selectInformationData(idaId, boost::lexical_cast<string>(grpId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::InformationData> InformationDataResource::selectInformationData(const string &idaId, const std::string &grpId, Echoes::Dbo::Session &session)
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
"           WHERE " QUOTE(TRIGRAM_ASSET SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = " + grpId +
"             AND " QUOTE(TRIGRAM_ASSET SEP "DELETE") " IS NULL"
"       )"
"     AND " QUOTE(TRIGRAM_INFORMATION_DATA SEP "DELETE") " IS NULL"
"   LIMIT 1";

    Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::InformationData>> queryRes = session.query<Wt::Dbo::ptr<Echoes::Dbo::InformationData>>(queryStr);

    return queryRes.resultValue();
}

EReturnCode InformationDataResource::getInformationDataList(map<string, long long> &parameters, const long long &grpId, string &responseMsg)
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
"           WHERE " QUOTE(TRIGRAM_ASSET SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = " + boost::lexical_cast<string>(grpId) +
"             AND " QUOTE(TRIGRAM_ASSET SEP "DELETE") " IS NULL"
"       )";

        if (parameters["filter_id"] > 0)
        {
            queryStr += "     AND " QUOTE(TRIGRAM_INFORMATION_DATA SEP TRIGRAM_FILTER SEP TRIGRAM_FILTER ID) " = " + boost::lexical_cast<string>(parameters["filter_id"]);
        }

        if (parameters["asset_id"] > 0)
        {
            queryStr += "     AND " QUOTE(TRIGRAM_INFORMATION_DATA SEP TRIGRAM_ASSET SEP TRIGRAM_ASSET ID) " = " + boost::lexical_cast<string>(parameters["asset_id"]);
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

EReturnCode InformationDataResource::getInformationData(const std::vector<std::string> &pathElements, const long long &grpId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::InformationData> idaPtr = selectInformationData(pathElements[1], grpId, m_session);

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

EReturnCode InformationDataResource::processGetRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    string nextElement = "";
    unsigned short indexPathElement = 1;
    vector<string> pathElements;
    map<string, long long> parameters;
    
    parameters["filter_id"] = 0;
    parameters["asset_id"] = 0;

    const string sRequest = processRequestParameters(request, pathElements, parameters);

    nextElement = getNextElementFromPath(indexPathElement, pathElements);
    if (nextElement.empty())
    {
        res = getInformationDataList(parameters, grpId, responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.empty())
            {
                res = getInformationData(pathElements, grpId, responseMsg);
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

EReturnCode InformationDataResource::postInformationData(const string& sRequest, const long long &grpId, string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    
    res = EReturnCode::NOT_IMPLEMENTED;
    return res;
}

EReturnCode InformationDataResource::processPostRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    res = EReturnCode::NOT_IMPLEMENTED;
    return res;
}

EReturnCode InformationDataResource::putInformationData(const std::vector<std::string> &pathElements, const string &sRequest, const long long &grpId,  string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    res = EReturnCode::NOT_IMPLEMENTED;
    return res;
}

EReturnCode InformationDataResource::processPutRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    res = EReturnCode::NOT_IMPLEMENTED;
    return res;
}

EReturnCode InformationDataResource::deleteInformationData(const std::vector<std::string> &pathElements, const long long &grpId,  string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::InformationData> idaPtr = selectInformationData(pathElements[1], grpId, m_session);

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

EReturnCode InformationDataResource::processDeleteRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
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
                res = deleteInformationData(pathElements, grpId, responseMsg);
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

