/********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  cp_sock.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(10/26/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "10/26/2012 01:21:38 PM"
 *                 
 ********************************************************************************/
#ifndef __CP_SOCK_H
#define __CP_SOCK_H

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/un.h>
#include <poll.h>
#include <errno.h> 

#include <sys/types.h>
#include <sys/socket.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "cp_klist.h"
#include "cp_string.h"
#include "cp_fds.h"


#define CP_DEF_RCVBUF_SIZE                2048
#define CP_DEF_SNDBUF_SIZE                2048

#define CP_SOCK_RCVBUF                    2048
#define CP_SOCK_SNDBUF                    2048

#define CP_SOCK_DEF_IDLE_TIMEOUT          120*1000 /* 2 minutes */
#define CP_SOCK_DEF_MSG_TIMEOUT           120*1000 /* 2 minutes */

#define CP_SOCK_KEEPINTVL                 600
#define CP_SOCK_KEEPCNT                   3

#define CP_SOCK_MODE_NONE                 0
#define CP_SOCK_MODE_ACCEPT               1
#define CP_SOCK_MODE_CONNECT              2
#define CP_SOCK_MODE_LISTEN               3

enum 
{
    CP_SOCK_EVENT_READ         =          EPOLLIN,   /* compatilbe with EPOLLIN  */
    CP_SOCK_EVENT_WRITE        =          EPOLLOUT,  /* compatilbe with EPOLLOUT */
    CP_SOCK_EVENT_ERROR        =          EPOLLERR,  /* compatilbe with EPOLLERR  */
    CP_SOCK_EVENT_HUP          =          EPOLLHUP,  /* compatilbe with EPOLLHUP */
    CP_SOCK_EVENT_IDLE_TIMEOUT =          (1<<16),   /* compatilbe with EPOLL reserved*/  
    CP_SOCK_EVENT_MSG_TIMEOUT  =          (1<<17),   /* compatible with EPOLL reserved*/
};

struct _CP_SOCK;
struct _CP_FDS;

typedef void (*PRIVT_FREE)(void);
typedef int (*CP_SOCK_EVENT_CALLBACK)(struct _CP_SOCK *sock);
typedef struct _CP_SOCK_CALLBACK
{
    CP_SOCK_EVENT_CALLBACK  error;
    CP_SOCK_EVENT_CALLBACK  read;
    CP_SOCK_EVENT_CALLBACK  write;
} CP_SOCK_CALLBACK; 


#define FLAG_SOCK_INIT             (1<<0)
#define FLAG_SOCK_MALLOC           (1<<1)
#define FLAG_SOCK_EPOLLED          (1<<2)
#define FLAG_SOCK_REGISTRY         (1<<3)
#define FLAG_SOCK_INQUEUE          (1<<4)
#define FLAG_SOCK_TASKED           (1<<5)

#define SOCK_STAT_INIT             0
#define SOCK_STAT_CONNECTING       1
#define SOCK_STAT_CONNECTED        2
#define SOCK_STAT_LISTENED         2
#define SOCK_STAT_ACCEPTED         2
#define SOCK_STAT_ALREADY          2
#define SOCK_STAT_DISCONNECT       3

typedef struct _CP_SOCK
{
    int                      fd;          /* the socket file description */
    int                      mode;        /* socket work mode: listen, accept, connect */
    unsigned short           flag;        /* Refer to the follow definition  */
    unsigned char            status;      /* current socket status */

    unsigned long            actv_time;     /* The last time get event(send/recv) data in this socket */
    unsigned long            msg_time;      /* The last time get incoming data in this socket */
    unsigned long            idle_timeout;  /* this is set to the socket idle timeout value, when timeout will disconnect */
    unsigned long            msg_timeout;   /* this is set to the socket recv data timeout value,  */

    int                      keepintvl;   /* heartbeat(keepalive) detect interval */
    int                      keepcnt;     /* heartbeat(keepalive) count */
    int                      rsize;       /* ioctl() set socket recv buffer size  */
    int                      ssize;       /* ioctl() set socket send buffer size  */

    struct epoll_event       event;       /* epoll_wait returned event */

    cp_string                *sbuf;       /* send buffer */
    cp_string                *rbuf;       /* receive buffer  */

    struct sockaddr          saddr;       /* sockaddr for connect */
    char                     laddr[16];   /* local IP address*/
    int                      lport;       /* local port */
    char                     raddr[16];   /* remote IP address  */
    int                      rport;       /* remote port */

    struct _CP_FDS           *fds;        /* epoll fds contex */
    CP_SOCK_EVENT_CALLBACK   cbfunc;      /* event callback function */
    CP_SOCK_EVENT_CALLBACK   client_func; /* client data callback function, only server socket use it */
    struct _CP_SOCK          *serv_sock;  /* Accept socket used to point to the server socket */

    int                      max_client;  /* max clients, only listen socket use it*/
    int                      accept_cnt;  /* current clients number, only listen socket use it*/
    struct list_head         accept_list; /* all the accept client list head, only listen socket use it */

    void                     *privt;      /* socket private data */
    PRIVT_FREE               privt_free;  /* private data free function if it's malloc */

    int                      index;       /* The index member position in the task array */
    struct list_head         rlist;       /* The list_head member position just for the registry list */

  /*  flag definitions
   *  1 1 1 1 1 1 1 1
   *  | | | | | | | |______ Initial    flag  - 0: no            1: yes
   *  | | | | | | |________ Malloc     flag  - 0: variable,     1: malloc
   *  | | | | | |__________ registry   flag  - 0: not in,       1: in the registry list
   *  | | | | |____________ epolled    flag  - 0: not in,       1: put in epoll
   *  | | | |______________ in queue   flag  - 0: not in,       1: put in queue
   *  | | |________________ task       flag  - 0: not in,       1: in the task list
   *  | |__________________ reserved
   *  |____________________ reserved
   */
} CP_SOCK;


static void inline cp_sock_set_idle_timeout(CP_SOCK *sock, unsigned long timeout)
{
    sock->idle_timeout = timeout;
}

static void inline cp_sock_set_msg_timeout(CP_SOCK *sock, unsigned long timeout)
{
    sock->msg_timeout = timeout;
}

static void inline cp_sock_set_private_data(CP_SOCK *sock, void *data, PRIVT_FREE free)
{
    sock->privt = data;
    sock->privt_free = free;
}

extern CP_SOCK *cp_sock_init(CP_SOCK *sock, unsigned int rsize, unsigned int ssize, int keepintvl, int keepcnt);
extern int cp_sock_close(CP_SOCK *sock);
extern void cp_sock_term(CP_SOCK *sock);
#define cp_sock_term_clear(sock) {cp_sock_term(sock); sock=NULL;}

extern int cp_sock_listen(CP_SOCK *sock, char *laddr, int lport, int backlog);
extern int cp_sock_connect(CP_SOCK *sock, char *raddr, int rport, int lport);
extern int cp_sock_accept(CP_SOCK *serv_sock, CP_SOCK *new_sock);

extern int cp_sock_recv(CP_SOCK *sock);
extern int cp_sock_send(CP_SOCK *sock);

extern int cp_sock_set_reuseaddr(int sockfd);
extern int cp_sock_set_keepalive(int sockfd, int keepintvl, int keepcnt);
extern int cp_sock_set_nonblock(int sockfd);
extern int cp_sock_set_buffer(int sockfd, int rsize, int ssize);

#endif /* __CP_SOCK_H */
