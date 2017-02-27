/**
 * @file XmlSerializer.h
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

#ifndef XMLSERIALIZER_H
#define	XMLSERIALIZER_H

#include <Wt/WDateTime>
#include <Wt/Dbo/Dbo>

namespace Wt
{
  namespace Dbo
  {

    class XmlSerializer {
    public:

        XmlSerializer(std::ostream& out, Session& s) : out_(out), s_(s) {
            indent_ = 1;
        }

        template <class V>
        void act(Wt::Dbo::FieldRef< V> field) {
            Indent();
            out_ << "<" << field.name() << ">" 
                    << field.value()
                    << "</" << field.name() << ">" 
                    << std::endl;
        }
        void act(Wt::Dbo::FieldRef< Wt::WDateTime> field) {
            Indent();
            out_ << "<" << field.name() << ">" 
                    << field.value().toString().toUTF8()
                    << "</" << field.name() << ">" 
                    << std::endl;
        }
        
        template <class V>
        void actPtr(Wt::Dbo::PtrRef< V> field) {
            Indent();
            out_ << "<" << s_.tableName<V>() << ">" << std::endl;
            ++indent_;
                Indent();
                out_ << "<id>" << field.id() << "</id>" << std::endl;
            --indent_;
            Indent();
            out_ << "</" << s_.tableName<V>() << ">" << std::endl;
        }

        template <class V>
        void actCollection(const Wt::Dbo::CollectionRef< V> & collec) {
            Indent();
            out_ << "<" << s_.tableName<V>() << "s>" << std::endl;
            ++indent_;
            for (Wt::Dbo::ptr<V> &field : collec.value()) {
                Indent();
                out_ << "<id>" << field.id() << "</id>" << std::endl;
            }
            --indent_;
            Indent();
            out_ << "</" << s_.tableName<V>() << "s>" << std::endl;
        }

        template <class C>
        void Serialize(C& c) const {
            c.persist(this);
        }

        template <class C>
        void Serialize(Wt::Dbo::ptr< C> & c) {
            Indent();
            out_ << "<" << s_.tableName<C>() << ">" << std::endl;
            ++indent_;
            const_cast<C&> (*c).persist(*this);
            --indent_;
            Indent();
            out_ << "</" << s_.tableName<C>() << ">" << std::endl;
        }

        template <class C>
        void Serialize(Wt::Dbo::collection< Wt::Dbo::ptr< C> >& cs) {
            Indent();
            out_ << "<" << s_.tableName<C>() << "s>" << std::endl;
            ++indent_;
            for (auto& c : cs) {
                Serialize(c);
            }
            --indent_;
            Indent();
            out_ << "</" << s_.tableName<C>() << "s>" << std::endl;
        }
        
        Session *session() { return &s_; }
    private:

        void Indent(void) const {
            for (int i = 0; i < indent_; ++i)
                out_ << "  ";
        }
        std::ostream& out_;
        Session& s_;
        int indent_;
    };

  }
}

#endif	/* XMLSERIALIZER_H */
