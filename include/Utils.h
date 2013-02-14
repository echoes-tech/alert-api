/* 
 * File:   Utils.h
 * Author: tsa
 *
 * Created on 8 août 2012, 14:33
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
        };
        
        static std::string connection;
};


#endif	/* UTILS_H */

