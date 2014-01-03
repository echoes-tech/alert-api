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

InformationResource::InformationResource(Echoes::Dbo::Session* session) : PublicApiResource::PublicApiResource(session)
{
}

InformationResource::~InformationResource()
{
}

EReturnCode InformationResource::getInformationsList(const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Echoes::Dbo::SafeTransaction transaction(*m_session);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Information>> infPtrCol = m_session->find<Echoes::Dbo::Information>()
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

EReturnCode InformationResource::getInformation(const std::vector<std::string> &pathElements, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Echoes::Dbo::SafeTransaction transaction(*m_session);

        Wt::Dbo::ptr<Echoes::Dbo::Information> infPtr = m_session->find<Echoes::Dbo::Information>()
                .where(QUOTE(TRIGRAM_INFORMATION ID) " = ?").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_INFORMATION SEP "DELETE") " IS NULL");

        res = serialize(infPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode InformationResource::getAliasForInformation(const std::vector<std::string> &pathElements, map<string, long long> &parameters, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    if (parameters["media_type_id"] <= 0 || parameters["user_role_id"] <= 0)
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Assert Resource] media_types or/and user_role are empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Echoes::Dbo::SafeTransaction transaction(*m_session);

            Wt::Dbo::ptr<Echoes::Dbo::UserRole> uroPtr = m_session->find<Echoes::Dbo::UserRole>()
                    .where(QUOTE(TRIGRAM_USER_ROLE ID) " = ?").bind(parameters["user_role_id"])
                    .where(QUOTE(TRIGRAM_USER_ROLE SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(orgId)
                    .where(QUOTE(TRIGRAM_USER_ROLE SEP "DELETE") " IS NULL");
            if (uroPtr)
            {
                Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasInformation> aaiPtr = m_session->find<Echoes::Dbo::AlertMessageAliasInformation>()
                        .where(QUOTE(TRIGRAM_ALERT_MESSAGE_ALIAS_INFORMATION SEP "DELETE") " IS NULL")
                        .where(QUOTE(TRIGRAM_USER_ROLE ID SEP TRIGRAM_USER_ROLE ID) " = ?").bind(parameters["user_role_id"])
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE ID SEP TRIGRAM_MEDIA_TYPE ID) " = ?").bind(parameters["media_type_id"])
                        .where(QUOTE(TRIGRAM_INFORMATION ID SEP TRIGRAM_INFORMATION ID) " = ?").bind(pathElements[1]);

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

EReturnCode InformationResource::processGetRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    string nextElement = "";
    unsigned short indexPathElement = 1;
    vector<string> pathElements;
    map<string, long long> parameters;

    parameters["media_type_id"] = 0;
    parameters["user_role_id"] = 0;
    
    const string sRequest = processRequestParameters(request, pathElements, parameters);

    nextElement = getNextElementFromPath(indexPathElement, pathElements);
    if (nextElement.empty())
    {
        res = getInformationsList(orgId, responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.empty())
            {
                res = getInformation(pathElements, orgId, responseMsg);
            }
            else if (!nextElement.compare("alias"))
            {
                res = getAliasForInformation(pathElements, parameters, orgId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Information Resource] bad nextElement";
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

EReturnCode InformationResource::postInformation(const string& sRequest, const long long &orgId, string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    Wt::WString name;
    Wt::WString calculate;
    Wt::WString desc;
    bool display;
    long long inuId;

    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            name = result.get("name");
            desc = result.get("desc");
            display = result.get("display");
            inuId = result.get("unit_id");
            if(result.contains("calculate"))
            {
                calculate = result.get("calculate");
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
        const string err = "[Information Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Echoes::Dbo::SafeTransaction transaction(*m_session);

            Wt::Dbo::ptr<Echoes::Dbo::InformationUnit> inuPtr = m_session->find<Echoes::Dbo::InformationUnit>()
                    .where(QUOTE(TRIGRAM_INFORMATION_UNIT ID) " = ?").bind(inuId)
                    .where(QUOTE(TRIGRAM_INFORMATION_UNIT SEP "DELETE") " IS NULL");
            if (inuPtr)
            {
                Echoes::Dbo::Information *newInf = new Echoes::Dbo::Information();
                newInf->name = name;
                newInf->display = display;
                newInf->calculate = calculate;
                newInf->desc = desc;
                newInf->informationUnit = inuPtr;

                Wt::Dbo::ptr<Echoes::Dbo::Information> newInfPtr = m_session->add<Echoes::Dbo::Information>(newInf);
                newInfPtr.flush();

                res = serialize(newInfPtr, responseMsg, EReturnCode::CREATED);
            }
            else
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, inuPtr);
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

EReturnCode InformationResource::processPostRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        res = postInformation(sRequest, orgId, responseMsg);
    }
    else
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Information Resource] bad nextElement";
        responseMsg = httpCodeToJSON(res, err);
    }

    return res;
}

EReturnCode InformationResource::putInformation(const std::vector<std::string> &pathElements, const string &sRequest, const long long &orgId,  string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    Wt::WString name;
    Wt::WString calculate;
    Wt::WString desc;
    bool display;
    bool isDisplayIsPresent = false;
    long long inuId = 0;

    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            if(result.contains("name"))
            {
                name = result.get("name");
            }
            if(result.contains("desc"))
            {
                desc = result.get("desc");
            }
            if(result.contains("display"))
            {
                display = result.get("display");
                isDisplayIsPresent = true;
            }
            if(result.contains("unit_id"))
            {
                inuId = result.get("unit_id");
            }
            if(result.contains("calculate"))
            {
                calculate = result.get("calculate");
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
        const string err = "[Information Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Echoes::Dbo::SafeTransaction transaction(*m_session);

            Wt::Dbo::ptr<Echoes::Dbo::Information> infPtr = m_session->find<Echoes::Dbo::Information>()
                .where(QUOTE(TRIGRAM_INFORMATION ID) " = ?").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_INFORMATION SEP "DELETE") " IS NULL");

            if (infPtr)
            {
                if (!name.empty())
                {
                    infPtr.modify()->name = name;
                }
                if (!desc.empty())
                {
                    infPtr.modify()->desc = desc;
                }
                if (isDisplayIsPresent)
                {
                    infPtr.modify()->display = display;
                }
                if (inuId > 0)
                {
                    Wt::Dbo::ptr<Echoes::Dbo::InformationUnit> inuPtr = m_session->find<Echoes::Dbo::InformationUnit>()
                            .where(QUOTE(TRIGRAM_INFORMATION_UNIT ID) " = ?").bind(inuId)
                            .where(QUOTE(TRIGRAM_INFORMATION_UNIT SEP "DELETE") " IS NULL");
                    if (inuPtr)
                    {
                        infPtr.modify()->informationUnit = inuPtr;
                    }
                    else
                    {
                        Wt::log("debug") << "[Information Resource] Put Information: unit not found";
                    }
                }
                if (!calculate.empty())
                {
                    infPtr.modify()->calculate = calculate;
                }

                res = serialize(infPtr, responseMsg);
            }
            else
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, infPtr);
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

