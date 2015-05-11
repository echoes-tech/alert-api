/* 
 * API Option Resource
 * @author ECHOES Technologies (FPO)
 * @date 29/11/2013
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "OptionResource.h"

using namespace std;

OptionResource::OptionResource(Echoes::Dbo::Session& session) : PublicApiResource::PublicApiResource(session)
{
}

OptionResource::~OptionResource()
{
}

EReturnCode OptionResource::getOptionsList(std::map<std::string, long long> &parameters, const long long &grpId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::Query<Wt::Dbo::ptr<Echoes::Dbo::Option>> queryRes = m_session.find<Echoes::Dbo::Option>()
                .where(QUOTE(TRIGRAM_OPTION SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_OPTION SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = ?").bind(grpId)
                .orderBy(QUOTE(TRIGRAM_OPTION ID));

        if (parameters["type_id"] > 0)
        {
            queryRes = queryRes.where(QUOTE(TRIGRAM_OPTION SEP TRIGRAM_OPTION_TYPE SEP TRIGRAM_OPTION_TYPE ID) " = ?");
            // FPO: I don't know why but we have to bind every marker here to make the binding.
            queryRes.bind(grpId).bind(parameters["type_id"]);
        }

        Wt::Dbo::collection<Wt::Dbo::ptr<Echoes::Dbo::Option>> optPtrCol =  queryRes.resultList();

        res = serialize(optPtrCol, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode OptionResource::getOption(const std::vector<std::string> &pathElements, const long long &grpId, string &responseMsg)
{
    EReturnCode res = EReturnCode::INTERNAL_SERVER_ERROR;
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);

        Wt::Dbo::ptr<Echoes::Dbo::Option> optPtr = m_session.find<Echoes::Dbo::Option>()
                .where(QUOTE(TRIGRAM_OPTION ID) " = ?").bind(pathElements[1])
                .where(QUOTE(TRIGRAM_OPTION SEP "DELETE") " IS NULL")
                .where(QUOTE(TRIGRAM_OPTION SEP TRIGRAM_GROUP SEP TRIGRAM_GROUP ID) " = ?").bind(grpId);

        res = serialize(optPtr, responseMsg);

        transaction.commit();
    }
    catch (Wt::Dbo::Exception const& e)
    {
        res = EReturnCode::SERVICE_UNAVAILABLE;
        responseMsg = httpCodeToJSON(res, e);
    }
    return res;
}

EReturnCode OptionResource::processGetRequest(const Wt::Http::Request &request, const long long &grpId, std::string &responseMsg)
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
        res = getOptionsList(parameters, grpId, responseMsg);
    }
    else
    {
        try
        {
            boost::lexical_cast<unsigned long long>(nextElement);

            nextElement = getNextElementFromPath(indexPathElement, pathElements);
            if (nextElement.empty())
            {
                res = getOption(pathElements, grpId, responseMsg);
            }
            else
            {
                res = EReturnCode::BAD_REQUEST;
                const string err = "[Option Resource] bad nextElement";
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

