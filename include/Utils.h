/**
 * @file Utils.h
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

#ifndef UTILS_H
#define	UTILS_H

#include <tools/Session.h>
#include <tools/MainIncludeFile.h>

class Utils 
{
    public:
        template <class T>
        static bool checkId(Wt::Dbo::ptr<T> ptr)
        {
            bool res = false;
            if (ptr.id() != -1)
            {
                res = true;
            }
            return res;
        }
};


#endif	/* UTILS_H */
