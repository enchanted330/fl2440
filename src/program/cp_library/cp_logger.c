/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue <guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  cp_log.c
 *    Description:  This file is the linux infrastructural logger system library
 *                 
 *        Version:  1.0.0(08/08/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "08/08/2012 04:24:01 PM"
 *                 
 ********************************************************************************/

#include "cp_logger.h"
#include "cp_common.h"

#define PRECISE_TIME_FACTOR 1000

static unsigned long log_rollback_size = LOG_ROLLBACK_NONE;

static cp_logger *logger = NULL;

char *log_str[LOG_LEVEL_MAX + 1] = { "", "F", "E", "W", "N", "D", "I", "T", "M" };

static char *cp_time_format = DEFAULT_TIME_FORMAT;

void cp_log_set_time_format(char *time_format)
{
    cp_time_format = time_format;
}

static void cp_log_default_signal_handler(int sig)
{
    if(!logger)
        return ;

    if (sig == SIGHUP)
    {
        signal(SIGHUP, cp_log_default_signal_handler);
        log_fatal("SIGHUP received - reopenning log file [%s]", logger->file);
        cp_log_reopen();
    }
}

static void log_banner(char *prefix)
{
    if(!logger)
        return ;

    fprintf(logger->fp, "%s log \"%s\" on level [%s] size [%lu], log system version %s\n",
            prefix, logger->file, log_str[logger->level], log_rollback_size / 1024, LOG_VERSION_STR);
#ifdef LOG_FILE_LINE
    fprintf(logger->fp, " [Date]    [Time]   [Level] [PID/TID] [File/Line]  [Content]\n");
#else
    fprintf(logger->fp, " [Date]    [Time]   [Level] [PID/TID] [Content]\n");
#endif
    fprintf(logger->fp, "-------------------------------------------------------------\n");
}

static void check_and_rollback(void)
{
    if(!logger)
        return ;

    if (log_rollback_size != LOG_ROLLBACK_NONE)
    {
        long _curOffset = ftell(logger->fp);

        if ((_curOffset != -1) && (_curOffset >= log_rollback_size))
        {
            char cmd[512];

            snprintf(cmd, sizeof(cmd), "cp -f %s %s.roll", logger->file, logger->file);
            system(cmd);

            if (-1 == fseek(logger->fp, 0L, SEEK_SET))
                fprintf(logger->fp, "log rollback fseek failed \n");

            rewind(logger->fp);

            truncate(logger->file, 0);
            log_banner("Already rollback");
        }
    }
}

cp_logger *cp_log_init(cp_logger *log, char *filename, int level, int log_size)
{
    if(NULL == log)
    {
        logger = t_malloc(sizeof(cp_logger));
        memset(logger, 0, sizeof(cp_logger));
        logger->flag |= CP_LOGGER_MALLOC; 
    }
    else
    {
        logger = log;
        memset(logger, 0, sizeof(cp_logger));
        logger->flag |= CP_LOGGER_ARGUMENT; 
    }

    if(NULL == logger)
    {
        return NULL;
    }

    strncpy(logger->file, filename, FILENAME_LEN); 
    logger->level = level;
    logger->size = log_size; 

    return logger;
}

int cp_log_open(void)
{
    struct sigaction act;
    char *filemode;

    if(!logger)
    {
        return -1;
    }

    log_rollback_size = logger->size <= 0 ? LOG_ROLLBACK_NONE : logger->size*1024;    /* Unit KiB */
        
    if ('\0' == logger->file)
        return -1;

    if (!strcmp(logger->file, DBG_LOG_FILE))
    {
        logger->fp = stderr;
        log_rollback_size = LOG_ROLLBACK_NONE;
        logger->flag |= CP_LOGGER_CONSOLE;
        goto OUT;
    }

    //filemode = (log_rollback_size==LOG_ROLLBACK_NONE) ? "a+" : "w+";
    filemode = "a+";

    logger->fp = fopen(logger->file, filemode);
    if (NULL == logger->fp)
    {
        fprintf(stderr, "Open log file \"%s\" in %s failure\n", logger->file, filemode);
        return -2;
    }

    act.sa_handler = cp_log_default_signal_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGHUP, &act, NULL);

  OUT:
    log_banner("Initialize");

    return 0;
}

void cp_log_close(void)
{
    if (!logger || !logger->fp )
        return;

    log_banner("\nTerminate");
    cp_log_raw("\n\n\n\n");

    fflush(logger->fp);

    fclose(logger->fp);
    logger->fp = NULL;

    return ;
}

int cp_log_reopen(void)
{
    int rc = 0;
    char *filemode;

    if( !logger )
        return -1;

    if (logger->flag & CP_LOGGER_CONSOLE )
    {
        fflush(logger->fp);
        logger->fp = stderr;
        return 0;
    }

    if (logger->fp)
    {
        cp_log_close();
        //filemode = log_rollback_size == LOG_ROLLBACK_NONE ? "a+" : "w+";
        filemode = "a+";
        logger->fp = fopen(logger->file, filemode); 
        
        if (logger->fp == NULL)
            rc = -2;
    }
    else
    {
        rc = -3;
    }

    if (!rc)
    {
        log_banner("\nReopen");
    }
    return rc;
}

