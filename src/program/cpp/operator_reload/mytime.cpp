/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<Email:guowenxue@gmail.com QQ:281143292>
 *                  All rights reserved.
 *
 *       Filename:  mytime.cpp
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(07/27/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "07/27/2012 02:50:05 PM"
 *                 
 ********************************************************************************/

#include "mytime.h"

Time::Time()
{
    hours = minutes = 0;
}

Time::Time(int h, int m)
{
    hours = h;
    minutes = m;
}

Time::~Time()
{
    hours = minutes = 0;
}

void Time::AddMin(int m)
{
    minutes += m;

    hours += minutes/60;
    minutes %= 60;
}


void Time::AddHour(int h)
{
    hours += h;
}

void Time::Reset(int h, int m)
{
    hours = h;
    minutes = m;
}


Time Time::operator+(const Time &t) const
{
    Time sum;

    sum.hours = this->hours + t.hours;
    sum.minutes = this->minutes + t.minutes;

    sum.hours += sum.minutes/60;
    sum.minutes %= 60;

    return sum;
}


Time Time::operator-(const Time &t) const
{
    Time diff; 
    int  tot1, tot2;

    tot1 = hours + minutes*60;
    tot2 = t.hours + t.minutes*60;

    diff.minutes = (tot1-tot2) % 60;
    diff.hours = (tot1-tot2) / 60;

    return diff;
}

Time Time::operator*(double mul) const
{
    Time result(0,0);

    long tot = (hours*60+minutes) *mul;

    result.hours = tot / 60;
    result.minutes = tot % 60;

    return result;
}


std::ostream & operator<<(std::ostream &os, const Time & t)
{

    os<<t.hours<<" hours," <<t.minutes<<" minutes.";
    return os;
}


