/* 
 * API GroupResource
 * @author ECHOES Technologies (GDR)
 * @date 18/02/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "GroupResource.h"

using namespace std;

GroupResource::GroupResource(Echoes::Dbo::Session& session) : PublicApiResource::PublicApiResource(session)
{
}

GroupResource::~GroupResource()
{
}

EReturnCode GroupResource::getGroupsList(const long long &grpId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Group>> grpPtrCol = m_session.find<Echoes::Dbo::Group>()
                .where(QUOTE(TRIGRAM_GROUP SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_GROUP ID) " = ?").bind(grpId)
                .orderBy(QUOTE(TRIGRAM_GROUP ID));

        res = serialize(grpPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode GroupResource::getGroup(const std::vector<std::string> &pathElements, const long long &grpId, std::string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::Group> grpPtr = m_session.find<Echoes::Dbo::Group>()
                .where(QUOTE(TRIGRAM_GROUP SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_GROUP ID) " = ?").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_GROUP ID) " = ?").bind(grpId);

        res = serialize(grpPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode GroupResource::processGetRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
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
        res = getGroupsList(grpId, responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.empty())
            {
                res = getGroup(pathElements, grpId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Group Resource] bad nextElement";
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