EReturnCode InformationResource::putAliasForInformation(const std::vector<std::string> &pathElements, const string &sRequest, const long long &orgId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    long long uroId;
    long long mtyId;
    Wt::WString value;
    
    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            uroId = result.get("user_role_id");
            mtyId = result.get("media_type_id");
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
        const string err = "[Information Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Echoes::Dbo::SafeTransaction transaction(*m_session);

            Wt::Dbo::ptr<Echoes::Dbo::Information> infPtr =  m_session->find<Echoes::Dbo::Information>()
                .where(QUOTE(TRIGRAM_INFORMATION ID) " = ?").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_INFORMATION SEP "DELETE") " IS NULL");
            if (infPtr)
            {
                Wt::Dbo::ptr<Echoes::Dbo::UserRole> uroPtr = m_session->find<Echoes::Dbo::UserRole>()
                        .where(QUOTE(TRIGRAM_USER_ROLE ID) " = ?").bind(uroId)
                        .where(QUOTE(TRIGRAM_USER_ROLE SEP "DELETE") " IS NULL");
                if (!uroPtr)
                {
                    res = EReturnCode::NOT_FOUND;
                    responseMsg = httpCodeToJSON(res, uroPtr);
                    return res;
                }

                Wt::Dbo::ptr<Echoes::Dbo::MediaType> mtyPtr = m_session->find<Echoes::Dbo::MediaType>()
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE ID) " = ?").bind(mtyId)
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE SEP "DELETE") " IS NULL");
                if (!mtyPtr)
                {
                    res = EReturnCode::NOT_FOUND;
                    responseMsg = httpCodeToJSON(res, mtyPtr);
                    return res;
                }

                Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasInformation> aaiPtr = m_session->find<Echoes::Dbo::AlertMessageAliasInformation>()
                        .where(QUOTE(TRIGRAM_USER_ROLE ID SEP TRIGRAM_USER_ROLE ID) " = ?").bind(uroId)
                        .where(QUOTE(TRIGRAM_INFORMATION ID SEP TRIGRAM_INFORMATION ID) " = ?").bind(pathElements[1])
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE ID SEP TRIGRAM_MEDIA_TYPE ID) " = ?").bind(mtyId);
                if (aaiPtr)
                {
                    aaiPtr.modify()->alias = value;
                }
                else
                {
                    Echoes::Dbo::AlertMessageAliasInformation *newAai = new Echoes::Dbo::AlertMessageAliasInformation();
                    newAai->pk.information = infPtr;
                    newAai->pk.userRole = uroPtr;
                    newAai->pk.mediaType = mtyPtr;
                    newAai->alias = value;
                    aaiPtr = m_session->add<Echoes::Dbo::AlertMessageAliasInformation>(newAai);
                }
                res = EReturnCode::OK;
            }
            else
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, infPtr);
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

