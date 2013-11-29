 /* 
 * API Asset Resource
 * @author ECHOES Technologies (FPO)
 * @date 02/11/2012
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2012-2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "AssetResource.h"

using namespace std;

AssetResource::AssetResource() : PublicApiResource::PublicApiResource()
{
}

AssetResource::~AssetResource()
{
}

EReturnCode AssetResource::getAssetsList(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Asset>> astPtrCol = m_session.find<Echoes::Dbo::Asset>()
                .where(QUOTE(TRIGRAM_ASSET SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_ASSET SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(m_session.user()->organization.id())
                .orderBy(QUOTE(TRIGRAM_ASSET ID));

        res = serialize(astPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode AssetResource::getAsset(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::ptr<Echoes::Dbo::Asset> astPtr = m_session.find<Echoes::Dbo::Asset>()
                .where(QUOTE(TRIGRAM_ASSET ID) " = ?").bind(m_pathElements[1])
                .where(QUOTE(TRIGRAM_ASSET SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_ASSET SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(m_session.user()->organization.id());

        res = serialize(astPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode AssetResource::getProbesListForAsset(string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    try 
    {
        Wt::Dbo::Transaction transaction(m_session);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Probe>> prbPtrCol = m_session.find<Echoes::Dbo::Probe>()
                .where(QUOTE(TRIGRAM_PROBE SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_PROBE SEP TRIGRAM_ASSET SEP TRIGRAM_ASSET ID) " = ?").bind(m_pathElements[1])
                .orderBy(QUOTE(TRIGRAM_PROBE ID));

        res = serialize(prbPtrCol, responseMsg);

        transaction.commit();
    } 
    catch (Wt::Dbo::Exception const& e) 
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    
    return res;
}

EReturnCode AssetResource::getAliasForAsset(std::string  &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;

    if (m_role.empty() || m_media.empty())
    {
        res = EReturnCode::BAD_REQUEST;
        responseMsg = httpCodeToJSON(res, "");
    }

    if(responseMsg.empty())
    {
        try 
        {
            Wt::Dbo::Transaction transaction(m_session);

            Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasAsset> aaaPtr = m_session.find<Echoes::Dbo::AlertMessageAliasAsset>()
                    .where(QUOTE(TRIGRAM_ALERT_MESSAGE_ALIAS_ASSET SEP "DELETE") " IS NULL")
                    .where(QUOTE(TRIGRAM_USER_ROLE ID SEP TRIGRAM_USER_ROLE ID) " = ?").bind(m_role)
                    .where(QUOTE(TRIGRAM_MEDIA ID SEP TRIGRAM_MEDIA ID) " = ?").bind(m_media)
                    .where(QUOTE(TRIGRAM_ASSET ID SEP TRIGRAM_ASSET ID) " = ?").bind(m_pathElements[1]);

            res = serialize(aaaPtr, responseMsg);

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

void AssetResource::processGetRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if(nextElement.empty())
    {
        m_statusCode = getAssetsList(responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();
            if(nextElement.empty())
            {
                m_statusCode = getAsset(responseMsg);
            }
            else if(!nextElement.compare("probes"))
            {
                m_statusCode = getProbesListForAsset(responseMsg);
            }
            else if(!nextElement.compare("alias"))
            {
                m_statusCode = getAliasForAsset(responseMsg);
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

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}

EReturnCode AssetResource::postAsset(string &responseMsg, const string &sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    Wt::WString name;
    Wt::WString architecture;
    Wt::WString distribution;
    Wt::WString release;

    if(!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            name = result.get("name");
            if(result.contains("distribution"))
            {
                distribution = result.get("distribution");
            }
            if(result.contains("architecture"))
            {
                architecture = result.get("architecture");
            }
            if(result.contains("release"))
            {
                release = result.get("release");
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
        responseMsg = httpCodeToJSON(res, "");
    }

    if(responseMsg.empty())
    {
        try
        {
            Wt::Dbo::Transaction transaction(m_session);

            Echoes::Dbo::Asset *newAst = new Echoes::Dbo::Asset();
            newAst->name = name;
            newAst->assetIsHost = false;
            newAst->organization = m_session.user()->organization;

            if(!architecture.empty())
            {
                Wt::Dbo::ptr<Echoes::Dbo::AssetArchitecture> asaPtr = m_session.find<Echoes::Dbo::AssetArchitecture>()
                        .where(QUOTE(TRIGRAM_ASSET_ARCHITECTURE SEP "NAME") " = ?").bind(architecture)
                        .limit(1);
                if (!asaPtr) 
                {
                    Echoes::Dbo::AssetArchitecture *newAsa = new Echoes::Dbo::AssetArchitecture();
                    newAsa->name = architecture;
                    asaPtr = m_session.add<Echoes::Dbo::AssetArchitecture>(newAsa);
                    asaPtr.flush();
                }
                newAst->assetArchitecture = asaPtr;
            }

            if(!distribution.empty())
            {
                Wt::Dbo::ptr<Echoes::Dbo::AssetDistribution> asdPtr = m_session.find<Echoes::Dbo::AssetDistribution>()
                        .where(QUOTE(TRIGRAM_ASSET_DISTRIBUTION SEP "NAME") " = ?").bind(distribution)
                        .limit(1);
                if (!asdPtr) 
                {
                    Echoes::Dbo::AssetDistribution *newAsd = new Echoes::Dbo::AssetDistribution();
                    newAsd->name = distribution;
                    asdPtr = m_session.add<Echoes::Dbo::AssetDistribution>(newAsd);
                    asdPtr.flush();
                }
                newAst->assetDistribution = asdPtr;
            }                

            if(!distribution.empty())
            {
                Wt::Dbo::ptr<Echoes::Dbo::AssetRelease> asrPtr = m_session.find<Echoes::Dbo::AssetRelease>()
                        .where(QUOTE(TRIGRAM_ASSET_RELEASE SEP "NAME") " = ?").bind(release)
                        .limit(1);
                if (!asrPtr)
                {
                    Echoes::Dbo::AssetRelease *newAsr = new Echoes::Dbo::AssetRelease();
                    newAsr->name = release;
                    asrPtr = m_session.add<Echoes::Dbo::AssetRelease>(newAsr);
                    asrPtr.flush();
                }
                newAst->assetRelease = asrPtr;
            }

            Wt::Dbo::ptr<Echoes::Dbo::Asset> newAstPtr = m_session.add<Echoes::Dbo::Asset>(newAst);
            newAstPtr.flush();

            res = serialize(newAstPtr, responseMsg, EReturnCode::CREATED);

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

void AssetResource::processPostRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if(nextElement.empty())
    {
        m_statusCode = postAsset(responseMsg, m_requestData);
    }
    else
    {   
        m_statusCode = EReturnCode::BAD_REQUEST;
        responseMsg = httpCodeToJSON(m_statusCode, "");
    }

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}

EReturnCode AssetResource::putAsset(string &responseMsg, const string &sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    Wt::WString name;
    Wt::WString architecture;
    Wt::WString distribution;
    Wt::WString release;

    if(!sRequest.empty())
    {
        try
        {
            Wt::Json::Object result;
            Wt::Json::parse(sRequest, result);

            if(result.contains("name"))
            {
                name = result.get("name");
            }
            if(result.contains("distribution"))
            {
                distribution = result.get("distribution");
            }
            if(result.contains("architecture"))
            {
                architecture = result.get("architecture");
            }
            if(result.contains("release"))
            {
                release = result.get("release");
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
        responseMsg = httpCodeToJSON(res, "");
    }

    if(responseMsg.empty())
    {
        try 
        {
            Wt::Dbo::Transaction transaction(m_session);

            Wt::Dbo::ptr<Echoes::Dbo::Asset> astPtr = m_session.find<Echoes::Dbo::Asset>()
                    .where(QUOTE(TRIGRAM_ASSET ID) " = ?").bind(m_pathElements[1])
                    .where(QUOTE(TRIGRAM_ASSET SEP "DELETE") " IS NULL")
                    .where(QUOTE(TRIGRAM_ASSET SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(m_session.user()->organization.id());

            if (astPtr) 
            {
                if(!name.empty())
                {
                    astPtr.modify()->name = name;
                }

                if(!architecture.empty())
                {
                    Wt::Dbo::ptr<Echoes::Dbo::AssetArchitecture> asaPtr = m_session.find<Echoes::Dbo::AssetArchitecture>()
                            .where(QUOTE(TRIGRAM_ASSET_ARCHITECTURE SEP "NAME") " = ?").bind(architecture)
                            .limit(1);
                    if (!asaPtr) 
                    {
                        Echoes::Dbo::AssetArchitecture *newAsa = new Echoes::Dbo::AssetArchitecture();
                        newAsa->name = architecture;
                        asaPtr = m_session.add<Echoes::Dbo::AssetArchitecture>(newAsa);
                        asaPtr.flush();
                    }
                    astPtr.modify()->assetArchitecture = asaPtr;
                }

                if(!distribution.empty())
                {
                    Wt::Dbo::ptr<Echoes::Dbo::AssetDistribution> asdPtr = m_session.find<Echoes::Dbo::AssetDistribution>()
                            .where(QUOTE(TRIGRAM_ASSET_DISTRIBUTION SEP "NAME") " = ?").bind(distribution)
                            .limit(1);
                    if (!asdPtr) 
                    {
                        Echoes::Dbo::AssetDistribution *newAsd = new Echoes::Dbo::AssetDistribution();
                        newAsd->name = distribution;
                        asdPtr = m_session.add<Echoes::Dbo::AssetDistribution>(newAsd);
                        asdPtr.flush();
                    }
                    astPtr.modify()->assetDistribution = asdPtr;
                }                

                if(!distribution.empty())
                {
                    Wt::Dbo::ptr<Echoes::Dbo::AssetRelease> asrPtr = m_session.find<Echoes::Dbo::AssetRelease>()
                            .where(QUOTE(TRIGRAM_ASSET_RELEASE SEP "NAME") " = ?").bind(release)
                            .limit(1);
                    if (!asrPtr)
                    {
                        Echoes::Dbo::AssetRelease *newAsr = new Echoes::Dbo::AssetRelease();
                        newAsr->name = release;
                        asrPtr = m_session.add<Echoes::Dbo::AssetRelease>(newAsr);
                        asrPtr.flush();
                    }
                    astPtr.modify()->assetRelease = asrPtr;
                }

                res = serialize(astPtr, responseMsg);
            } 
            else 
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, astPtr);
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

EReturnCode AssetResource::putAliasForAsset(string &responseMsg, const string &sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    long long uroId;
    long long medId;
    Wt::WString value;

    if(!sRequest.empty())
    {    
        try
        {
            Wt::Json::Object result;

            Wt::Json::parse(sRequest, result);

            Wt::Json::Object alias = result.get("alias");
            uroId = alias.get("role_id");
            medId = alias.get("media_id");
            value = alias.get("value");
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
        responseMsg = httpCodeToJSON(res, "");
    }

    if(responseMsg.empty())
    {
        try 
        {
            Wt::Dbo::Transaction transaction(m_session);

            Wt::Dbo::ptr<Echoes::Dbo::Asset> astPtr = m_session.find<Echoes::Dbo::Asset>()
                    .where(QUOTE(TRIGRAM_ASSET ID) " = ?").bind(m_pathElements[1])
                    .where(QUOTE(TRIGRAM_ASSET SEP "DELETE") " IS NULL")
                    .where(QUOTE(TRIGRAM_ASSET SEP TRIGRAM_ORGANIZATION SEP TRIGRAM_ORGANIZATION ID) " = ?").bind(m_session.user()->organization.id());

            if (astPtr) 
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

                Wt::Dbo::ptr<Echoes::Dbo::Media> medPtr = m_session.find<Echoes::Dbo::Media>()
                        .where(QUOTE(TRIGRAM_MEDIA ID) " = ?").bind(medId)
                        .where(QUOTE(TRIGRAM_MEDIA SEP "DELETE") " IS NULL");
                if (!medPtr)
                {
                    res = EReturnCode::NOT_FOUND;
                    responseMsg = httpCodeToJSON(res, medPtr);
                    return res;
                }

                Wt::Dbo::ptr<Echoes::Dbo::AlertMessageAliasAsset> aaaPtr = m_session.find<Echoes::Dbo::AlertMessageAliasAsset>()
                        .where(QUOTE(TRIGRAM_USER_ROLE ID SEP TRIGRAM_USER_ROLE ID) " = ?").bind(uroId)
                        .where(QUOTE(TRIGRAM_ASSET ID SEP TRIGRAM_ASSET ID) " = ?").bind(m_pathElements[1])
                        .where(QUOTE(TRIGRAM_MEDIA ID SEP TRIGRAM_MEDIA ID) " = ?").bind(medId);
                if (aaaPtr) 
                {
                    aaaPtr.modify()->alias = value;
                }
                else
                {
                    Echoes::Dbo::AlertMessageAliasAsset *newAaa = new Echoes::Dbo::AlertMessageAliasAsset();
                    newAaa->pk.asset = astPtr;
                    newAaa->pk.userRole = uroPtr;
                    newAaa->pk.media = medPtr;
                    newAaa->alias = value;
                    aaaPtr = m_session.add<Echoes::Dbo::AlertMessageAliasAsset>(newAaa);
                }
                res = EReturnCode::OK;
            } 
            else 
            {
                res = EReturnCode::NOT_FOUND;
                responseMsg = httpCodeToJSON(res, astPtr);
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

void AssetResource::processPutRequest(Wt::Http::Response &response)
{
    string responseMsg = "";
    string nextElement = "";

    nextElement = getNextElementFromPath();
    if(nextElement.empty())
    {
        m_statusCode = EReturnCode::BAD_REQUEST;
        responseMsg = httpCodeToJSON(m_statusCode, "");
    }
    else
    {   
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath();

            if(nextElement.empty())
            {
                m_statusCode = putAsset(responseMsg, m_requestData);
            }
            else if(!nextElement.compare("alias"))
            {
                m_statusCode = putAliasForAsset(responseMsg, m_requestData);
            }
            else
            {
                m_statusCode = EReturnCode::BAD_REQUEST;
                responseMsg = httpCodeToJSON(m_statusCode, "");
            }
        }
        catch(boost::bad_lexical_cast const& e)
        {
            m_statusCode = EReturnCode::BAD_REQUEST;
            responseMsg = httpCodeToJSON(m_statusCode, e);
        }
    }

    response.setStatus(m_statusCode);
    response.out() << responseMsg;
    return;
}

void AssetResource::processDeleteRequest(Wt::Http::Response &response)
{
    return;
}

void AssetResource::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    m_role = "";
    m_media = "";
    if (!request.getParameterValues("role").empty())
    {
        m_role = request.getParameterValues("role")[0];
    }
    
    if (!request.getParameterValues("media").empty())
    {
        m_media = request.getParameterValues("media")[0];
    }
    PublicApiResource::handleRequest(request, response);
    return;
}

