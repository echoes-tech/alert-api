/* 
 * API AddonResource
 * @author ECHOES Technologies (GDR)
 * @date 6/03/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "AddonResource.h"

using namespace std;

AddonResource::AddonResource(Echoes::Dbo::Session& session) : PublicApiResource::PublicApiResource(session)
{
    resourceClassName = "AddonResource";
    
    functionMap["getAddonsList"] = boost::bind(&AddonResource::getAddonsList, this, _1, _2, _3, _4);
    functionMap["getAddon"] = boost::bind(&AddonResource::getAddon, this, _1, _2, _3, _4);
    functionMap["getSearch_typesForAddon"] = boost::bind(&AddonResource::getSearch_typesForAddon, this, _1, _2, _3, _4);
    
    calls = FillCallsVector();
    
    /*Call structFillTmp;
    structFillTmp.method = "GET";
    structFillTmp.path = "";
    structFillTmp.function = boost::bind(&AddonResource::getAddonsList, this, _1, _2, _3, _4);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "GET";
    structFillTmp.path = "/[0-9]+";
    structFillTmp.function = boost::bind(&AddonResource::getAddon, this, _1, _2, _3, _4);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "GET";
    structFillTmp.path = "/search_types/(.)*";
    structFillTmp.function = boost::bind(&AddonResource::getSearch_typesForAddon, this, _1, _2, _3, _4);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "GET";
    structFillTmp.path = "/(\\D)*";
    structFillTmp.function = boost::bind(&AddonResource::getSearch_typesForAddon, this, _1, _2, _3, _4);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "POST";
    structFillTmp.path = ".*";
    structFillTmp.function = boost::bind(&AddonResource::Error, this, _1, _2, _3, _4);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "PUT";
    structFillTmp.path = ".*";
    structFillTmp.function = boost::bind(&AddonResource::Error, this, _1, _2, _3, _4);
    calls.push_back(structFillTmp);
    
    structFillTmp.method = "DELETE";
    structFillTmp.path = ".*";
    structFillTmp.function = boost::bind(&AddonResource::Error, this, _1, _2, _3, _4);
    calls.push_back(structFillTmp);*/
}

AddonResource::~AddonResource()
{
}

EReturnCode AddonResource::getAddonsList(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Addon>> adoPtrCol = m_session.find<Echoes::Dbo::Addon>()
                .where(QUOTE(TRIGRAM_ADDON SEP "DELETE") " IS NULL")
                .orderBy(QUOTE(TRIGRAM_ADDON ID));

        res = serialize(adoPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode AddonResource::getAddon(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::Addon> adoPtr = m_session.find<Echoes::Dbo::Addon>()
                .where(QUOTE(TRIGRAM_ADDON ID) " = ?").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_ADDON SEP "DELETE") " IS NULL");

        res = serialize(adoPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode AddonResource::getSearch_typesForAddon(const long long &orgId, std::string &responseMsg, const std::vector<std::string> &pathElements, const std::string &sRequest)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        const string queryStr =
        " SELECT sty"
        "   FROM " QUOTE(TABLE_REFERENCE_PREFIX SEP "SEARCH_TYPE" SEP TRIGRAM_SEARCH_TYPE) " sty"
        "   WHERE"
        "     " QUOTE(TRIGRAM_SEARCH_TYPE ID) " IN"
        "       ("
        "         SELECT " QUOTE(TABLE_REFERENCE_PREFIX SEP "SEARCH_TYPE" SEP TRIGRAM_SEARCH_TYPE SEP TRIGRAM_SEARCH_TYPE ID)
        "           FROM " QUOTE(TABLE_JOINT_PREFIX SEP TRIGRAM_ADDON SEP TRIGRAM_SEARCH_TYPE)
        "           WHERE " QUOTE(TABLE_REFERENCE_PREFIX SEP "ADDON" SEP TRIGRAM_ADDON SEP TRIGRAM_ADDON ID) " ="
        "             ("
        "               SELECT " QUOTE(TRIGRAM_ADDON ID)
        "                 FROM " QUOTE(TABLE_REFERENCE_PREFIX SEP "ADDON" SEP TRIGRAM_ADDON)
        "                 WHERE"
        "                   " QUOTE(TRIGRAM_ADDON ID) " = " + pathElements[1] +
        "                   AND " QUOTE(TRIGRAM_ADDON SEP "DELETE") " IS NULL"
        "             )"
        "       )"
        "     AND " QUOTE(TRIGRAM_SEARCH_TYPE SEP "DELETE") " IS NULL"
        " ORDER BY " QUOTE(TRIGRAM_SEARCH_TYPE ID);

        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::SearchType>> queryRes = m_session.query<Wt::Dbo::ptr<Echoes::Dbo::SearchType>>(queryStr);        
        
        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::SearchType>> styPtrCol = queryRes.resultList();

        res = serialize(styPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode AddonResource::processGetRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
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
        res = getAddonsList(grpId, responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);
            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.empty())
            {
                res = getAddon(orgId, responseMsg, pathElements);
            }
            else if (nextElement.compare("search_types") == 0)
            {
                res = getSearch_typesForAddon(orgId, responseMsg, pathElements);                
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Addon Resource] bad nextElement";
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

