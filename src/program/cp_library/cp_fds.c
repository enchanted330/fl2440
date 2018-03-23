/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  cp_fds.c
 *    Description:  This file is the linux epoll basic library
 *                 
 *        Version:  1.0.0(10/25/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "10/25/2012 04:55:30 PM"
 *                 
 ********************************************************************************/

#include <sys/resource.h>
#include "cp_fds.h"
#include "cp_sock.h"
#include "cp_time.h"
#include "cp_common.h"

/* set maximum file descriptor number that can be opened in this process */
int cp_fds_setrlimit(int maxfd)
{
    struct  rlimit    res;

    if(getrlimit(RLIMIT_NOFILE, &res) <0)
    {
        log_fatal("getrlimit failed: %s\n", strerror(errno));
        return -1;
    }

    if(res.rlim_cur < maxfd)
    {
        res.rlim_cur = maxfd;
        if(setrlimit(RLIMIT_NOFILE, &res) <0)
        {
            log_fatal("setrlimit failed: %s\n", strerror(errno));
            return -2;
        }
    }

    return 0;
}

/* Initialze $fds context, if $fds is NULL, then malloc it */
CP_FDS *cp_fds_init(CP_FDS *fds, int maxevents, int timeout)
{
    int          rv = 0;

    if(NULL != fds)
    { 
        memset(fds, 0, sizeof(*fds));
        fds->epfd = -1;
    }
    else
    {
        if( !(fds=(CP_FDS *)t_malloc(sizeof(*fds))) )
        {
            log_err("fds context malloc failed: %s\n", strerror(errno));
            return NULL;
        }
        else
        {
            log_trace("malloc fds with address [%p]\n", fds);
            memset(fds, 0, sizeof(*fds));
            fds->epfd = -1;
            fds->flag |= FLAG_FDS_MALLOC;
        }
    }

    /* set maximum file descriptor number that can be opened in this process */
    maxevents = maxevents<CP_DEF_MAX_EVENTS ? maxevents : CP_DEF_MAX_EVENTS;
    if( cp_fds_setrlimit(maxevents) )
    {
        rv = -2;
        goto cleanup;
    }

    /* Create epoll file description */
    fds->epfd = epoll_create(maxevents);
    if(fds->epfd < 0)
    {
        rv = -3;
        log_fatal("epoll_create failed: %s\n", strerror(errno));
        goto cleanup;
    }
    log_trace("Open epoll file description [%d]\n", fds->epfd);

    /* Initialise the server/client/task list */
    INIT_LIST_HEAD(&fds->server_list);
    INIT_LIST_HEAD(&fds->client_list);
    fds->task_array = cp_array_init(NULL, maxevents);
    fds->event_queue = cp_queue_init(NULL, maxevents);

    fds->timeout = timeout<0 ? -1 : timeout;
    fds->max_event = maxevents;
    fds->flag |= FLAG_FDS_INIT;

cleanup:
    if(rv)
    {
        log_err("Initialise fds contex failed\n");
        cp_fds_term(fds);
        return NULL;
    }

    log_nrml("Initialise fds contex [%p] ok\n", fds);
    return fds;
}

/* Terminate $fds context, if $fds malloced, then free it */
void cp_fds_term(CP_FDS *fds)
{
    log_dbg("terminate epoll fds contex now\n");

    if(!fds)
    {
        log_err("Invalude input arguments\n");
        return;
    }

    if(fds->epfd >= 0)
    {
        log_dbg("Close epoll file description [%d]\n", fds->epfd); 
        close(fds->epfd); 
    }

    if(fds->task_array)
    {
        cp_array_term(fds->task_array);
    }

    if(fds->event_queue)
    {
        cp_queue_destroy(fds->event_queue);
    }

    if(fds->flag& FLAG_FDS_MALLOC)
    {
        t_free(fds);
    }

    log_nrml("Terminate epoll fds contex ok.\n");

    return ;
}

/* epoll_ctl() to add this socket to epoll   */
int cp_add_epoll_event(CP_SOCK *sock)
{
    int                     rv = 0;
    CP_FDS                  *fds;

    if( !sock || !(fds=sock->fds) )
    {
        log_err("Invalude input arguments\n");
        return -1;
    }

    if( sock->flag&FLAG_SOCK_EPOLLED )
        return 0; /* already put in epoll */

    if(fds->event_cnt > fds->max_event)
    {
        log_fatal("No more epoll event space [%d/%d] for socket[%d]\n", 
                fds->event_cnt, fds->max_event, sock->fd);
        return -2;
    }

    memset(&sock->event, 0, sizeof(sock->event));
    sock->event.events = EPOLLIN;
    sock->event.data.ptr = sock;

    if ( epoll_ctl (fds->epfd, EPOLL_CTL_ADD, sock->fd, &sock->event) < 0) 
    {
        if(EEXIST == errno)
        {
            log_warn("socket[%d] already registe in epoll\n", sock->fd);
            goto cleanup;
        }
        else
        {
            log_err("socket[%d] registe in epoll failed: %s\n", sock->fd, strerror(errno));
            rv = -3;
            goto cleanup;
        }
    }
    
    fds->event_cnt++;
    sock->flag |= FLAG_SOCK_EPOLLED;

cleanup:
    if(rv)
        log_err("add socket [%d] to epoll event failed\n", sock->fd);
    else
        log_dbg("add socket [%d] to epoll event ok\n", sock->fd);

    return rv;
}


/* epoll_ctl() to mod this socket in epoll */
int cp_mod_epoll_event(CP_SOCK *sock, int event)
{
    CP_FDS                  *fds;

    if( !sock || !(fds=sock->fds) )
    {
        log_err("Invalude input arguments\n");
        return -1;
    }

    if( !(sock->flag&FLAG_SOCK_EPOLLED) )
    {
        return -2;  /* not in epoll  */
    }

    sock->event.events = event;
    if ( epoll_ctl (fds->epfd, EPOLL_CTL_MOD, sock->fd, &sock->event) < 0) 
    {
        log_err("modidfy socket [%d] epoll event failed: %s\n", sock->fd, strerror(errno));
        return -3;
    }
    
    log_dbg("modidfy socket [%d] epoll event ok\n", sock->fd);

    return 0;
}


void cp_del_epoll_event(CP_SOCK *sock)
{
    CP_FDS                  *fds;

    if( !sock || !(fds=sock->fds) )
    {
        log_err("Invalude input arguments\n");
        return;
    }

    if( !(sock->flag&FLAG_SOCK_EPOLLED) )
        return; /* not in epoll  */

    sock->flag &= ~FLAG_SOCK_EPOLLED;
    epoll_ctl(fds->epfd, EPOLL_CTL_DEL, sock->fd, NULL);
    fds->event_cnt--;

    log_dbg("remove socket [%d] from epoll event ok\n", sock->fd);
    return ;
}

/* add the listen/connect socket into server_list/client_list */
int cp_fds_add_sock_registry(CP_SOCK *sock)
{
    CP_FDS                  *fds;

    if( !sock || !(fds=sock->fds) )
    {
        log_err("Invalude input arguments\n");
        return -1;
    }

    if( sock->flag&FLAG_SOCK_REGISTRY )
        return 0;  /* already in registry list  */

    if(CP_SOCK_MODE_CONNECT == sock->mode)
    {
        log_dbg("regist socket [%p] on CONNECT mode into client list ok\n", sock);
        list_add_tail(&sock->rlist, &fds->client_list);
    }
    else if(CP_SOCK_MODE_LISTEN == sock->mode)
    {
        log_dbg("regist socket [%p] on LISTEN mode into server list ok\n", sock);
        list_add_tail(&sock->rlist, &fds->server_list);
    }
    else if(CP_SOCK_MODE_ACCEPT==sock->mode && sock->serv_sock)
    {
        log_dbg("regist socket [%p] on ACCEPT mode into server list ok\n", sock);
        list_add_tail(&sock->rlist, &sock->serv_sock->accept_list);
        sock->serv_sock->accept_cnt++;
    }
    else
    {
        log_err("regist socket [%p] on mode %d into client/server list failed: Unsupport mode.\n", sock, sock->mode);
        return -2;
    }

    sock->flag |= FLAG_SOCK_REGISTRY;
    return 0;
}

/*  remove the listen/connect socket from server_list/client_list */
void cp_fds_del_sock_registry(CP_SOCK *sock)
{
    if(!sock || !(sock->flag&FLAG_SOCK_REGISTRY) )
        return;  /* not in registry list  */

    if(CP_SOCK_MODE_ACCEPT==sock->mode && sock->serv_sock)
        sock->serv_sock->accept_cnt--;

    log_dbg("remove socket [%d] from socket registry list ok\n", sock->fd);
    list_del(&sock->rlist);
    sock->flag &= ~FLAG_SOCK_REGISTRY;

    return ;
}


/* Add a socket in task list */
int cp_fds_add_sock_task(CP_SOCK *sock)
{
    CP_FDS                  *fds;
    int                     rv;

    if( !sock || !(fds=sock->fds) )
    {
        log_err("Invalude input arguments\n");
        return -1;
    }

    if( sock->flag&FLAG_SOCK_TASKED )
        return 0; /* already in a list */

    log_info("add socket [%d] into task list\n", sock->fd);
    if( (rv=cp_array_add(fds->task_array, sock)) >=0 )
    {
        sock->index = rv;
        sock->flag |= FLAG_SOCK_TASKED;
        return 0;
    }

    return -2;
}

void cp_fds_del_sock_task(CP_SOCK *sock)
{
    if(!sock || !(sock->flag&FLAG_SOCK_TASKED))
        return ;
 
    log_info("remove socket [%d:%p] from task list[%p] by index [%d]\n", sock->fd, sock, sock->fds->task_array, sock->index);

    if(sock->index >= 0)
        cp_array_rm_byindex(sock->fds->task_array, sock->index);
    else
        cp_array_rm_bydata(sock->fds->task_array, sock);

    sock->flag &= ~FLAG_SOCK_TASKED;
    sock->index = -1;
    memset(&sock->event, 0, sizeof(struct epoll_event));

    return ;
}

void *cp_fds_sock_enqueue(CP_SOCK *sock)
{
    if(!sock || sock->flag&FLAG_SOCK_INQUEUE || sock->flag&FLAG_SOCK_TASKED || !sock->fds)
        return NULL;

    sock->flag |= FLAG_SOCK_INQUEUE;
    return cp_enqueue(sock->fds->event_queue, sock);
}

void *cp_fds_sock_dequeue(CP_FDS *fds)
{
    CP_SOCK    *sock = NULL;

    if( !fds )
        return NULL;

    sock = cp_dequeue(fds->event_queue);
    sock->flag &= ~FLAG_SOCK_INQUEUE;

    return sock;
}

void *cp_fds_sock_rmqueue(CP_SOCK *sock)
{
    if(!sock || !(sock->flag&FLAG_SOCK_INQUEUE) || !sock->fds)
        return NULL;

    sock->flag &= ~FLAG_SOCK_INQUEUE;
    return cp_rmqueue(sock->fds->event_queue, sock);
}

void cp_fds_list_sock_task(CP_FDS *fds)
{
    int               i;
    CP_SOCK           *sock;

    if(!fds)
        return ;

    log_dbg("list all the socket in task list:\n");

    /* list all the socket from task list */
    cp_list_array_for_each(fds->task_array, i, sock)
    { 
        if(sock)
            log_dbg("socket[%d:%p] in task list\n", sock->fd, sock); 
    }

    return ;
}


void cp_fds_destroy_sock(CP_SOCK *sock)
{
    cp_fds_del_sock_task(sock);
    cp_fds_sock_rmqueue(sock);
    cp_del_epoll_event(sock);
    cp_fds_del_sock_registry(sock);
    cp_sock_term(sock);

    return ;
}


int cp_fds_detect_event(CP_FDS *fds)
{
    int                   i=0, nfds=0;
    CP_SOCK               *sock;
    struct epoll_event    evts[CP_MAX_EVENT_ONCE];

    if( !fds )
        return -1;

    nfds = epoll_wait(fds->epfd, evts, CP_MAX_EVENT_ONCE, fds->timeout);
    ///if(nfds<0 && errno!=EINTR)
    if(nfds<0)
    {
        log_fatal("epoll_wait failure: %s\n", strerror(errno));
        return -2;
    }

    if(nfds == 0)
        return 0;

    log_dbg("epoll_wait get [%d] events\n", nfds);

    for (i=0; i<nfds; i++) 
    {
        sock=(CP_SOCK *)evts[i].data.ptr;
        sock->event = evts[i];
        sock->actv_time = time_now();

        log_dbg("socket [%d] get event [0x%0x] and added to event queue ok\n", sock->fd, sock->event.events);
        if( SOCK_STAT_ALREADY==sock->status )
            cp_fds_sock_enqueue(sock);
    }

    return 0;
}

void cp_fds_proc_event(CP_FDS *fds)
{
    int                    i;
    CP_SOCK                *sock;
    int                    rv;

    if( !fds )
        return;

    //cp_fds_list_sock_task(fds);

    //log_trace("Start process the events....\n");
    
    /* Process all the socket in task list first, its urgent */
    cp_list_array_for_each(fds->task_array, i, sock)
    {
        if(sock)
        {
            if(sock->cbfunc)
            {
                log_dbg("Process socket [%d] in task list on event [%d]\n", sock->fd, sock->event.events);
                rv = sock->cbfunc(sock);
            }
            else
            { 
                log_err("Process Socket [%d:%p] not implement event callback and remove it\n", sock->fd, sock);
                cp_fds_del_sock_task(sock);
            }
        }
    }

    /* Process all the socket in event queue now */
    while(!cp_queue_is_empty(fds->event_queue))
    {
        if( NULL!= (sock=cp_fds_sock_dequeue(fds)) )
        { 
            //log_dbg("after handle one socket count: %d\n", cp_queue_count(fds->event_queue));
            if(sock->cbfunc)
            { 
                log_dbg("Process socket [%d %p] in event queue on event [%d]\n", sock->fd, sock, sock->event.events); 
                rv = sock->cbfunc(sock);
            }
            else
            { 
                log_err("Process Socket [%d:%p] not implement event callback and remove it\n", sock->fd, sock);
            } 
        }
    }

    //log_trace("End process the events....\n");
    return;
}


void cp_sock_term_server(CP_SOCK *serv_sock)
{
    CP_SOCK           *sock, *tsock;

    log_warn("terminate server socket [%d] and [%d] accept client now\n", serv_sock->fd, serv_sock->accept_cnt);
    list_for_each_entry_safe(sock, tsock, &serv_sock->accept_list, rlist) 
    {
        /*remove all the accept socket from task and registry list, then destroy it*/
        //log_warn("destroy accept socket [%d:%p]\n", sock->fd, sock);
        cp_fds_destroy_sock(sock);
    }

    /*remove this server socket from task and registry list, then destroy it*/
    log_warn("terminate server socket [%d] and its accept client ok\n", serv_sock->fd);
    cp_fds_destroy_sock(serv_sock);

    return ;
}


void cp_sock_term_all_server(CP_FDS *fds)
{ 
    CP_SOCK           *sock, *tsock;

    log_dbg("destroy all the listen socket now\n");

    /* Terminate all the listen socket */
    list_for_each_entry_safe(sock, tsock, &fds->server_list, rlist) 
    {
        cp_sock_term_server(sock);
    }

    log_warn("destroy all the listen socket ok\n");

    return ;
}


void cp_sock_term_all_client(CP_FDS *fds)
{
    CP_SOCK           *sock = NULL, *tsock;

    log_dbg("destroy all the connect socket now\n");
    /* Terminate all the listen socket */
    list_for_each_entry_safe(sock, tsock, &fds->client_list, rlist) 
    {
        /*remove all the connect socket from task and registry list, then destroy it*/
        cp_fds_destroy_sock(sock);
    }
    log_warn("destroy all the connect socket ok\n");
}

/* Checkout the socket timeout happened or not, if happened then put it in task list */
void cp_sock_detect_timeout(CP_FDS *fds)
{ 
    CP_SOCK           *sock, *tsock, *serv_sock;

    /* check all the connect socket timeout */
    list_for_each_entry_safe(sock, tsock, &fds->client_list, rlist) 
    {
        /*If the socket timeout value and timeout, then disconnect it */
        if( sock->idle_timeout>0 && time_elapsed(sock->actv_time)>sock->idle_timeout )
        {
            log_warn("socket[%d] idle timeout happened and add to task list\n", sock->fd);
            log_dbg("last: %lu elapsed time: %lu idel timeout: %lu\n", sock->actv_time, time_elapsed(sock->actv_time), sock->idle_timeout);
            sock->event.events = CP_SOCK_EVENT_IDLE_TIMEOUT;
            cp_fds_add_sock_task(sock);
        }
        else if( sock->msg_timeout>0 && sock->msg_time>0 && time_elapsed(sock->msg_time)>sock->msg_timeout )
        {
            log_warn("socket[%d] message timeout happened and add to task list\n", sock->fd);
            sock->event.events = CP_SOCK_EVENT_MSG_TIMEOUT;
            cp_fds_add_sock_task(sock);
        }
    } 

    /* check all the accept socket timeout */
    list_for_each_entry(serv_sock, &fds->server_list, rlist) 
    {
        list_for_each_entry_safe(sock, tsock, &serv_sock->accept_list, rlist) 
        {
            /*If the socket timeout value and timeout, then disconnect it */
            if( sock->idle_timeout>0 && time_elapsed(sock->actv_time)>sock->idle_timeout )
            {
                sock->event.events = CP_SOCK_EVENT_IDLE_TIMEOUT;
                log_warn("socket[%d] idle timeout happened and add to task list\n", sock->fd);
                log_dbg("last: %lu elapsed time: %lu idel timeout: %lu\n", sock->actv_time, time_elapsed(sock->actv_time), sock->idle_timeout);
                cp_fds_add_sock_task(sock);
            }
            else if( sock->msg_timeout>0 && sock->msg_time>0 && time_elapsed(sock->msg_time)>sock->msg_timeout )
            {
                sock->event.events = CP_SOCK_EVENT_MSG_TIMEOUT;
                log_warn("socket[%d] message timeout happened and add to task list\n", sock->fd);
                cp_fds_add_sock_task(sock); 
            }
        } /* list_for_each_entry_safe server accept socket list */
    } /* list_for_each_entry all the server socket list */
}


void cp_sock_term_all_task(CP_FDS *fds)
{
    int               i;
    CP_SOCK           *sock;

    log_dbg("remove all the socket in task list now\n");

    /* remove all the socket from task array */
    cp_list_array_for_each(fds->task_array, i, sock)
    {
        //log_warn("remove socket[%d] in task list ok\n", sock->fd);
        if(sock)
            cp_fds_del_sock_task(sock);
    }

    while( !cp_queue_is_empty(fds->event_queue) )
    {
        cp_dequeue(fds->event_queue);
    }

    log_warn("remove all the socket in task list ok\n");
    
    return ;
}