EReturnCode InformationResource::processPutRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        const string err = "[Information Resource] bad nextElement";
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
                res = putInformation(pathElements, sRequest, orgId, responseMsg);
            }
            else if (!nextElement.compare("alias"))
            {
                res = putAliasForInformation(pathElements, sRequest, orgId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Information Resource] bad nextElement";
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

EReturnCode InformationResource::deleteInformation(const std::vector<std::string> &pathElements, const long long &orgId, string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try 
    {  
        Echoes::Dbo::SafeTransaction transaction(*m_session);

        Wt::Dbo::ptr<Echoes::Dbo::Information> infPtr = m_session->find<Echoes::Dbo::Information>()
                .where(QUOTE(TRIGRAM_INFORMATION ID) " = ?").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_INFORMATION SEP "DELETE") " IS NULL");

        if(infPtr)
        {
            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::InformationData>> idaPtrCol = m_session->find<Echoes::Dbo::InformationData>()
                    .where(QUOTE(TRIGRAM_INFORMATION_DATA SEP TRIGRAM_INFORMATION SEP TRIGRAM_INFORMATION ID)" = ?").bind(pathElements[1])
                    .where(QUOTE(TRIGRAM_INFORMATION_DATA SEP "DELETE") " IS NULL");

            if (idaPtrCol.size() == 0)
            {
                infPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
                res = EReturnCode::NO_CONTENT;
            }
            else
            {
                res = EReturnCode::CONFLICT;
                responseMsg = httpCodeToJSON(res, infPtr);
            }
        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = httpCodeToJSON(res, infPtr);
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

EReturnCode InformationResource::processDeleteRequest(const Wt::Http::Request &request, const long long &orgId, std::string &responseMsg)
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
        const string err = "[Filter Resource] bad nextElement";
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
                res = deleteInformation(pathElements, orgId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Filter Resource] bad nextElement";
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

