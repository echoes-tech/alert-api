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

MediaResource::MediaResource(Echoes::Dbo::Session* session) : PublicApiResource::PublicApiResource(session)
{
}

MediaResource::~MediaResource()
{
}

Wt::Dbo::ptr<Echoes::Dbo::Media> MediaResource::selectMedia(const long long &medId, const long long &orgId, Echoes::Dbo::Session &session)
{
    return selectMedia(boost::lexical_cast<string>(medId), boost::lexical_cast<string>(orgId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::Media> MediaResource::selectMedia(const string &medId, const long long &orgId, Echoes::Dbo::Session &session)
{
    return selectMedia(medId, boost::lexical_cast<string>(orgId), session);
}

Wt::Dbo::ptr<Echoes::Dbo::Media> MediaResource::selectMedia(const string &medId, const std::string &orgId, Echoes::Dbo::Session &session)
{
    const string queryStr =
" SELECT med"
"   FROM " QUOTE("T_MEDIA_MED") " med"
"   WHERE"
"     " QUOTE(TRIGRAM_MEDIA ID) " = " + medId +
"     AND " QUOTE(TRIGRAM_MEDIA SEP TRIGRAM_USER SEP TRIGRAM_USER ID) " IN"
"       ("
"         SELECT " QUOTE(TRIGRAM_USER ID)
"           FROM " QUOTE("T_USER_USR")
"           WHERE " QUOTE(TRIGRAM_USER SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = " + orgId +
"             AND " QUOTE(TRIGRAM_USER SEP "DELETE") " IS NULL"
"       )"
"     AND " QUOTE(TRIGRAM_MEDIA SEP "DELETE") " IS NULL"
"   LIMIT 1";

    Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Media>> queryRes = session.query<Wt::Dbo::ptr<Echoes::Dbo::Media>>(queryStr);

    return queryRes.resultValue();
}

EReturnCode MediaResource::getMediasList(map<string, long long> &parameters, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Echoes::Dbo::SafeTransaction transaction(*m_session);
        string queryStr =
" SELECT med"
"   FROM " QUOTE("T_MEDIA_MED") " med"
"   WHERE"
"     " QUOTE(TRIGRAM_MEDIA SEP TRIGRAM_USER SEP TRIGRAM_USER ID) " IN"
"       ("
"         SELECT " QUOTE(TRIGRAM_USER ID)
"           FROM " QUOTE("T_USER_USR")
"           WHERE " QUOTE(TRIGRAM_USER SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = " + boost::lexical_cast<string>(orgId) +
"             AND " QUOTE(TRIGRAM_USER SEP "DELETE") " IS NULL"
"       )";

        if (parameters["type_id"] > 0)
        {
            queryStr +=
"     AND " QUOTE(TRIGRAM_MEDIA SEP TRIGRAM_MEDIA_TYPE SEP TRIGRAM_MEDIA_TYPE ID) " = " + boost::lexical_cast<string>(parameters["type_id"]);
        }

        queryStr +=
"     AND " QUOTE(TRIGRAM_MEDIA SEP "DELETE") " IS NULL"
"   ORDER BY " QUOTE(TRIGRAM_MEDIA ID);

        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Media>> queryRes = m_session->query<Wt::Dbo::ptr<Echoes::Dbo::Media>>(queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Media>> medPtrCol = queryRes.resultList();

        res = serialize(medPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode MediaResource::getMedia(const std::vector<std::string> &pathElements, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Echoes::Dbo::SafeTransaction transaction(*m_session);

        Wt::Dbo::ptr<Echoes::Dbo::Media> medPtr = selectMedia(pathElements[1], orgId, *m_session);

        res = serialize(medPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode MediaResource::processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    string nextElement = "";
    unsigned short indexPathElement = 1;
    vector<string> pathElements;
    map<string, long long> parameters;
    
    parameters["type_id"] = 0;

    const string sRequest = processRequestParameters(request, pathElements, parameters);

    nextElement = getNextElementFromPath(indexPathElement, pathElements);
    if (nextElement.empty())
    {
        res = getMediasList(parameters, orgId, responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.empty())
            {
                res = getMedia(pathElements, orgId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Media Resource] bad nextElement";
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

EReturnCode MediaResource::postMedia(const string& sRequest, const long long &orgId, string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    long long mtyId;
    Wt::WString value;

    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            mtyId = result.get("type_id");
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
        const string err = "[Media Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Echoes::Dbo::SafeTransaction transaction(*m_session);

            Wt::Dbo::ptr<Echoes::Dbo::MediaType> mtyPtr = m_session->find<Echoes::Dbo::MediaType>()
                    .where(QUOTE(TRIGRAM_MEDIA_TYPE ID) " = ?").bind(mtyId)
                    .where(QUOTE(TRIGRAM_MEDIA_TYPE SEP "DELETE") " IS NULL");
            if (mtyPtr)
            {
                Echoes::Dbo::Media *newMed = new Echoes::Dbo::Media();
                newMed->user = m_session->user();
                newMed->mediaType = mtyPtr;
                newMed->value = value;
                newMed->token = Wt::WRandom::generateId(25);
                newMed->isConfirmed = false;
                newMed->isDefault = false;
                Wt::Dbo::ptr<Echoes::Dbo::Media> newMedPtr = m_session->add<Echoes::Dbo::Media>(newMed);
                newMedPtr.flush();

                res = serialize(newMedPtr, responseMsg, EReturnCode::CREATED);

                transaction.commit();
            }
            else
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, mtyPtr);
            }
        }
        catch (Wt::Dbo::Exception const& e)
        {
            res = EReturnCode::SERVICE_UNAVAILABLE;
            responseMsg = httpCodeToJSON(res, e);
        }
    }

    return res;
}

EReturnCode MediaResource::processPostRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        res = postMedia(sRequest, orgId, responseMsg);
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Media Resource] bad nextElement";
        responseMsg = httpCodeToJSON(res, err);
    }

    return res;
}

EReturnCode MediaResource::putMedia(const std::vector<std::string> &pathElements, const string &sRequest, const long long &orgId,  string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    Wt::WString token;
    Wt::WString value;
    bool isConfirmed;
    bool isConfirmedIsPresent = false;
    bool isDefault;
    bool isDefaultIsPresent = false;

    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            if (result.contains("token"))
            {
                token = result.get("token");
            }
            if (result.contains("value"))
            {
                value = result.get("value");
            }
            if (result.contains("is_confirmed"))
            {
                isConfirmed = result.get("is_confirmed");
                isConfirmedIsPresent = true;
            }
            if (result.contains("is_default"))
            {
                isDefault = result.get("is_default");
                isDefaultIsPresent = true;
            }
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
        const string err = "[Media Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Echoes::Dbo::SafeTransaction transaction(*m_session);

            Wt::Dbo::ptr<Echoes::Dbo::Media> medPtr = selectMedia(pathElements[1], orgId, *m_session);

            if (medPtr)
            {
                if (!token.empty())
                {
                    medPtr.modify()->token = token;
                }

                if (!value.empty())
                {
                    medPtr.modify()->value = value;
                }

                if (isConfirmedIsPresent)
                {
                    medPtr.modify()->isConfirmed = isConfirmed;
                }

                if (isDefaultIsPresent)
                {
                    medPtr.modify()->isDefault = isDefault;
                }

                res = serialize(medPtr, responseMsg);
            }
            else
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, medPtr);
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

EReturnCode MediaResource::processPutRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        const string err = "[Media Resource] bad nextElement";
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
                res = putMedia(pathElements, sRequest, orgId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Media Resource] bad nextElement";
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

EReturnCode MediaResource::deleteMedia(const std::vector<std::string> &pathElements, const long long &orgId,  string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Echoes::Dbo::SafeTransaction transaction(*m_session);

        Wt::Dbo::ptr<Echoes::Dbo::Media> medPtr = selectMedia(pathElements[1], orgId, *m_session);

        if (medPtr)
        {
            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::AlertMediaSpecialization>> amsPtrCol = m_session->find<Echoes::Dbo::AlertMediaSpecialization>()
                    .where(QUOTE(TRIGRAM_ALERT_MEDIA_SPECIALIZATION SEP TRIGRAM_MEDIA SEP TRIGRAM_MEDIA ID)" = ?").bind(pathElements[1])
                    .where(QUOTE(TRIGRAM_ALERT_MEDIA_SPECIALIZATION SEP "DELETE") " IS NULL");

            if (amsPtrCol.size() == 0)
            {
                medPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
                res = EReturnCode::NO_CONTENT;
            }
            else
            {
                res = EReturnCode::CONFLICT;
                responseMsg = httpCodeToJSON(res, medPtr);
            } 
        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = httpCodeToJSON(res, medPtr);
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

EReturnCode MediaResource::processDeleteRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        const string err = "[Media Resource] bad nextElement";
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
                res = deleteMedia(pathElements, orgId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Media Resource] bad nextElement";
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

