/********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue <guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  cp_logger.h
 *    Description:  This file is the linux infrastructural logger system library
 *
 *        Version:  1.0.0(08/08/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "08/08/2012 05:16:56 PM"
 *                 
 ********************************************************************************/

#ifndef __CP_LOG_H
#define __CP_LOG_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

#include <pthread.h>
#include <sys/types.h>
#include <sys/time.h>

#define LOG_VERSION_STR             "1.0.0"

#ifndef FILENAME_LEN
#define FILENAME_LEN                64
#endif

#define DEFAULT_LOGFILE             "cp_logger.log"
#define DBG_LOG_FILE                "console"  /*  Debug mode log file is console */ 

#define LOG_ROLLBACK_SIZE           512    /* Default rollback log size  */
#define LOG_ROLLBACK_NONE           0      /* Set rollback size to 0 will not rollback  */

#define DEFAULT_TIME_FORMAT         "%Y-%m-%d %H:%M:%S"
#define MAX_LOG_MESSAGE_LEN         0x1000

//#define DUMPLICATE_OUTPUT  /* Log to file and printf on console  */
#define LOG_FILE_LINE      /* Log the file and line */

enum
{
    LOG_LEVEL_DISB = 0,               /*  Disable "Debug" */
    LOG_LEVEL_FATAL,                  /*  Debug Level "Fatal" */
    LOG_LEVEL_ERROR,                  /*  Debug Level "ERROR" */
    LOG_LEVEL_WARN,                   /*  Debug Level "warnning" */
    LOG_LEVEL_NRML,                   /*  Debug Level "Normal" */
    LOG_LEVEL_DEBUG,                  /*  Debug Level "Debug" */
    LOG_LEVEL_INFO,                   /*  Debug Level "Information" */
    LOG_LEVEL_TRACE,                  /*  Debug Level "Trace" */
    LOG_LEVEL_MAX,
};

#define CP_LOGGER_MALLOC              1<<0
#define CP_LOGGER_ARGUMENT            0<<0

#define CP_LOGGER_CONSOLE             1<<1
#define CP_LOGGER_FILE                0<<1

#define CP_LOGGER_LEVEL_OPT           1<<2 /*  The log level is sepcified by the command option */
typedef struct _cp_logger
{
    unsigned char      flag;  /* This logger pointer is malloc() or passed by argument */
    char               file[FILENAME_LEN];
    int                level;
    int                size;

    FILE               *fp;
} cp_logger;

extern char *log_str[];

extern cp_logger *cp_log_init(cp_logger *log, char *filename, int level, int log_size);
extern int  cp_log_open(void);
extern void cp_log_set_time_format(char *time_format);
extern int  cp_log_reopen(void);
extern void cp_log_term(void);
extern void cp_log_raw(const char *fmt, ...);
extern void cp_log(int level, char *fmt, ...);
extern void cp_log_line(int level, char *file, int line, char *fmt, ...);
extern void cp_log_str(int level, const char *msg);

extern void cp_log_dump(int level, char *buf, int len);

#ifdef LOG_FILE_LINE
#define log_trace(fmt, ...) cp_log_line(LOG_LEVEL_TRACE, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...)  cp_log_line(LOG_LEVEL_INFO,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_dbg(fmt, ...)   cp_log_line(LOG_LEVEL_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_nrml(fmt, ...)  cp_log_line(LOG_LEVEL_NRML,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_warn(fmt, ...)  cp_log_line(LOG_LEVEL_WARN,  __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_err(fmt, ...)   cp_log_line(LOG_LEVEL_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define log_fatal(fmt, ...) cp_log_line(LOG_LEVEL_FATAL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define log_trace(fmt, ...) cp_log(LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...) cp_log(LOG_LEVEL_INFO, fmt, ##__VA_ARGS__)
#define log_dbg(fmt, ...) cp_log(LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)
#define log_nrml(fmt, ...) cp_log(LOG_LEVEL_NRML, fmt, ##__VA_ARGS__)
#define log_warn(fmt, ...) cp_log(LOG_LEVEL_WARN, fmt, ##__VA_ARGS__)
#define log_err(fmt, ...) cp_log(LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)
#define log_fatal(fmt, ...) cp_log(LOG_LEVEL_FATAL, fmt, ##__VA_ARGS__)
#endif


#endif /* __CP_LOG_H  */