void cp_log_term(void)
{
    if(!logger)
        return ;

    cp_log_close();

    if (logger->flag & CP_LOGGER_MALLOC )
    {
        t_free(logger);
    }
    logger = NULL;
}

void cp_log_raw(const char *fmt, ...)
{
    va_list argp;

    if (!logger || !logger->fp)
        return;

    check_and_rollback();

    va_start(argp, fmt);
    vfprintf(logger->fp, fmt, argp);
    va_end(argp);
}

static void cp_printout(char *level, char *fmt, va_list argp)
{
    char buf[MAX_LOG_MESSAGE_LEN];
    struct tm *local;
    struct timeval now;
    char timestr[256];

    if(!logger)
        return ;

    pthread_t tid;

    check_and_rollback();

#ifdef MULTHREADS
    tid = pthread_self();
#else 
    tid = getpid();
#endif

    gettimeofday(&now, NULL);
    local = localtime(&now.tv_sec);

    strftime(timestr, 256, cp_time_format, local);
    vsnprintf(buf, MAX_LOG_MESSAGE_LEN, fmt, argp);

#ifdef DUMPLICATE_OUTPUT
    printf("%s.%03ld [%s] [%06lu]: %s",
           timestr, now.tv_usec / PRECISE_TIME_FACTOR, level, tid, buf);
#endif

    if (logger->fp)
        fprintf(logger->fp, "%s.%03ld [%s] [%06lu]: %s", timestr, now.tv_usec / PRECISE_TIME_FACTOR,
                level, tid, buf);

    if (logger->fp)
        fflush(logger->fp);
}

static void cp_printout_line(char *level, char *fmt, char *file, int line, va_list argp)
{
    char buf[MAX_LOG_MESSAGE_LEN];
    struct tm *local;
    struct timeval now;
    char timestr[256];

    if(!logger)
        return ;

    pthread_t tid;

    check_and_rollback();

#ifdef MULTHREADS
    tid = pthread_self();
#else 
    tid = getpid();
#endif

    gettimeofday(&now, NULL);
    local = localtime(&now.tv_sec);

    strftime(timestr, 256, cp_time_format, local);
    vsnprintf(buf, MAX_LOG_MESSAGE_LEN, fmt, argp);

#ifdef DUMPLICATE_OUTPUT
    printf("%s.%03ld [%s] [%06lu] (%s [%04d]) : %s",
           timestr, now.tv_usec / PRECISE_TIME_FACTOR, level, tid, file, line, buf);
#endif

    if (logger->fp)
        fprintf(logger->fp, "%s.%03ld [%s] [%06lu] (%s [%04d]) : %s",
                timestr, now.tv_usec / PRECISE_TIME_FACTOR, level, tid, file, line, buf);

    if (logger->fp)
        fflush(logger->fp);
}

void cp_log(int level, char *fmt, ...)
{
    va_list argp;

    if (!logger || level>logger->level)
        return;

    va_start(argp, fmt);
    cp_printout(log_str[level], fmt, argp);
    va_end(argp);
}

void cp_log_line(int level, char *file, int line, char *fmt, ...)
{
    va_list argp;

    if (!logger || level>logger->level)
        return;

    va_start(argp, fmt);
    cp_printout_line(log_str[level], fmt, file, line, argp);

    va_end(argp);
}

#define LINELEN 81
#define CHARS_PER_LINE 16
static char *print_char =
    "                "
    "                "
    " !\"#$%&'()*+,-./"
    "0123456789:;<=>?"
    "@ABCDEFGHIJKLMNO"
    "PQRSTUVWXYZ[\\]^_"
    "`abcdefghijklmno"
    "pqrstuvwxyz{|}~ "
    "                "
    "                "
    " ???????????????"
    "????????????????" 
    "????????????????" 
    "????????????????" 
    "????????????????" 
    "????????????????";

void cp_log_dump(int level, char *buf, int len)
{
    int rc;
    int idx;
    char prn[LINELEN];
    char lit[CHARS_PER_LINE + 2];
    char hc[4];
    short line_done = 1;

    if (!logger || level>logger->level)
        return;

    rc = len;
    idx = 0;
    lit[CHARS_PER_LINE] = '\0';

    while (rc > 0)
    {
        if (line_done)
            snprintf(prn, LINELEN, "%08X: ", idx);

        do
        {
            unsigned char c = buf[idx];
            snprintf(hc, 4, "%02X ", c);
            strncat(prn, hc, LINELEN);

            lit[idx % CHARS_PER_LINE] = print_char[c];
        }
        while (--rc > 0 && (++idx % CHARS_PER_LINE != 0));

        line_done = (idx % CHARS_PER_LINE) == 0;
        if (line_done)
        {
#ifdef DUMPLICATE_OUTPUT
            printf("%s  %s\n", prn, lit);
#endif
            if (logger->fp)
                fprintf(logger->fp, "%s  %s\n", prn, lit);
        }
    }

    if (!line_done)
    {
        int ldx = idx % CHARS_PER_LINE;
        lit[ldx++] = print_char[(int)buf[idx]];
        lit[ldx] = '\0';

        while ((++idx % CHARS_PER_LINE) != 0)
            strncat(prn, "   ", LINELEN);

#ifdef DUMPLICATE_OUTPUT
        printf("%s  %s\n", prn, lit);
#endif
        if (logger->fp)
            fprintf(logger->fp, "%s  %s\n", prn, lit);

    }
}
