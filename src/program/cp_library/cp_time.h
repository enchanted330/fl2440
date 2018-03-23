/********************************************************************************
 *      Copyright:  (C) 2012 CoherentPlus Sdn. Bhd.
 *                  All rights reserved.
 *
 *       Filename:  cp_time.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(02/23/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "02/23/2012 07:46:37 AM"
 *                 
 ********************************************************************************/
#ifndef __CP_TIME_H
#define __CP_TIME_H

#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>

#include <linux/rtc.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

typedef struct __DATE_TIME
{
    int iYear;
    int iMonth;
    int iDay;
    int iHour;
    int iMinute;
    int iSecond;
    int iDayOfWeek;
} DATE_TIME;

static inline void micro_second_sleep(unsigned long ms)
{
    struct timespec cSleep;
    unsigned long ulTmp;
    cSleep.tv_sec = ms / 1000;
    if (cSleep.tv_sec == 0)
    {
        ulTmp = ms * 10000;
        cSleep.tv_nsec = ulTmp * 100;
    }
    else
    {
        cSleep.tv_nsec = 0;
    } 
    
    nanosleep(&cSleep, 0);
}

/*UNIT: micro second*/
static inline unsigned long time_now()
{
    struct timeval now; 
    gettimeofday(&now, 0);
    return (now.tv_sec * 1000) + (now.tv_usec / 1000);
}

/*UNIT: micro second*/
static inline unsigned long time_elapsed(unsigned long start)
{
    unsigned long current = time_now(); 
    
    if (current < start)
    {
        return (0xFFFFFFFF - start) + current;
    }

    //printf("time_elapsed: %ld\n", current-start);
    return current - start;
}
               
static inline void get_current_time(DATE_TIME * date)
{
    time_t now = time(NULL);
    struct tm *tnow = localtime(&now); 
    
    memset(date, 0, sizeof(DATE_TIME)); 
    date->iYear = 1900 + tnow->tm_year;
    date->iMonth = 1 + tnow->tm_mon;
    date->iDay = tnow->tm_mday;

    date->iHour = tnow->tm_hour;
    date->iMinute = tnow->tm_min;
    date->iSecond = tnow->tm_sec; 
    date->iDayOfWeek = tnow->tm_wday; 
    return;
}
#define get_sys_time(date)   get_current_time(date)

static inline int get_rtc_time(DATE_TIME *date)
{
    int                 rv, fd = -1;
    struct rtc_time     rtc_tm;  

    memset(date, 0, sizeof(DATE_TIME));

    if ((fd=open("/dev/rtc0", O_RDONLY)) < 0)
        return -1;

    if((rv=ioctl(fd, RTC_RD_TIME, &rtc_tm)) < 0)
        return -2;

    date->iYear = 1900 + rtc_tm.tm_year;
    date->iMonth = 1 + rtc_tm.tm_mon;
    date->iDay = rtc_tm.tm_mday;

    date->iHour = rtc_tm.tm_hour;
    date->iMinute = rtc_tm.tm_min;
    date->iSecond = rtc_tm.tm_sec;
    date->iDayOfWeek = rtc_tm.tm_wday;

    close(fd);

    return 0;
}

#endif
