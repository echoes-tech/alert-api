/**
 * @file mailAssign.h
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

#ifndef MAILASSIGN_H
#define	MAILASSIGN_H

#include "PublicMailResource.h"

#include <Wt/Mail/Client>
#include <Wt/Mail/Message>
#include <Wt/Http/Response>
#include <Wt/Http/ResponseContinuation>

class MailAssign : public PublicMailResource
{
    public:
        MailAssign(Echoes::Dbo::Session& session);
        virtual ~MailAssign();

    protected:

        std::string m_number;
        std::string m_message;
        
        void handleHttpResponse(boost::system::error_code err, const Wt::Http::Message& response, EReturnCode *res, string *responseMsg, bool * answered);
        EReturnCode doMailAssign(map<string, string> parameters, const vector<string> &pathElements, const string &sRequest, string &responseMsg, const Wt::Http::Request &request);
        virtual EReturnCode processPostRequest(const Wt::Http::Request &request, std::string &responseMsg);
        EReturnCode PostAssign(map<string, string> parameters, const vector<string> &pathElements, const string &sRequest, string &responseMsg, const Wt::Http::Request &request);
        virtual EReturnCode processGetRequest(const Wt::Http::Request &request, std::string &responseMsg);

};


#endif	/* MAILASSIGN_H */
