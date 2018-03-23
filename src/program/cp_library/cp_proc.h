/********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  cp_proc.h
 *    Description:  This head file is for Linux process API
 *
 *        Version:  1.0.0(11/06/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "11/06/2012 09:21:33 PM"
 *                 
 ********************************************************************************/

#ifndef __CP_PROC_H
#define __CP_PROC_H

#include <signal.h>

#define PID_ASCII_SIZE  11

typedef struct __CP_PROC_SIG
{
    int       signal;
    unsigned  stop;     /* 0: Not term  1: Stop  */
}  CP_PROC_SIG;

typedef void *(THREAD_BODY) (void *thread_arg);

extern CP_PROC_SIG     g_cp_signal;
extern void cp_install_proc_signal(void);

extern void daemonize(int nochdir, int noclose);
extern int record_daemon_pid(const char *pid_file);
extern pid_t get_daemon_pid(const char *pid_file);
extern int check_daemon_running(const char *pid_file);
extern int set_daemon_running(const char *pid_file);

extern void exec_system_cmd(const char *format, ...);

extern int thread_start(pthread_t * thread_id, THREAD_BODY * thread_workbody, void *thread_arg);

#endif
