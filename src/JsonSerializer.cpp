/**
 * @file JsonSerializer.cpp
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

#include "JsonSerializer.h"

using namespace std;

namespace Wt
{
    namespace Dbo
    {
        unsigned short JsonSerializer::m_rank = 0;
        vector<string> JsonSerializer::m_joinTableContainer;
        
        JsonSerializer::JsonSerializer(Session& s) :
        m_out(cout),
        m_session(s),
        m_result(""),
        m_currentElem(&m_root),
        m_maxRank(3),
        m_isCollection(false),
        m_parentTableName("")
        {
        }

        JsonSerializer::~JsonSerializer()
        {
        }

        string JsonSerializer::transformFieldName(const string& fieldName)
        {
            string res = fieldName;
            transform(res.begin(), res.end(), res.begin(), ::tolower);
            res.erase(0, res.find_first_of("_") + 1);
            return res;
        }

        string JsonSerializer::transformTableName(const string& fieldTable)
        {
            string res = fieldTable;
            // Check if fieldTable has more than 1 '_'
            if (res.find_first_of("_") != res.find_last_of("_")) {
                res = transformFieldName(fieldTable);
                res.erase(res.find_last_of("_"));
            }
            return res;
        }

        string JsonSerializer::getTrigramFromTableName(const string& tableName)
        {
            string res = tableName;
            res.erase(0, res.find_last_of("_") + 1);
            return res;
            
        }
        
        string JsonSerializer::getResult()
        {
            return m_result;
        }

        void JsonSerializer::print()
        {
            m_out << m_result;
        }

        Session *JsonSerializer::session() {
            return &m_session;
        }
    }
}
