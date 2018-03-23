/********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  cp_fds.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(10/25/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "10/25/2012 04:55:37 PM"
 *                 
 ********************************************************************************/

#ifndef __CP_FDS_H
#define __CP_FDS_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include <sys/epoll.h>

#include "cp_klist.h"
#include "cp_logger.h"
#include "cp_sock.h"
#include "cp_queue.h"
#include "cp_array.h"

#define CP_MAX_EVENT_ONCE    128
#define CP_DEF_MAX_EVENTS    1024
#define CP_DEF_FDS_TIMEOUT   10

#define IN_EVENT_LIST        0
#define IN_TASK_LIST         1

#define FLAG_FDS_INIT        (1<<0)
#define FLAG_FDS_MALLOC      (1<<1)

struct _CP_SOCK;
struct _CP_FDS;

typedef struct _CP_FDS
{
    unsigned char         flag;         /* Refer to follow definition  */
    int                   epfd;         /* the epoll_create() returns file description */ 
    int                   timeout;      /* epoll_wait timeout value  */
    int                   max_event;    /* Maximum monitor FD number */
    int                   event_cnt;    /* current socket regist in epoll */

    struct list_head      server_list;  /* a list keep all the listen sockets */ 
    struct list_head      client_list;  /* a list keep all the connect sockets */ 
    CP_QUEUE              *event_queue; /* a queue keep all these sockets get event happened */
    CP_ARRAY              *task_array;  /* an array keep all these pending/or other special task socket */ 

  /*  flag definitions
   *  0 0 0 0 0 1 1 1
   *  |       | | | |______ Initial or not  - 0: No          1: Yes
   *  |       | | |________ Malloc or not   - 0: Variable,   1: Malloc
   *  |       | |__________ Stop or not     - 0: Not stop    1: Stop
   *  |       |____________ Reserved
   *  |____________________ Reserved
   */
} CP_FDS;

/* Initialze $fds context, if $fds is NULL, then malloc it */
extern CP_FDS *cp_fds_init(CP_FDS *fds, int maxevents, int timeout);
/* Terminate $fds context, if $fds malloced, then free it */
extern void cp_fds_term(CP_FDS *fds);
#define cp_fds_term_clear(fds) {cp_fds_term(fds); fds=NULL;}

/* epoll_ctl() to add this socket to epoll   */
int cp_add_epoll_event(struct _CP_SOCK *sock);
/* epoll_ctl() to mod this socket in epoll */
int cp_mod_epoll_event(struct _CP_SOCK *sock, int event);
/* epoll_ctl() to del this socket from epoll */
void cp_del_epoll_event(struct _CP_SOCK *sock);


/* add the socket into the socket registry list */ 
int cp_fds_add_sock_registry(struct _CP_SOCK *sock);
/* remove the socket from the socket registry list */ 
void cp_fds_del_sock_registry(struct _CP_SOCK *sock);

/* add the socket into task list */
int cp_fds_add_sock_task(struct _CP_SOCK *sock);
/* remove the socket from event or task list */
void cp_fds_del_sock_task(struct _CP_SOCK *sock);
/* list all the socket from task list */
void cp_fds_list_sock_task(CP_FDS *fds);

void *cp_fds_sock_enqueue(struct _CP_SOCK *sock);
void *cp_fds_sock_rmqueue(struct _CP_SOCK *sock);
void *cp_fds_sock_dequeue(CP_FDS *fds);

/* remove the socket from task and registry list and destroy it */
void cp_fds_destroy_sock(struct _CP_SOCK *sock);
#define cp_fds_destroy_sock_clear(sock) {cp_fds_destroy_sock(sock); sock=NULL;}

/* epoll_wait detect all these socket get event and add it to task list */
extern int cp_fds_detect_event(CP_FDS *fds);
/* process all these socket get event and add it to task list */
extern void cp_fds_proc_event(CP_FDS *fds);
/* Detect all these socket get timeout and add it to task list */
extern void cp_sock_detect_timeout(CP_FDS *fds);


extern void cp_sock_term_server(struct _CP_SOCK *serv_sock);
extern void cp_sock_term_all_server(CP_FDS *fds);
extern void cp_sock_term_all_client(CP_FDS *fds);
extern void cp_sock_term_all_task(CP_FDS *fds);

#endif /* __CP_FDS_H */
