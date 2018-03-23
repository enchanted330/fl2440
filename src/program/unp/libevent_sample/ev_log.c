/*********************************************************************************
 *      Copyright:  (C) 2014 Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  ev_log.c
 *    Description:  This file used to show how to use libevent log system
 *                 
 *        Version:  1.0.0(01/27/2014)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "01/27/2014 03:33:09 PM"
 *                 
 ********************************************************************************/


#include <stdio.h>
#include <string.h>

#include "event.h"
#include "log-internal.h"

#define LOG_ROLLBACK_SIZE           512        /*  Default rollback log size  */
#define LOG_ROLLBACK_NONE           0          /*  Set rollback size to 0 will not rollback  */ 
#define DBG_LOG_FILE                "console"  /*   Debug mode log file is console */

/*
 * #define EVENT_LOG_DEBUG 0
 * #define EVENT_LOG_MSG   1
 * #define EVENT_LOG_WARN  2
 * #define EVENT_LOG_ERR   3
 */
static int            loglevel = EVENT_LOG_MSG;
static FILE           *logfp = NULL;
static unsigned long  log_rollback_size = LOG_ROLLBACK_NONE;

int log_init (char *logfile, int size, int level);
static void logfn_cb ( int serverity, const char *msg);

/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{

    log_init("./test.log", 4, EVENT_LOG_DEBUG);
    event_set_log_callback(logfn_cb);

    _event_debugx("Far too many %s (%d)", "wombats", 99);
    event_msgx("Far too many %s (%d)", "wombats", 99);
    event_warn("Far too many %s (%d)", "wombats", 99);
    event_err(-1, "Far too many %s (%d)", "wombats", 99);

    return 0;
} /* ----- End of main() ----- */


/**************************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 *************************************************************************************/
int log_init (char *logfile, int size, int level)
{
    const char        *filemode = "a+";

    if(!logfile || size<0 || level<0)
        return -1;

    loglevel = level>=EVENT_LOG_ERR ? EVENT_LOG_ERR : level;
    log_rollback_size = size <= 0 ? LOG_ROLLBACK_NONE : size*1024;    /*  Unit KiB  */

    if (!strcmp(logfile, DBG_LOG_FILE))
    {
        logfp = stderr;
        log_rollback_size = LOG_ROLLBACK_NONE;
        return 0;
    }

    logfp = fopen(logfile, filemode);
    if( !logfp )
    {
        fprintf(stderr, "Open log file \"%s\" in %s failure\n", logfile, filemode);
        return 0;
    }

    return 0;
} /* ----- End of ev_init_log()  ----- */

static void logfn_cb ( int serverity, const char *msg)
{
    const char *s;

    if ( !logfp || serverity<loglevel )
    {
        return ;
    }

    switch ( serverity ) 
    {
        case EVENT_LOG_DEBUG:	
            s = "debug";
            break;

        case EVENT_LOG_MSG:	
            s = "msg";
            break;

        case EVENT_LOG_WARN:	
            s = "warn";
            break;

        case EVENT_LOG_ERR:	
            s = "error";
            break;

        default:	
            s = "?"; /* Should never comes here  */
            break;
    }				/* -----  end switch  ----- */

    fprintf(logfp, "[%s] %s\n", s, msg);

    return ;
}		/* -----  End of logfn_cb()  ----- */

