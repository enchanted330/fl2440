/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue <guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  test_logger.c
 *    Description:  This is the linux logger system test code.
 *                 
 *        Version:  1.0.0(08/08/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "08/08/2012 06:51:40 PM"
 *                 
 ********************************************************************************/

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <libgen.h>
#include "cp_logger.h"


/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    char    buf[30]="Hello World!\n";
    char    file[FILENAME_LEN];

    snprintf(file, FILENAME_LEN, "%s.log", basename(argv[0]) );

    //if (! cp_log_init(NULL, DBG_LOG_FILE, LOG_LEVEL_NRML, LOG_ROLLBACK_NONE) )
    //if (! cp_log_init(NULL, DEFAULT_LOGFILE, LOG_LEVEL_MAX, LOG_ROLLBACK_SIZE) )
    if(! cp_log_init(NULL, file, LOG_LEVEL_MAX, 12) || cp_log_open() )
        return 0;


    while(1)
    {
        log_info("1connection.\n");
        log_dbg("2connection.\n");
        log_nrml("3connection.\n");
        log_warn("4connection.\n");
        log_err("5connection.\n");
        log_fatal("6connection.\n");

        cp_log_dump(LOG_LEVEL_DEBUG, buf, 30);

        sleep(1);
    } 
    
    cp_log_term();

    return 0;
} /* ----- End of main() ----- */

