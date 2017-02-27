/**
 * @file ItookiSendedReceiver.h
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

#ifndef ITOOKISENDEDRECEIVER_H
#define	ITOOKISENDEDRECEIVER_H

#include "PublicItookiResource.h"


class ItookiSendedReceiver   : public PublicItookiResource
{
    public:
        ItookiSendedReceiver(Echoes::Dbo::Session& session);
        virtual ~ItookiSendedReceiver();

    protected:

        std::string m_number;
        std::string m_message;
        
        Wt::WTimer *timer;
        
        void endTimer(Wt::WString refenvoiToChange, Wt::WString refenvoi, Wt::WString error, bool sended);
        EReturnCode postSended(map<string, long long> parameters, const vector<string> &pathElements, const string &sRequest, string &responseMsg);
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, std::string &responseMsg);

};

#endif	/* ITOOKISENDEDRECEIVER_H */
