/*********************************************************************************
 *      Copyright:  (C) 2014 Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  ev_cli.c
 *    Description:  This file shows how to use libevent to write a client program, 
 *                  this sample program will connect to remote host server and send
 *                  data to it.
 *                 
 *        Version:  1.0.0(02/10/2014)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "02/10/2014 10:20:13 AM"
 *                 
 ********************************************************************************/


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <signal.h>
#include <libgen.h>
#include <errno.h>

#include <event2/event.h>
#include <event2/util.h>
#include <event2/event_struct.h>
#include "version.h"


typedef struct timer_ctx_s 
{ 
    struct timeval            tv; 
    struct event              ev;
} timer_ctx_t;  /*---  end of struct timer_ctx_s  ---*/


struct timeval                lasttime;

void timeout_cb(int fd,short _event,void *arg)
{
    struct timeval            newtime, difference;
    double                    elapsed; 
    timer_ctx_t               *timer_ctx = (timer_ctx_t *)arg;

    evutil_gettimeofday(&newtime, NULL);
    evutil_timersub(&newtime, &lasttime, &difference);
    elapsed = difference.tv_sec + (difference.tv_usec / 1.0e6);

    printf("timeout_cb called at %d: %.3f seconds elapsed.\n", (int)newtime.tv_sec, elapsed);
    lasttime = newtime;

    event_add( &(timer_ctx->ev), &(timer_ctx->tv) );
}


int main(int argc, char **argv)
{
    struct event_base         *base = NULL;
    timer_ctx_t               timer_ctx;

    base = event_base_new();
    if (!base)
    {
        printf("event_base_new() failure: %s\n", strerror(errno));
        return -1;
    }


    /*
     * @param ev an event struct to be modified
     * @param base the event base to which ev should be attached.
     * @param fd the file descriptor to be monitored
     * @param events desired events to monitor; can be EV_READ and/or EV_WRITE
     * @param callback callback function to be invoked when the event occurs
     * @param callback_arg an argument to be passed to the callback function
     * @return 0 if success, or -1 on invalid arguments.
     */
    event_assign(&(timer_ctx.ev), base, -1, EV_PERSIST, timeout_cb, (void*) &timer_ctx);

    evutil_timerclear( &(timer_ctx.tv) );
    timer_ctx.tv.tv_sec=3; 
    timer_ctx.tv.tv_usec=0;
    event_add(&(timer_ctx.ev),&(timer_ctx.tv));

    evutil_gettimeofday(&lasttime, NULL);

    event_base_dispatch(base);

    return 0; 
}

