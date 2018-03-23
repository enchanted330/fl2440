/*********************************************************************************
 *      Copyright:  (C) 2013 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  time.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(09/28/2013~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "09/28/2013 01:37:18 PM"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>


/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    time_t            tm;
    struct tm         *ptm;
    char              buf[512];

    time(&tm);

    ptm = localtime(&tm);
    printf("locatltime: Current data time: %d-%d-%d %d:%d:%d [%d] in week, [%d] in year\n", 
            (ptm->tm_year+1900), (ptm->tm_mon+1), ptm->tm_mday,
            ptm->tm_hour, ptm->tm_min, ptm->tm_sec, ptm->tm_wday, ptm->tm_yday);

    ptm = gmtime(&tm);
    printf(" gmtime: Current date time: %d-%d-%d %d:%d:%d [%d] in week, [%d] in year\n", 
            (ptm->tm_year+1900), (ptm->tm_mon+1), ptm->tm_mday,
            ptm->tm_hour, ptm->tm_min, ptm->tm_sec, ptm->tm_wday, ptm->tm_yday);


    printf("asctime: Current date time: %s", asctime(ptm));
    printf("  ctime: Current date time: %s", ctime(&tm));

    memset(buf, 0, sizeof(buf));
    strftime(buf, sizeof(buf), "%F %H:%M:%S %W [%z]", ptm);
    printf("strftime: Current date time: %s\n", buf);

    return 0;
} /* ----- End of main() ----- */

