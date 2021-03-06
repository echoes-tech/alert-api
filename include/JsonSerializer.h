/**
 * @file JsonSerializer.h
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

#ifndef JSONSERIALIZER_H
#define	JSONSERIALIZER_H

#include <boost/algorithm/string.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <Wt/WDateTime>
#include <Wt/Dbo/Dbo>
#include <Wt/Auth/Dbo/AuthInfo>
#include <Wt/WLogger>

#include <tools/Session.h>

namespace Wt { namespace Dbo {

class JsonSerializer
{
public:

    JsonSerializer(Session& s);
    virtual ~JsonSerializer();

    static std::string transformFieldName(const std::string& fieldName);
    static std::string transformTableName(const std::string& fieldTable);
    static std::string getTrigramFromTableName(const std::string& tableName);

    template <class V>
    void act(Wt::Dbo::FieldRef<V> field)
    {
//        std::cout << "In act(Wt::Dbo::FieldRef<V> field) - " << field.name() << " - rank: " << boost::lexical_cast<std::string>(m_rank) << std::endl;
//        std::cout << "V is: " << typeid(V).name() << std::endl;
        m_currentElem->put(transformFieldName(field.name()), field.value());
//        std::cout << "Out act(Wt::Dbo::FieldRef<V> field) - " << field.name() << " - rank: " << boost::lexical_cast<std::string>(m_rank) << std::endl;
    }
    // When the field is WDateTime or WString, field added as std::string to add quotes " " in json

    void act(Wt::Dbo::FieldRef<Wt::WDateTime> field)
    {
//        std::cout << "In act(Wt::Dbo::FieldRef<Wt::WDateTime> field)) - " << field.name() << " - rank: " << boost::lexical_cast<std::string>(m_rank) << std::endl;
        m_currentElem->put<std::string>(transformFieldName(field.name()), field.value().toString().toUTF8());
//        std::cout << "Out act(Wt::Dbo::FieldRef<Wt::WDateTime> field)) - " << field.name() << " - rank: " << boost::lexical_cast<std::string>(m_rank) << std::endl;
    }

    void act(Wt::Dbo::FieldRef<Wt::WString> field)
    {
//        std::cout << "In act(Wt::Dbo::FieldRef<Wt::WString> field)) - " << field.name() << " - rank: " << boost::lexical_cast<std::string>(m_rank) << std::endl;
        m_currentElem->put<std::string>(transformFieldName(field.name()), field.value().toUTF8());
//        std::cout << "Out act(Wt::Dbo::FieldRef<Wt::WString> field)) - " << field.name() << " - rank: " << boost::lexical_cast<std::string>(m_rank) << std::endl;
    }

    void act(Wt::Dbo::FieldRef<boost::optional<Wt::WString>> field)
    {
//        std::cout << "In act(Wt::Dbo::FieldRef<boost::optional<Wt::WString>> field)) - " << field.name() << " - rank: " << boost::lexical_cast<std::string>(m_rank) << std::endl;
        std::string value;
        if (field.value())
        {
            value = field.value().get().toUTF8();
        }
        else
        {
            value = "";
        }
        m_currentElem->put<std::string>(transformFieldName(field.name()), value);
//        std::cout << "Out act(Wt::Dbo::FieldRef<boost::optional<Wt::WString>> field)) - " << field.name() << " - rank: " << boost::lexical_cast<std::string>(m_rank) << std::endl;
    }

    template <class V>
    void actId(V& value, const std::string& name, int& size)
    {
        field(*this, value, name, size);
    }

    template <class V>
    void actPtr(Wt::Dbo::PtrRef< V> field)
    {
//        std::cout << "In actPtr(Wt::Dbo::PtrRef< V> field) - " << m_session.tableName<V>() << " - rank: " << boost::lexical_cast<std::string>(m_rank) << std::endl;
        if (m_rank < m_maxRank)
        {
            m_rank++;
            processSerialize(field.value());
            m_rank--;
        }
        else if (m_rank == m_maxRank)
        {
            processSerialize(field.value());
        }
//        std::cout << "Out actPtr(Wt::Dbo::PtrRef< V> field) - " << m_session.tableName<V>() << " - rank: " << boost::lexical_cast<std::string>(m_rank) << std::endl;
    }

    template<class S>
    void actPtr(Wt::Dbo::PtrRef< Wt::Auth::Dbo::AuthInfo<S >> field)
    {
        Wt::log("debug") << "[JsonSerializer] - actPtr(Wt::Dbo::PtrRef< Wt::Auth::Dbo::AuthInfo<S >> field)";
    }

    template <class V>
    void actWeakPtr(WeakPtrRef<V> field)
    {
        Wt::log("debug") << "[JsonSerializer] - actWeakPtr(WeakPtrRef<V> field)";
    }

    template <class V>
    void actCollection(const Wt::Dbo::CollectionRef<V> & collec)
    {
//        std::cout << "In actCollection(const Wt::Dbo::CollectionRef< V> & collec) - " << m_session.tableName<V>() << " - size: " << collec.value().size() << " rank: " << boost::lexical_cast<std::string>(m_rank) << std::endl;
        // FIXME i'm famous !
        // Si cette méthode est appelée de manière récursive, m_joinTableContainer est partargé.
        // Ceci a pour conséquence, que si un objet apparait dans une collection de rang > 0
        // il ne sera présent que dans le 1er objet de la collection.
        if (std::find(m_joinTableContainer.begin(), m_joinTableContainer.end(), collec.joinName()) == m_joinTableContainer.end())
        {
            m_joinTableContainer.push_back(collec.joinName());

            std::string tableName = "";
            std::string fieldIdName = "";
            std::string deleteCondition = "";
            if (boost::starts_with(collec.joinName(), TABLE_JOINT_PREFIX SEP))
            {
                tableName = collec.joinName();
                fieldIdName = m_parentTableName + SEP + getTrigramFromTableName(m_parentTableName) + ID;
                deleteCondition = "";
            }
            else
            {
                tableName = boost::lexical_cast<std::string>(m_session.tableName<V>());
                fieldIdName = collec.joinName() + SEP + getTrigramFromTableName(m_parentTableName) + ID;
                deleteCondition = "AND \"" + getTrigramFromTableName(boost::lexical_cast<std::string>(m_session.tableName<V>())) + SEP "DELETE\" IS NULL";
            }

            const long long i = m_session.query<long long>(
" SELECT COUNT(1)"
"   FROM \"" + tableName + "\""
"   WHERE"
"     \"" + fieldIdName + "\" = " + boost::lexical_cast<std::string>(m_currentElem->get<long long>("id")) +
"     " + deleteCondition
            );

            m_currentElem->put<long long>(transformTableName(m_session.tableName<V>()) + "s", i);
        }
//        std::cout << "Out actCollection(const Wt::Dbo::CollectionRef< V> & collec) - " << m_session.tableName<V>() << " - size: " << collec.value().size() << " rank: " << boost::lexical_cast<std::string>(m_rank) << std::endl;
    }
    void actCollection(const Wt::Dbo::CollectionRef<Echoes::Dbo::Syslog> & collec)
    {
        Wt::log("debug") << "[JsonSerializer] - actCollection(const Wt::Dbo::CollectionRef<Echoes::Dbo::Syslog> & collec)";
    }
    // We do not want to display AuthInfo to our users.
    template<class S>
    void actCollection(const Wt::Dbo::CollectionRef< Wt::Auth::Dbo::AuthInfo<S >> &collec)
    {
        Wt::log("debug") << "[JsonSerializer] - actCollection(const Wt::Dbo::CollectionRef< Wt::Auth::Dbo::AuthInfo<S >> &collec)";
    }

    template <class C>
    void processSerialize(C& c)
    {
//        std::cout << "In processSerialize(C& c) - " << m_session.tableName<C>() << " - rank: " << boost::lexical_cast<std::string>(m_rank) << std::endl;
        const_cast<C&> (c).persist(*this);
//        std::cout << "Out processSerialize(C& c) - " << m_session.tableName<C>() << " - rank: " << boost::lexical_cast<std::string>(m_rank) << std::endl;
    }

    template <class C>
    void processSerialize(Wt::Dbo::ptr< C> & c)
    {
//        std::cout << "In processSerialize(Wt::Dbo::ptr< C> & c) - " << m_session.tableName<C>() << " - rank: " << boost::lexical_cast<std::string>(m_rank) << std::endl;
        if (m_rank <= m_maxRank)
        {
            if (c)
            {
                if (c->deleteTag.isNull())
                {
                    boost::property_tree::ptree elem;
                    boost::property_tree::ptree *previousElem;
                    std::string previousParentTableName;

                    if (m_currentElem != &m_root || m_rank > 0)
                    {
                        if (!m_isCollection || m_rank > 1)
                        {
                            previousElem = m_currentElem;
                            m_currentElem = &elem;
                            previousParentTableName = m_parentTableName;
                        }
                    }
                    m_rank++;
                    m_currentElem->put("id", c.id());
                    if (m_rank <= m_maxRank)
                    {
                        m_parentTableName = m_session.tableName<C>();
                        const_cast<C&> (*c).persist(*this);
                    }
                    m_rank--;
                    if (m_currentElem != &m_root || m_rank > 0)
                    {
                        if (!m_isCollection || m_rank > 1)
                        {
                            m_currentElem = previousElem;
                            m_currentElem->put_child(transformTableName(m_session.tableName<C>()), elem);
                            m_parentTableName = previousParentTableName;
                        }
                    }
                }
                else
                {
                    m_currentElem->put(transformTableName(m_session.tableName<C>()), "{}");
                }
            }
            else
            {
                m_currentElem->put(transformTableName(m_session.tableName<C>()), "{}");
            }
        }
//        std::cout << "Out processSerialize(Wt::Dbo::ptr< C> & c) - " << m_session.tableName<C>() << " - rank: " << boost::lexical_cast<std::string>(m_rank) << std::endl;
    }

    template<class S>
    // We do not want to display AuthInfo to our users.
    void processSerialize(Wt::Dbo::ptr< Wt::Auth::Dbo::AuthInfo<S >> &c)
    {
        Wt::log("debug") << "[JsonSerializer] - processSerialize(Wt::Dbo::ptr< Wt::Auth::Dbo::AuthInfo<S >> &c)";
    }

    // WARNING: hack to create an array on root of JSON
    // boost::property_tree::json_parser::write_json doesn't handle array for root
    // boost version: 1.53

    template <class C>
    void processSerialize(Wt::Dbo::collection< Wt::Dbo::ptr<C>>& cs)
    {
//        std::cout << "In processSerialize(Wt::Dbo::collection< Wt::Dbo::ptr< C> >& cs) - " << m_session.tableName<C>() << " - rank: " << boost::lexical_cast<std::string>(m_rank) << std::endl;
        long unsigned int i = 0;
        m_ss << "[\n";
        m_rank++;
        for (auto& c : cs)
        {
            processSerialize(c);
            if (!m_currentElem->empty())
            {
                try
                {
                    boost::property_tree::json_parser::write_json(m_ss, *m_currentElem);
                }
                catch (boost::property_tree::json_parser::json_parser_error const &e)
                {
                    Wt::log("error") << "[JsonSerializer] - " << e.message();
                }
                m_result = m_ss.str();
                if (i < cs.size() - 1)
                {
                    std::string tmp = m_ss.str().erase(m_ss.str().size() - 1);
                    m_ss.str("");
                    m_ss.clear();
                    m_ss << tmp;
                    m_ss << ",\n";
                }
            }
            m_currentElem->clear();
            m_joinTableContainer.clear();
            ++i;
        }
// FIXME (or not.)
// When the last elem is "deleted" (echoes way) a ',' remains before the final brackett
// Should be removed if it exists
// We will fix this the day we have a problem which should not happen.
        m_rank--;
        m_ss << "]\n";
//        std::cout << "Out processSerialize(Wt::Dbo::collection< Wt::Dbo::ptr< C> >& cs) - " << m_session.tableName<C>() << " - rank: " << boost::lexical_cast<std::string>(m_rank) << std::endl;
    }

    template<class S>
    // We do not want to display AuthInfo to our users.
    void processSerialize(Wt::Dbo::collection< Wt::Dbo::ptr< Wt::Auth::Dbo::AuthInfo<S >> >& cs)
    {
        Wt::log("debug") << "[JsonSerializer] - processSerialize(Wt::Dbo::collection< Wt::Dbo::ptr< Wt::Auth::Dbo::AuthInfo<S >> >& cs)";
    }

    template <class C>
    void serialize(C& c)
    {
        m_rank = 0;
//        std::cout << "In serialize(C& c) - rank: " << boost::lexical_cast<std::string>(m_rank) << std::endl;
        m_joinTableContainer.clear();
        processSerialize(c);
        try
        {
            boost::property_tree::json_parser::write_json(m_ss, m_root);
        }
        catch (boost::property_tree::json_parser::json_parser_error const &e)
        {
            Wt::log("error") << "[JsonSerializer] - " << e.message();
        }
        m_result = m_ss.str();
//        std::cout << "Out serialize(C& c) - rank: " << boost::lexical_cast<std::string>(m_rank) << std::endl;
    }
    // WARNING: hack to create an array on root of JSON
    // boost::property_tree::json_parser::write_json doesn't handle array for root
    // boost version: 1.53

    template <class C>
    void serialize(Wt::Dbo::collection< Wt::Dbo::ptr< C> >& cs)
    {
        m_rank = 0;
        m_isCollection = true;
//        std::cout << "In serialize(Wt::Dbo::collection< Wt::Dbo::ptr< C> >& cs) - rank: " << boost::lexical_cast<std::string>(m_rank) << std::endl;
        m_joinTableContainer.clear();
        processSerialize(cs);
        m_result = m_ss.str();
//        std::cout << "Out serialize(Wt::Dbo::collection< Wt::Dbo::ptr< C> >& cs) - rank: " << boost::lexical_cast<std::string>(m_rank) << std::endl;
    }

    std::string getResult();
    void print();
    Session *session();
private:
    std::ostream& m_out;
    std::stringstream m_ss;
    Session& m_session;
    std::string m_result;
    boost::property_tree::ptree m_root;
    boost::property_tree::ptree *m_currentElem;
    const unsigned short m_maxRank;
    bool m_isCollection;
    std::string m_parentTableName;

    static std::vector<std::string> m_joinTableContainer;
    static unsigned short m_rank;
};

}}

#endif	/* JSONSERIALIZER_H */
