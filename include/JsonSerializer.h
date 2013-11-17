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

#ifndef JSONSERIALIZER_H
#define	JSONSERIALIZER_H

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <Wt/WDateTime>
#include <Wt/Dbo/Dbo>
#include <Wt/Auth/Dbo/AuthInfo>

namespace Wt
{
  namespace Dbo
  {
    class JsonSerializer {
    public:

        JsonSerializer(Session& s);

        static std::string transformFieldName(const std::string& fieldName);
        static std::string transformTableName(const std::string& fieldTable);

        template <class V>
        void act(Wt::Dbo::FieldRef< V> field)
        {
            m_currentElem->put(transformFieldName(field.name()), field.value());
        }
        // When the field is WDateTime or WString, field added as std::string to add quotes " " in json
        void act(Wt::Dbo::FieldRef< Wt::WDateTime> field)
        {
            m_currentElem->put<std::string>(transformFieldName(field.name()), field.value().toString().toUTF8());
        }
        void act(Wt::Dbo::FieldRef< Wt::WString> field)
        {
            m_currentElem->put<std::string>(transformFieldName(field.name()), field.value().toUTF8());
        }

        template <class V>
        void actPtr(Wt::Dbo::PtrRef< V> field)
        {
            boost::property_tree::ptree elem;
            m_currentElem = &elem;
            if (field.id() > 0) {
                if (field.value().get()->deleteTag.isNull())
                {
                    elem.put("id", field.id());
                    const_cast<V&> (*field.value().get()).persist(*this);
                    m_currentElem->put_child(transformTableName(m_session.tableName<V>()), elem);
                }
            }
            m_currentElem = &m_root;
        }
        template<class S>
        void actPtr(Wt::Dbo::PtrRef< Wt::Auth::Dbo::AuthInfo<S>> field)
        {

        }

        template <class V>
        void actCollection(const Wt::Dbo::CollectionRef< V> & collec)
        {
            long long i = 0;
            for (Wt::Dbo::ptr<V> &field : collec.value())
            {
                if (field) {
                    if (field.get()->deleteTag.isNull())
                    {
                        i++;
                    }
                }
            }
            m_currentElem->put<long long>(transformTableName(m_session.tableName<V>()) + "s", i);
        }
        template<class S>
        void actCollection(const Wt::Dbo::CollectionRef< Wt::Auth::Dbo::AuthInfo<S>> &collec) {

        }

        template <class C>
        void serialize(C& c)
        {
            const_cast<C&>(c).persist(*this);
            if ((!m_isPtr) && (!m_isCollection))
            {
                boost::property_tree::json_parser::write_json(m_ss, m_root);
                m_result = m_ss.str();
            }
        }

        template <class C>
        void serialize(Wt::Dbo::ptr< C> & c)
        {
            m_isPtr = true;
            if (c->deleteTag.isNull())
            {
                m_currentElem->put("id", c.id());
                const_cast<C&> (*c).persist(*this);
            }

            boost::property_tree::json_parser::write_json(m_ss, m_root);

            if (!m_isCollection)
            {
                m_result = m_ss.str();
            }
        }
        template<class S>
        void serialize(Wt::Dbo::ptr< Wt::Auth::Dbo::AuthInfo<S>> &c)
        {

        }

        template <class C>
        void serialize(Wt::Dbo::collection< Wt::Dbo::ptr< C> >& cs)
        {
            long unsigned int i = 0;
            m_isCollection = true;
            m_ss << "[\n";
            for (auto& c : cs)
            {
                serialize(c);
                if (i < cs.size() - 1)
                {
                    std::string tmp = m_ss.str().erase(m_ss.str().size() - 1);
                    m_ss.str("");
                    m_ss.clear();
                    m_ss << tmp;
                    m_ss << ",\n";
                }
                i++;
            }
//            out_ << ss.str() << "]\n";
            m_result = m_ss.str() + "]\n";
        }
        template<class S>
        void serialize(Wt::Dbo::collection< Wt::Dbo::ptr< Wt::Auth::Dbo::AuthInfo<S>> >& cs)
        {

        }

        std::string getResult();
        void print();
        Session *session();
    private:

        std::ostream& m_out;
        std::stringstream m_ss;
        bool m_isCollection;
        bool m_isPtr;
        Session& m_session;
        std::string m_result;
        boost::property_tree::ptree m_root, *m_currentElem;
    };

  }
}

#endif	/* JSONSERIALIZER_H */

