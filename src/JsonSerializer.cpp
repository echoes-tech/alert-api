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
        m_maxRank(2),
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
