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

using namespace std;

PluginResource::PluginResource() : PublicApiResource::PublicApiResource()
{
    m_parameters["media_type_id"] = 0;
    m_parameters["user_role_id"] = 0;
}

PluginResource::~PluginResource()
{
}

EReturnCode PluginResource::getPluginsList(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Plugin>> plgPtrCol = m_session.find<Echoes::Dbo::Plugin>()
                .where(QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(m_session.user()->organization.id())
                .orderBy(QUOTE(TRIGRAM_PLUGIN ID));

        res = serialize(plgPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode PluginResource::getPlugin(std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::ptr<Echoes::Dbo::Plugin> plgPtr = m_session.find<Echoes::Dbo::Plugin>()
                .where(QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_PLUGIN ID) " = ?").bind(m_pathElements[1])
                .where(QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(m_session.user()->organization.id());

        res = serialize(plgPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode PluginResource::getAliasForPlugin(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    if (m_parameters["media_type_id"] <= 0 || m_parameters["user_role_id"] <= 0)
    {
        res = EReturnCode::BAD_REQUEST;
        const string err = "[Criterion Resource] media_types or/and user_role are empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session);

            Wt::Dbo::ptr<Echoes::Dbo::UserRole> uroPtr = m_session.find<Echoes::Dbo::UserRole>()
                    .where(QUOTE(TRIGRAM_USER_ROLE ID) " = ?").bind(m_parameters["user_role_id"])
                    .where(QUOTE(TRIGRAM_USER_ROLE SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(m_session.user()->organization.id())
                    .where(QUOTE(TRIGRAM_USER_ROLE SEP "DELETE") " IS NULL");
            if (uroPtr)
            {
                Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasPlugin> aapPtr = m_session.find<Echoes::Dbo::AlertMessageAliasPlugin>()
                        .where(QUOTE(TRIGRAM_ALERT_MESSAGE_ALIAS_PLUGIN SEP "DELETE") " IS NULL")
                        .where(QUOTE(TRIGRAM_USER_ROLE ID SEP TRIGRAM_USER_ROLE ID) " = ?").bind(m_parameters["user_role_id"])
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE ID SEP TRIGRAM_MEDIA_TYPE ID) " = ?").bind(m_parameters["media_type_id"])
                        .where(QUOTE(TRIGRAM_PLUGIN ID SEP TRIGRAM_PLUGIN ID) " = ?").bind(m_pathElements[1]);

                res = serialize(aapPtr, responseMsg);
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

EReturnCode PluginResource::getInformationListForPlugin(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try
    {
        Wt::Dbo::Transaction transaction(m_session);
        const string queryStr =
" SELECT inf\n"
"   FROM " QUOTE("T_INFORMATION" SEP TRIGRAM_INFORMATION) " inf\n"
"   WHERE\n"
"     " QUOTE(TRIGRAM_INFORMATION ID) " IN\n"
"       (\n"
"         SELECT " QUOTE(TRIGRAM_INFORMATION_DATA SEP TRIGRAM_INFORMATION SEP TRIGRAM_INFORMATION ID) "\n"
"           FROM " QUOTE("T_INFORMATION_DATA" SEP TRIGRAM_INFORMATION_DATA) "\n"
"           WHERE\n"
"             " QUOTE(TRIGRAM_INFORMATION_DATA SEP TRIGRAM_FILTER SEP TRIGRAM_FILTER ID) " IN\n"
"               (\n"
"                 SELECT " QUOTE(TRIGRAM_FILTER ID) "\n"
"                   FROM " QUOTE("T_FILTER" SEP TRIGRAM_FILTER) "\n"
"                   WHERE\n"
"                     " QUOTE(TRIGRAM_FILTER SEP TRIGRAM_SEARCH SEP TRIGRAM_SEARCH ID) " IN\n"
"                       (\n"
"                         SELECT " QUOTE(TRIGRAM_SEARCH ID) "\n"
"                           FROM " QUOTE("T_SEARCH" SEP TRIGRAM_SEARCH) "\n"
"                           WHERE\n"
"                             " QUOTE(TRIGRAM_SEARCH SEP TRIGRAM_SOURCE SEP TRIGRAM_SOURCE ID) " IN\n"
"                               (\n"
"                                 SELECT " QUOTE(TRIGRAM_SOURCE ID) "\n"
"                                   FROM " QUOTE("T_SOURCE" SEP TRIGRAM_SOURCE) "\n"
"                                   WHERE\n"
"                                     " QUOTE(TRIGRAM_SOURCE ID) " IN\n"
"                                       (\n"
"                                         SELECT " QUOTE("T_SOURCE" SEP TRIGRAM_SOURCE SEP TRIGRAM_SOURCE ID) "\n"
"                                           FROM " QUOTE("TJ" SEP TRIGRAM_PLUGIN SEP TRIGRAM_SOURCE) "\n"
"                                           WHERE\n"
"                                             " QUOTE("T_PLUGIN" SEP TRIGRAM_PLUGIN SEP TRIGRAM_PLUGIN ID) " IN\n"
"                                               (\n"
"                                                 SELECT " QUOTE(TRIGRAM_PLUGIN ID) "\n"
"                                                   FROM " QUOTE("T_PLUGIN" SEP TRIGRAM_PLUGIN) "\n"
"                                                   WHERE\n"
"                                                     " QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = " + boost::lexical_cast<string>(m_session.user()->organization.id()) + "\n"
"                                                     AND " QUOTE(TRIGRAM_PLUGIN ID) " = " + m_pathElements[1] + "\n"
"                                                     AND " QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL\n"
"                                               )\n"
"                                       )\n"
"                                     AND " QUOTE(TRIGRAM_SOURCE SEP "DELETE") " IS NULL\n"
"                               )\n"
"                             AND " QUOTE(TRIGRAM_SEARCH SEP "DELETE") " IS NULL\n"
"                       )\n"
"                     AND " QUOTE(TRIGRAM_FILTER SEP "DELETE") " IS NULL\n"
"               )\n"
"             AND " QUOTE(TRIGRAM_INFORMATION_DATA SEP "DELETE") " IS NULL\n"
"       )\n"
"     AND " QUOTE(TRIGRAM_INFORMATION SEP "DELETE") " IS NULL;";

        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Information>> queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::Information>>(queryStr);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Information>> infPtrCol = queryRes.resultList();

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

void PluginResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = getPluginsList(responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();
            if (nextElement.empty())
            {
                m_statusCode = getPlugin(responseMsg);
            }
            else if (!nextElement.compare("alias"))
            {
                m_statusCode = getAliasForPlugin(responseMsg);
            }
            else if (!nextElement.compare("informations"))
            {
                m_statusCode = getInformationListForPlugin(responseMsg);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                const string err = "[Plugin Resource] bad nextElement";
                responseMsg = httpCodeToJSON(m_statusCode, err);
            }
        }
        catch (boost::bad_lexical_cast const& e)
        {
            m_statusCode = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(m_statusCode, e);
        }
    }

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}

EReturnCode PluginResource::postPlugin(string& responseMsg, const string& sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    Wt::WString name;
    Wt::WString desc;
//    long long preId;

    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            name = result.get("name");
            desc = result.get("desc");
//            preId = result.get("reference_id");
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
        const string err = "[Plugin Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session);

//            Wt::Dbo::ptr<Echoes::Dbo::PluginReference> prePtr = m_session.find<Echoes::Dbo::PluginReference>()
//                    .where(QUOTE(TRIGRAM_PLUGIN_REFERENCE ID) " = ?").bind(preId)
//                    .where(QUOTE(TRIGRAM_PLUGIN_REFERENCE SEP "DELETE") " IS NULL");
//            if (prePtr)
//            {
                Echoes::Dbo::Plugin *newPlg = new Echoes::Dbo::Plugin();
                newPlg->organization = m_session.user()->organization;
//                newPlg->pluginReference = prePtr;
//                newPlg->versionRef = prePtr->version;
                newPlg->versionRef = "1.0";
                newPlg->versionClient = "1.0";
                newPlg->name = name;
                newPlg->desc = desc;
                Wt::Dbo::ptr<Echoes::Dbo::Plugin> newPlgPtr = m_session.add<Echoes::Dbo::Plugin>(newPlg);
                newPlgPtr.flush();

                res = serialize(newPlgPtr, responseMsg, EReturnCode::CREATED);
//            }
//            else
//            {
//                res = EReturnCode::NOT_FOUND;
//                responseMsg = httpCodeToJSON(res, prePtr);
//            }

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

void PluginResource::processPostRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = postPlugin(responseMsg, m_requestData);
    }
    else
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
        const string err = "[Plugin Resource] bad nextElement";
        responseMsg = httpCodeToJSON(m_statusCode, err);
    }

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}

EReturnCode PluginResource::putAliasForPlugin(string &responseMsg, const string &sRequest)
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
            Wt::Dbo::Transaction transaction(m_session);

            Wt::Dbo::ptr<Echoes::Dbo::Plugin> plgPtr =  m_session.find<Echoes::Dbo::Plugin>()
                .where(QUOTE(TRIGRAM_PLUGIN ID) " = ?").bind(m_pathElements[1])
                .where(QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(m_session.user()->organization.id())
                .where(QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL");
            if (plgPtr)
            {
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

                Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasPlugin> aapPtr = m_session.find<Echoes::Dbo::AlertMessageAliasPlugin>()
                        .where(QUOTE(TRIGRAM_USER_ROLE ID SEP TRIGRAM_USER_ROLE ID) " = ?").bind(uroId)
                        .where(QUOTE(TRIGRAM_PLUGIN ID SEP TRIGRAM_PLUGIN ID) " = ?").bind(m_pathElements[1])
                        .where(QUOTE(TRIGRAM_MEDIA_TYPE ID SEP TRIGRAM_MEDIA_TYPE ID) " = ?").bind(mtyId);
                if (aapPtr)
                {
                    aapPtr.modify()->alias = value;
                }
                else
                {
                    Echoes::Dbo::AlertMessageAliasPlugin *newAap = new Echoes::Dbo::AlertMessageAliasPlugin();
                    newAap->pk.plugin = plgPtr;
                    newAap->pk.userRole = uroPtr;
                    newAap->pk.mediaType = mtyPtr;
                    newAap->alias = value;
                    aapPtr = m_session.add<Echoes::Dbo::AlertMessageAliasPlugin>(newAap);
                }
                res = EReturnCode::OK;
            }
            else
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, plgPtr);
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

EReturnCode PluginResource::putPlugin(string &responseMsg, const string &sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    Wt::WString name;
    Wt::WString desc;

    if (!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            if (result.contains("name"))
            {
                name = result.get("name");
            }
            if (result.contains("desc"))
            {
                desc = result.get("desc");
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
        const string err = "[Plugin Resource] sRequest is not empty";
        responseMsg = httpCodeToJSON(res, err);
    }

    if (responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session);

            Wt::Dbo::ptr<Echoes::Dbo::Plugin> plgPtr = m_session.find<Echoes::Dbo::Plugin>()
                .where(QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_PLUGIN ID) " = ?").bind(m_pathElements[1])
                .where(QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(m_session.user()->organization.id());

            if (plgPtr)
            {
                if (!name.empty())
                {
                    plgPtr.modify()->name = name;
                }

                if (!desc.empty())
                {
                    plgPtr.modify()->desc = desc;
                }

                res = serialize(plgPtr, responseMsg);
            }
            else
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, plgPtr);
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

void PluginResource::processPutRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
        const string err = "[Plugin Resource] bad nextElement";
        responseMsg = httpCodeToJSON(m_statusCode, err);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();

            if (nextElement.empty())
            {
                m_statusCode = putPlugin(responseMsg, m_requestData);
            }
            if (!nextElement.compare("alias"))
            {
                m_statusCode = putAliasForPlugin(responseMsg, m_requestData);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                const string err = "[Plugin Resource] bad nextElement";
                responseMsg = httpCodeToJSON(m_statusCode, err);
            }
        }
        catch (boost::bad_lexical_cast const& e)
        {
            m_statusCode = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(m_statusCode, e);
        }
    }

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}

EReturnCode PluginResource::deletePlugin(string& responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try 
    {  
        Wt::Dbo::Transaction transaction(m_session);
           
        Wt::Dbo::ptr<Echoes::Dbo::Plugin> plgPtr = m_session.find<Echoes::Dbo::Plugin>()
                .where(QUOTE(TRIGRAM_PLUGIN SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_PLUGIN ID) " = ?").bind(m_pathElements[1])
                .where(QUOTE(TRIGRAM_PLUGIN SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(m_session.user()->organization.id());

        if(plgPtr)
        {
            Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Source>> srcPtrCol = plgPtr->sources.find().where(QUOTE(TRIGRAM_SOURCE SEP "DELETE") " IS NULL");

            if (srcPtrCol.size() == 0)
            {
                plgPtr.modify()->deleteTag = Wt::WDateTime::currentDateTime();
                res = EReturnCode::NO_CONTENT;
            }
            else
            {
                res = EReturnCode::CONFLICT;
                responseMsg = httpCodeToJSON(res, plgPtr);
            } 
        }
        else
        {
            res = EReturnCode::NOT_FOUND;
            responseMsg = httpCodeToJSON(res, plgPtr);
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

void PluginResource::processDeleteRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if (nextElement.empty())
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
        const string err = "[Plugin Resource] bad nextElement";
        responseMsg = httpCodeToJSON(m_statusCode, err);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();

            if (nextElement.empty())
            {
                m_statusCode = deletePlugin(responseMsg);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                const string err = "[Plugin Resource] bad nextElement";
                responseMsg = httpCodeToJSON(m_statusCode, err);
            }
        }
        catch (boost::bad_lexical_cast const& e)
        {
            m_statusCode = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(m_statusCode, e);
        }
    }

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}

