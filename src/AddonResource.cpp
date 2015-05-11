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
}

AddonResource::~AddonResource()
{
}

EReturnCode AddonResource::getAddonsList(const long long &grpId, string& responseMsg)
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

EReturnCode AddonResource::getAddon(const vector<string> &pathElements, const long long &grpId, string& responseMsg)
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

EReturnCode AddonResource::getSearchTypeForAddon(const vector<string> &pathElements, const long long &grpId, string& responseMsg)
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
                res = getAddon(pathElements, grpId, responseMsg);
            }
            else if (nextElement.compare("search_types") == 0)
            {
                res = getSearchTypeForAddon(pathElements, grpId, responseMsg);                
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

