/* 
 * Json Serializer
 * @author Vermeille Guillaume <Guillaume.v.sanchez@gmail.com>
 * @modified by ECHOES Technologies <contact@echoes-tech.com> 
 * @date 15/11/2013
 * 
 * Copyright (C) 2013 ECHOES TECHNOLGIES SAS <contact@echoes-tech.com> 
 * Copyright (C) 2012 Vermeille Guillaume <Guillaume.v.sanchez@gmail.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software 
 * and associated documentation files (the "Software"), to deal in the Software without restriction, 
 * including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, 
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, 
 * subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial 
 * portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT 
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, 
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE 
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "JsonSerializer.h"

namespace Wt
{
    namespace Dbo
    {
        unsigned short JsonSerializer::m_rank = 0;
        std::vector<std::string> JsonSerializer::m_joinTableContainer;
        
        JsonSerializer::JsonSerializer(Session& s) :
        m_out(std::cout),
        m_session(s),
        m_result(""),
        m_currentElem(&m_root),
        m_maxRank(3),
        m_isCollection(false)
        {
        }
        
        std::string JsonSerializer::transformFieldName(const std::string& fieldName) {
            std::string res = fieldName;
            std::transform(res.begin(), res.end(), res.begin(), ::tolower);
            res.erase(0, res.find_first_of("_") + 1);
            return res;
        }

        std::string JsonSerializer::transformTableName(const std::string& fieldTable) {
            std::string res = fieldTable;
            // Check if fieldTable has more than 1 '_'
            if (res.find_first_of("_") != res.find_last_of("_")) {
                res = transformFieldName(fieldTable);
                res.erase(res.find_last_of("_"));
            }
            return res;
        }
        
        std::string JsonSerializer::getResult()
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
