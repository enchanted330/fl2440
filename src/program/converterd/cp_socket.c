/*********************************************************************************
 *      Copyright:  (C) 2014 Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  cp_socket.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(11/18/2014)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "11/18/2014 11:15:04 PM"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/un.h>
#include <sys/types.h>
#include <linux/sockios.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "cp_logger.h"
#include "cp_socket.h"

int cp_sock_set_nonblock(int sockfd)
{
    int opts;
    /*
     * fcntl may set: 
     *
     * EACCES, EAGAIN: Operation is prohibited by locks held by other 
     *          processes. Or, operation is prohibited because the file has 
     *          been memory-mapped by another process. 
     * EBADF:   fd is not an open file descriptor, or the command was F_SETLK 
     *          or F_SETLKW and the file descriptor open mode doesn't match 
     *          with the type of lock requested.
     * EDEADLK: It was detected that the specified F_SETLKW command would 
     *          cause a deadlock.
     * EFAULT:  lock is outside your accessible address space.
     * EINTR:   For F_SETLKW, the command was interrupted by a signal. For 
     *          F_GETLK and F_SETLK, the command was interrupted by a signal 
     *          before the lock was checked or acquired. Most likely when 
     *          locking a remote file (e.g. locking over NFS), but can 
     *          sometimes happen locally.
     * EINVAL:  For F_DUPFD, arg is negative or is greater than the maximum 
     *          allowable value. For F_SETSIG, arg is not an allowable signal 
     *          number.
     * EMFILE:  For F_DUPFD, the process already has the maximum number of 
     *          file descriptors open. 
     * ENOLCK:  Too many segment locks open, lock table is full, or a remote 
     *          locking protocol failed (e.g. locking over NFS).
     * EPERM:   Attempted to clear the O_APPEND flag on a file that has the 
     *          append-only attribute set.
     */
    opts = fcntl(sockfd, F_GETFL);
    if (opts < 0)
    {
        log_warn("fcntl() get socket options failure: %s\n", strerror(errno));
        return -1;
    }

    opts |= O_NONBLOCK;
    if (fcntl(sockfd, F_SETFL, opts) < 0)
    {
        log_warn("fcntl() set socket options failure: %s\n", strerror(errno));
        return -1;
    }

    log_dbg("Set socket[%d] none blocking\n", sockfd);
    return opts;
}

int cp_sock_connect(cp_sock_t *sock)
{
    int                 rv = -1;
    int                 sock_fd = -1;
    char                service[20];
    struct addrinfo     hints, *rp;
    struct addrinfo     *res = NULL;
    struct in_addr      inaddr;
    char                ipstr[20];

    if(!sock)
    {
        log_err("Invalid input argument\n");
        return -1;
    }

    if(SOCK_STAT_CONNECTED == sock->status)
    {
        return 0;
    }
    else if(SOCK_STAT_CONNECTING == sock->status)
    {
        goto connecting;
    }

    log_nrml("Start socket connect to [%s:%d]...\n", sock->host, sock->port);

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; /*   Only support IPv4 */
    hints.ai_socktype = SOCK_STREAM;

    /*  If Hostname is a valid IP address, then don't use name resolution */
    if( inet_aton(sock->host, &inaddr) )
    {
        log_info("%s is a valid IP address, don't use name resolution.\n", sock->host);
        hints.ai_flags |= AI_NUMERICHOST;
    }  

    /*  Obtain address(es) matching host/port */
    snprintf(service, sizeof(service), "%d", sock->port);
    if( (rv=getaddrinfo(sock->host, service, &hints, &res)) )
    {
        log_err("getaddrinfo() parser [%s:%s] failed: %s\n", sock->host, service, gai_strerror(rv));
        rv = -2;
        goto failed;
    }   


    /*  getaddrinfo() returns a list of address structures. Try each 
     *  address until we successfully connect or bind 
     */
    rv = -3; /* Connect failure  */
    for (rp=res; rp!=NULL; rp=rp->ai_next)
    {
        /*  Create the socket */ 
        sock_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol); 
        if( sock_fd < 0) 
        { 
            log_err("socket() create failed: %s\n", strerror(errno));
            rv = -4;
            continue;
        }

        inet_ntop(AF_INET, &(((struct sockaddr_in *)(rp->ai_addr))->sin_addr), ipstr, sizeof(ipstr));
        log_dbg("DNS resolve IP address [%s]\n", ipstr);
        
        cp_sock_set_nonblock(sock_fd);

        /* Nono block Connect to the remote server */
        if(0==connect(sock_fd, rp->ai_addr, rp->ai_addrlen))
        {
            /* Conenct to server successfully */
            memcpy(&sock->saddr, &rp->ai_addr, sizeof(sock->saddr));
            sock->fd = sock_fd;
            sock->status = SOCK_STAT_CONNECTED;
            rv = 0;
            goto connected;
        }
        else
        {
            if(EINPROGRESS == errno)
            {
                /* Connect to server now in progress  */
                memcpy(&sock->saddr, &rp->ai_addr, sizeof(sock->saddr));
                sock->fd = sock_fd;
                sock->status = SOCK_STAT_CONNECTING; 
                rv = 0; 
                goto connecting;
            }
            else
            {
                /* Connect to server failed. */
                close(sock_fd);
                rv = -5;
                log_err("connect() to server failed: %s\n", strerror(errno));
                goto failed;
            }
        }

        close(sock_fd);
    }


failed:
    log_err("create socket connect to [%s:%d] failed: %s\n", sock->host, sock->port, strerror(errno));
    sock->status = SOCK_STAT_DISCONNECT;
    sock->fd = -1;
    return rv;

connecting:
    if(SOCK_STAT_CONNECTING == sock->status)
    {
        int         len;

        len = sizeof(sock->saddr);

        errno = 0;
        if( 0 == connect(sock->fd, &sock->saddr, len) )
        {
            /* connect() return 0 means it already connected */
            sock->status = SOCK_STAT_CONNECTED;
            rv = 0;
            goto connected;
        }

        /* Connect failure will continue to check */
        switch (errno)
        {
            case EISCONN:
                sock->status = SOCK_STAT_CONNECTED;
                rv = 0;
                goto connected;

            case EALREADY:
            case EINPROGRESS:
                log_dbg("socket[%d] connect to remote [%s:%d] in progress\n", sock->fd, sock->host, sock->port);
                rv = 0;
                goto cleanup;

            default:
                log_err("socket[%d] connect to remote [%s:%d] failed: %s\n", sock->fd, sock->host, sock->port, strerror(errno));
                sock->status = SOCK_STAT_DISCONNECT;
                rv = -7;
                break;
        }
    }

connected:
    if(SOCK_STAT_CONNECTED == sock->status)
    {
        rv = 0;
        log_nrml("socket[%d] connected to remote server [%s:%d]\n", sock->fd, sock->host, sock->port);
        goto cleanup;
    }

cleanup:
    if(res)
        freeaddrinfo(res);  /* No longer needed */

    return rv;
}

void cp_sock_close(cp_sock_t *sock)
{
    close(sock->fd);
    sock->fd = 0;
    sock->status = SOCK_STAT_INIT;
}

void cp_sock_term(cp_sock_t *sock)
{
    close(sock->fd);
    memset(sock, 0, sizeof(*sock));
}
