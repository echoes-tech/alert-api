/**
 * @file ItookiAswReceiver.cpp
 * @author Thomas Saquet, Florent Poinsaut
 * @date 
 * @brief File containing example of doxygen usage for quick reference.
 *
 * Alert - API is a part of the Alert software
 * Copyright (C) 2013-2017
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
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
