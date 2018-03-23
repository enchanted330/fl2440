/********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<Email:guowenxue@gmail.com QQ:281143292>
 *                  All rights reserved.
 *
 *       Filename:  mytime.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(07/27/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "07/27/2012 02:17:24 PM"
 *                 
 ********************************************************************************/

#ifndef __MYTIME_H
#define __MYTIME_H

#include <iostream>

class Time
{
    private:
        int           hours;
        int           minutes;

    public:
        Time();
        Time(int h, int m=0);
        ~Time();

        void AddMin(int m);
        void AddHour(int h);
        void Reset(int h=0, int m=0);

        Time operator+(const Time &t) const;
        Time operator-(const Time &t) const;
        Time operator*(double n) const;

        friend Time operator*(double m, const Time & t) { return t * m; }
        friend std::ostream & operator<<(std::ostream &os, const Time &t);
};



#endif
