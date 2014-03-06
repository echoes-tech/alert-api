/* 
 * Itooki SMS Answer Receiver
 * @author ECHOES Technologies (TSA)
 * @date 08/08/2012
 * 
 * THIS PROGRAM IS CONFIDENTIAL AND PROPRIETARY TO ECHOES TECHNOLOGIES SAS
 * AND MAY NOT BE REPRODUCED, PUBLISHED OR DISCLOSED TO OTHERS WITHOUT
 * COMPANY AUTHORIZATION.
 * 
 * COPYRIGHT 2012-2013 BY ECHOES TECHNOLGIES SAS
 * 
 */

#include "itooki/ItookiAswReceiver.h"

using namespace std;

ItookiAswReceiver::ItookiAswReceiver(Echoes::Dbo::Session& session) : Wt::WResource(),
m_session(session)
{
}

ItookiAswReceiver::~ItookiAswReceiver()
{
    beingDeleted();
}

void ItookiAswReceiver::handleRequest(const Wt::Http::Request &request, Wt::Http::Response &response)
{
    Wt::log("notice") << "[ASW] Query string : " << request.queryString();


    m_number = "";
    m_message = "";

    if (!request.getParameterValues("numero").empty())
    {
        m_number = request.getParameterValues("numero")[0];
    }
    if (!request.getParameterValues("message").empty()) 
    {
        m_message = request.getParameterValues("message")[0];
    }

    // new transaction
    try
    {
        Wt::Dbo::Transaction transaction(m_session, true);
        Echoes::Dbo::UserHistoricalAction *uha = new Echoes::Dbo::UserHistoricalAction;
        uha->dateTime = Wt::WDateTime::currentDateTime();
        uha->tableObject = m_number + " : " + m_message;
        m_session.add<Echoes::Dbo::UserHistoricalAction>(uha);
    }
    catch(Wt::Dbo::Exception const& e)
    {
        Wt::log("error") << "[ASW ITOOKI]" << e.what();
        //TODO : behaviour in error case
    }


}