/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  cp_sock.c
 *    Description:  This file is the Linux TCP socket basic library.
 *                 
 *        Version:  1.0.0(10/26/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "10/26/2012 01:20:41 PM"
 *                 
 ********************************************************************************/

#include "cp_sock.h"
#include "cp_logger.h"
#include "cp_common.h"
#include "cp_time.h"

CP_SOCK *cp_sock_init(CP_SOCK *sock, unsigned int rsize, unsigned int ssize, int keepintvl, int keepcnt)
{
    if(!sock)
    {
        if( !(sock=(CP_SOCK *)t_malloc(sizeof(*sock))) )
        {
            log_err("socket context malloc failed: %s\n", strerror(errno));
            goto failed;
        }
        else
        {
            memset(sock, 0, sizeof(*sock));
            sock->flag |= FLAG_SOCK_MALLOC;
        }
    }
    else
    {
        memset(sock, 0, sizeof(*sock));
    }

    if( !(sock->rbuf=cp_string_create_empty(CP_DEF_RCVBUF_SIZE)) )
    {
        log_err("socket context create recv buffer failed\n");
        cp_sock_term(sock);
        goto failed;
    }

    if( !(sock->sbuf=cp_string_create_empty(CP_DEF_SNDBUF_SIZE)) )
    {
        log_err("socket context create send buffer failed\n");
        cp_sock_term(sock);
        goto failed;
    }

    log_dbg("socket context [%p] initialise ok\n", sock);
    sock->rsize = rsize;
    sock->ssize = ssize;
    sock->keepintvl = keepintvl;
    sock->keepcnt = keepcnt;
    sock->idle_timeout = CP_SOCK_DEF_IDLE_TIMEOUT;
    sock->msg_timeout = CP_SOCK_DEF_MSG_TIMEOUT;
    sock->fd = -1;
    sock->index = -1;
    sock->actv_time = time_now();

    sock->flag |= FLAG_SOCK_INIT;

    return sock;

failed:
    log_err("socket context initialize failed\n");
    return NULL;
}

void cp_sock_term(CP_SOCK *sock)
{
    if(!sock)
        return;

    log_dbg("terminate socket [%p:%d] now\n", sock, sock->fd);

    if(sock->flag & FLAG_SOCK_INIT)
        cp_sock_close(sock);

    if(sock->rbuf)
    {
        cp_string_destroy(sock->rbuf);
    } 

    if(sock->privt && sock->privt_free)
    {
        sock->privt_free();
    }

    if(sock->sbuf)
    {
        cp_string_destroy(sock->sbuf);
    }

    if(sock->flag & FLAG_SOCK_MALLOC)
    {
        //memset(sock, 0, sizeof(*sock));
        t_free(sock);
        return ;
    }
    
    //memset(sock, 0, sizeof(*sock));
    sock->fd = -1;  

    return ;
}

int cp_sock_close(CP_SOCK *sock)
{
    int        rv = 0;
    int        force = 0;

    if(!sock)
        return -1;

    if(sock->fd>0)
    {
        log_dbg("Close socket[%d] bind [%s:%d]\n", sock->fd, sock->laddr, sock->lport);
        if(force)
        {
            /*
             * If l_onoff is nonzero and l_linger is zero, TCP aborts the connection when it is closed.  
             * That is, TCP discards any data still remaining in the socket send buffer and sends an RST 
             * to the peer, not the normal four-packet connection termination sequence.
             */
            struct linger so_linger;

            so_linger.l_onoff = 1;  /* Turn on linger */
            so_linger.l_linger = 0; /* Set the timeout to 0  */

            setsockopt (sock->fd, SOL_SOCKET, SO_LINGER, (char *) &so_linger, sizeof (struct linger));
        } 
        
        if(CP_SOCK_MODE_ACCEPT == sock->mode || CP_SOCK_MODE_CONNECT == sock->mode)
        {
            shutdown (sock->fd, SHUT_RDWR);
        }

        if( (rv=close(sock->fd)) )
        {
            return -2;
        }
    }

    sock->fd = -1;
    return rv;
}


int cp_sock_listen(CP_SOCK *sock, char *laddr, int lport, int backlog)
{
    int                 rv = 0;
    int                 fd = -1;
    char                service[20];
    struct addrinfo     hints, *rp;
    struct addrinfo     *res = NULL;
    struct in_addr      inaddr;

    if(!sock || !(sock->flag&FLAG_SOCK_INIT) || lport<=0 || backlog <=0)
    {
        log_err("Invalide input arguments\n");
        rv = -1;
        goto cleanup;
    }

    strncpy(sock->laddr, (!laddr?"0.0.0.0":laddr), sizeof(sock->laddr));
    log_dbg("Create socket listen on [%s:%d] now\n", sock->laddr, lport);

    cp_sock_close(sock);

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; /*  Only support IPv4 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP; /*  TCP protocol */ 
    hints.ai_flags = AI_PASSIVE;

    /* If 'laddr' is a valid IP address, then don't use name resolution */
    if(laddr && inet_aton(laddr, &inaddr))
    {
        log_info("%s is a valid IP address, don't use name resolution.\n", laddr);
        hints.ai_flags |= AI_NUMERICHOST;
    }

    /* Obtain address(es) matching host/port */
    snprintf(service, sizeof(service), "%d", lport);
    if( (rv=getaddrinfo(laddr, service, &hints, &res)) )
    {
        log_err("getaddrinfo() parser [%s:%s] failed: %s\n", laddr, service, gai_strerror(rv));
        rv = -2;
        goto cleanup;
    }


    /*  getaddrinfo() returns a list of address structures. Try each 
     *  address until we successfully connect or bind*/
    sock->fd = -1;
    for (rp=res; rp!=NULL; rp=rp->ai_next) 
    {
        /* Create the socket */
        if( (fd=socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) < 0)
        {
            log_err("socket() create failed: %s\n", strerror(errno));
            rv = -3;
            continue;
        }

        cp_sock_set_reuseaddr(fd);
        cp_sock_set_keepalive(fd, sock->keepintvl, sock->keepcnt);
        cp_sock_set_nonblock(fd); 
        
        if( 0==bind(fd, rp->ai_addr, rp->ai_addrlen) ) 
        { 
            struct sockaddr_in    *saddr = (struct sockaddr_in *)rp->ai_addr;
            sock->fd = fd;
            sock->lport = lport;

            strncpy (sock->laddr, inet_ntoa (saddr->sin_addr), sizeof(sock->laddr)-1);
            log_dbg("Bind socket[%d] ok\n", fd);
            rv = 0;
            break;
        }
        else
        {
            rv = -4;
            log_err("Bind socket[%d] failed: %s\n", fd, strerror(errno));
        }

        close(fd);
    } /* for (rp=res; rp!=NULL; rp=rp->ai_next)  */


    if(sock->fd < 0)
    {
        close(fd);
        goto cleanup;
    }

    if(listen(sock->fd, backlog))
    {
        log_err("Listen on socket[%d] failed: %s\n", sock->fd, strerror(errno));
        rv = -6;
        goto cleanup;
    }

    sock->mode = CP_SOCK_MODE_LISTEN;
    sock->rport = 0;
    memset(&sock->raddr, 0, sizeof(sock->raddr));

cleanup:
    if( rv )
    {
        log_err("Create socket listen on [%s:%d] failed\n", sock->laddr, lport);
        if(sock->fd > 0)
            close(sock->fd);
    }
    else
    {
        sock->actv_time = time_now();
        sock->status = SOCK_STAT_LISTENED;
        log_nrml("Create socket[%p:%d] listen [%s:%d] ok\n", sock, sock->fd, sock->laddr, lport);
    }

    if(res)
        freeaddrinfo(res);  /* No longer needed */
    return rv;
}


int cp_sock_accept(CP_SOCK *serv_sock, CP_SOCK *new_sock)
{
    struct sockaddr_in       saddr;
    socklen_t                len = sizeof(saddr);
    int                      fd = -1;
    int                      rv = 0;

    if(!serv_sock || !new_sock || serv_sock->fd<0 )
    {
        log_err("Invalide input arguments\n");
        rv = -1;
        goto cleanup;
    }

    log_dbg("accept new client from server [fd=%d %s:%d] now\n", serv_sock->fd, serv_sock->laddr, serv_sock->lport);

    if( (fd=accept(serv_sock->fd, (struct sockaddr *)&saddr, &len)) < 0 )
    {
        log_err("Accept new client from server [fd=%d %s:%d] failed: %s\n", 
                serv_sock->fd, serv_sock->laddr, serv_sock->lport, strerror(errno));
        rv = -2;
        goto cleanup;
    }

    /* Set socket buffer size  */
    cp_sock_set_buffer(fd, new_sock->rsize, new_sock->ssize);
    /* Enable socket keepalive to detect network  */
    cp_sock_set_keepalive(fd, new_sock->keepintvl, new_sock->keepcnt);
    cp_sock_set_nonblock(fd); 

    new_sock->fd = fd;
    new_sock->mode = CP_SOCK_MODE_ACCEPT;
    /* get remote address */
    strncpy (new_sock->raddr, inet_ntoa(saddr.sin_addr), sizeof(new_sock->raddr)-1);
    new_sock->rport = ntohs(saddr.sin_port);

    /* Get local address */
    len = sizeof(saddr);
    if (getsockname (new_sock->fd, (struct sockaddr *) &saddr, (socklen_t *) &len)) 
    {
        strncpy(new_sock->laddr, serv_sock->laddr, sizeof(new_sock->laddr)-1);
        new_sock->lport = serv_sock->lport;
    }
    else
    {
        strncpy (new_sock->laddr, inet_ntoa (saddr.sin_addr), sizeof (new_sock->laddr) - 1);
        new_sock->lport = ntohs (saddr.sin_port);
    }

cleanup:
    if(!rv)
    {
        new_sock->status = SOCK_STAT_ACCEPTED;
        new_sock->actv_time = time_now();
        log_nrml("Accept new client socket [%d %s:%d] <== [%s:%d]\n", 
                new_sock->fd, new_sock->laddr, new_sock->lport, new_sock->raddr, new_sock->rport);
    }
    else
    {
        log_err("Accept new client from server [%d] on [%s:%d] failed\n", 
                serv_sock->fd, serv_sock->raddr, serv_sock->rport);
        close(fd);
    }

    return rv;
}


/*
 *  Description:  Create a socket connect to the remove server
 *   Input args:  $sock: The socket contex   $raddr: Remote server address  
 *                $rport: Remote server listened port     
 *                $lport: Use this local port connect to remote server
 *  Output args:  None
 * Return value:  <0:connect failed  ==0:connect ok  >0:connect in progress
 *
 */
int cp_sock_connect(CP_SOCK *sock, char *raddr, int rport, int lport)
{
    int                 rv = 0;
    int                 fd = -1;
    char                service[20];
    struct addrinfo     hints, *rp;
    struct addrinfo     *res = NULL;
    struct in_addr      inaddr;
    struct sockaddr_in  saddr;
    int                 len = sizeof(saddr); 

    if(!sock || !(sock->flag&FLAG_SOCK_INIT) || !raddr || rport<=0)
    {
        log_err("Invalide input arguments\n");
        rv = -1;
        goto failed;
    }

    if(SOCK_STAT_CONNECTED == sock->status)
    {
        return 0;
    }
    else if(SOCK_STAT_CONNECTING == sock->status)
    {
        goto connecting;
    }

    log_dbg("create socket connect to remote server [%s:%d] now\n", raddr, rport);

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; /*  Only support IPv4 */
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP; /*  TCP protocol */ 
    hints.ai_flags = AI_PASSIVE;

    /* If 'raddr' is a valid IP address, then don't use name resolution */
    if(raddr && inet_aton(raddr, &inaddr))
    {
        log_info("%s is a valid IP address, don't use name resolution.\n", raddr);
        hints.ai_flags |= AI_NUMERICHOST;
    }

    /* Obtain address(es) matching host/port */
    snprintf(service, sizeof(service), "%d", rport);
    if( (rv=getaddrinfo(raddr, service, &hints, &res)) )
    {
        log_err("getaddrinfo() parser [%s:%s] failed: %s\n", raddr, service, gai_strerror(rv));
        rv = -2;
        goto failed;
    }

    cp_sock_close(sock); /* close any opened socket on it */
    sock->fd = -1;

    /*  getaddrinfo() returns a list of address structures. Try each 
     *  address until we successfully connect or bind*/
    for (rp=res; rp!=NULL; rp=rp->ai_next) 
    {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);

        /* Create the socket */
        if( fd < 0)
        {
            log_err("socket() create failed: %s\n", strerror(errno));
            rv = -3;
            continue;
        }
        log_trace("create socket[%p:%d] with connect mode ok\n", sock, fd); 
        
        /* We need bind lport to this socket */
        if(lport > 0)
        { 
            memset(&saddr, 0, len);
            saddr.sin_family = AF_INET;
            saddr.sin_port = htons ((u_short) lport); 
            
            if ( bind(fd, (struct sockaddr *)&saddr, len) )
            {
                rv = -4;
                close(fd);
                log_err("Bind port[%d] to connect socket [%d] failed: %s\n", lport, fd, strerror(errno));
                continue;
            }
            else
            {
                sock->lport = lport; 
                rv = 0;
                log_dbg("Bind port[%d] to connect socket [%d] OK\n", lport, fd);
            }
        }

        /* Set socket options */
        cp_sock_set_buffer(fd, sock->rsize, sock->ssize);
        cp_sock_set_keepalive(fd, sock->keepintvl, sock->keepcnt);
        cp_sock_set_nonblock(fd);

        /* Nono block Connect to the remote server */
        if(0==connect(fd, rp->ai_addr, rp->ai_addrlen))
        { 
            /* Conenct to server successfully */
            sock->fd = fd;
            sock->status = SOCK_STAT_CONNECTING; 

            memcpy(&sock->saddr, &rp->ai_addr, sizeof(sock->saddr));
            
            rv = 0;
            goto connected;
        }
        else
        {
            if(EINPROGRESS == errno)
            {
                /* Connect to server now in progress  */
                sock->fd = fd;
                sock->status = SOCK_STAT_CONNECTING; 
                memcpy(&sock->saddr, &rp->ai_addr, sizeof(sock->saddr));

                rv = 0;
                goto connecting;
            }
            else
            {
                /* Connect to server failed. */
                close(fd);
                rv = -5;
                log_err("connect() to server failed: %s\n", strerror(errno));
                goto failed;
            }
        } 

        close(fd);
    }  /* for (rp=res; rp!=NULL; rp=rp->ai_next)  */


    if(sock->fd < 0)
    {
        close(fd);
        goto failed;
    }


connecting: 
    if(SOCK_STAT_CONNECTING == sock->status)
    { 

        len = sizeof(sock->saddr); 

        errno = 0;
        if( !connect(sock->fd, &sock->saddr, len) )
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
                log_dbg("socket[%d] connect to remote [%s:%d] in progress\n", sock->fd, raddr, rport);
                rv = 0;
                goto cleanup;

            default:
                log_err("socket[%d] connect to remote [%s:%d] failed: %s\n", sock->fd, raddr, rport, strerror(errno));
                sock->status = SOCK_STAT_DISCONNECT; 
                rv = -7;
                break;
        }
    }

connected:
    if(SOCK_STAT_CONNECTED == sock->status)
    { 
        rv = 0;

        log_nrml("socket[%d] connected to remote server [%s:%d]\n", sock->fd, raddr, rport);
        /* Set remote address */ 
        sock->rport=rport; 
        strncpy (sock->raddr, raddr, sizeof(sock->raddr)-1); 
        sock->actv_time = time_now();
        
        /* Get local address */ 
        memset(&saddr, 0, len);
        len = sizeof(saddr); 

        if (!getsockname (sock->fd, (struct sockaddr *) &saddr, (socklen_t *) &len)) 
        {
            strncpy (sock->laddr, inet_ntoa (saddr.sin_addr), sizeof (sock->laddr) - 1);
            sock->lport = ntohs (saddr.sin_port);
        }
        else
        {
            memset(&sock->laddr, 0, sizeof(sock->laddr));
            sock->lport = lport;
        }
        goto cleanup;
    }


failed:
    log_err("Create socket connect to [%s:%d] failed\n", raddr, rport);
    if(sock && sock->fd >= 0)
        close(sock->fd);

cleanup:
    if(res)
        freeaddrinfo(res);  /* No longer needed */

    return rv;
}


int cp_sock_recv(CP_SOCK *sock)
{
    int               rv, left;
    int               flag;
    cp_string         *rbuf;

    if(!sock || !sock->rbuf || !sock->rbuf->data || sock->fd<0)
    {
        log_err("Invalide input arguments\n");
        return -1;
    }

    rbuf = sock->rbuf;
    left = rbuf->size-rbuf->len;
    flag = left ? 0 : MSG_PEEK; 

    if(left <= 0)
        return 0;
    
    rv=recv(sock->fd, &rbuf->data[rbuf->len], left, flag);
    if( rv > 0)
    {
        rbuf->len += rv;
    }
    else if(rv<0)
    {
        if(EAGAIN==errno || EINPROGRESS==errno)
        { 
            log_warn("socket [%d] please recv data again: %s\n", sock->fd, strerror(errno));
            rv = 0;
        }
        else
        {
            log_err("socket [%d] recv data failed: %s\n", sock->fd, strerror(errno));
            rv = -2;
        }
    }

    if(rv > 0)
    {
        sock->actv_time = sock->msg_time = time_now();
    }
    return rv;
}

int cp_sock_send(CP_SOCK *sock)
{
    int                      rv;
    struct pollfd            sock_poll;

    if(!sock || sock->fd<0 || !sock->sbuf->data || sock->sbuf->len<=0)
    {
        log_err("Invalide input arguments\n");
        return -1;
    }

    sock_poll.events = POLLOUT;
    sock_poll.fd = sock->fd;

    if( (rv=poll(&sock_poll, 1, 0)) > 0 )
    {
        if( !(POLLOUT &sock_poll.events) )
        {
            return -2;
        }

        if ( (rv=send(sock->fd, sock->sbuf->data, sock->sbuf->len, MSG_NOSIGNAL)) < 0) 
        {
            if (EAGAIN == errno) 
            {
                log_warn("socket [%d] please send data again: %s\n", sock->fd, strerror(errno));
                return 0;
            }
            else
            {
                log_err("socket [%d] send data failed: %s\n", sock->fd, strerror(errno));
                return -4;
            }
        }
        else
        {
            sock->sbuf->len -= rv;
            sock->actv_time = time_now();
            memmove (sock->sbuf->data, sock->sbuf->data+rv, sock->sbuf->len);
            return rv;
        }
    }
    else /* poll()<0 means failed, poll()==0 means timeout */
    {
        log_err("send data from socket [%d] poll failed: %s\n", sock->fd, strerror(errno));
        return 0;
    }

    return 0;
}

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

int cp_sock_set_buffer(int sockfd, int rsize, int ssize)
{
    int        opt;
    socklen_t  optlen = sizeof(opt);

    if(sockfd < 0)
        return -1;

    /* Get system default receive buffer size, Linux X86: 85K */
    if (getsockopt (sockfd, SOL_SOCKET, SO_RCVBUF, (char *) &opt, &optlen)) 
    {
        log_warn("getsockopt() get receive buffer failure: %s\n", strerror(errno));
        return -2;
    }

    /* Only when current receive buffer size larger than the default one will change it  */
    if(rsize > opt)
    {
        opt = (int) rsize;
        if (setsockopt (sockfd, SOL_SOCKET, SO_RCVBUF, (char *) &opt, optlen)) 
        {
            log_warn("setsockopt() set receive buffer to %d failure: %s\n", opt, strerror(errno));
            return -2;
        }
    }

    /* Get system default send buffer size, Linux X86: 16K */
    if (getsockopt (sockfd, SOL_SOCKET, SO_SNDBUF, (char *) &opt, &optlen)) 
    {
        log_warn("getsockopt() get send buffer failure: %s\n", strerror(errno));
        return -3;
    }

    /* Only when current receive buffer size larger than the default one will change it  */
    if(ssize > opt)
    {
        opt = (int) ssize;
        if (setsockopt (sockfd, SOL_SOCKET, SO_SNDBUF, (char *) &opt, optlen)) 
        {
            log_warn("setsockopt() set send buffer to %d failure: %s\n", opt, strerror(errno));
            return -3;
        }
    }

    log_info("Set socket[%d] RCVBUF size:%d  SNDBUF size:%d\n", sockfd, rsize, ssize);
    return 0;
}

int cp_sock_set_reuseaddr(int sockfd)
{
    int opt = 1;
    int len = sizeof (int);

    if (setsockopt (sockfd, SOL_SOCKET, SO_REUSEADDR, (void *) &opt, len)) 
    {
        log_err("Set socket[%d] option SO_REUSEADDR failed:%s\n", sockfd, strerror(errno));
        return -1;
    } 
    log_dbg("Set socket[%d] option SO_REUSEADDR ok\n", sockfd);

    return 0;
}

/*
 * Enable socket SO_KEEPALIVE, if the connection disconnected, any system call on socket
 * will return immediately and errno will be set to "WSAENOTCONN"
 *
 * keepalive is not program related, but socket related, * so if you have multiple sockets, 
 * you can handle keepalive for each of them separately.
 *
 * Reference: http://tldp.org/HOWTO/html_single/TCP-Keepalive-HOWTO/
 */
int cp_sock_set_keepalive(int sockfd, int keepintvl, int keepcnt)
{
    int  opt;

    if(sockfd < 0)
        return -1;

    /* Enable the KEEPALIVE flag */
    opt = 1;
    if (setsockopt (sockfd, SOL_SOCKET, SO_KEEPALIVE, (char *) &opt, sizeof (opt))) 
    {
        log_warn("setsockopt() enable SO_KEEPALIVE failure: %s\n", strerror(errno));
        return -2;
    }

    if(keepintvl || keepcnt)
    {
        /*
         *  The tcp_keepidle parameter specifies the interval between the last data packet sent 
         *  (simple ACKs are not considered data) and the first keepalive probe; after the 
         *  connection is marked to need keepalive, this counter is not used any further. 
         *  ~ >: cat /proc/sys/net/ipv4/tcp_keepalive_time 
         *  7200
         */
        opt = 3; /* 3 seconds  */
        if (setsockopt (sockfd, SOL_TCP, TCP_KEEPIDLE, (char *) &opt, sizeof (opt))) 
        {
            log_err("setsockopt() set TCP_KEEPIDLE to %d seconds failure: %s\n", opt, strerror(errno));
            return -3;
        }

        if((opt=keepintvl) > 0)
        {
            /*
             * The tcp_keepintvl parameter specifies the interval between subsequential keepalive 
             * probes, regardless of what the connection has exchanged in the meantime.
             * ~ >: cat /proc/sys/net/ipv4/tcp_keepalive_intvl 
             * 75
             */
            if (setsockopt (sockfd, SOL_TCP, TCP_KEEPINTVL, (char *) &opt, sizeof (opt))) 
            {
                log_err("setsockopt() set TCP_KEEPINTVL to %d failure: %s\n", opt, strerror(errno));
                return -4;
            }
        }

        if((opt=keepcnt) > 0)
        {
            /*
             * The TCP_KEEPCNT option specifies the maximum number of unacknowledged probes to 
             * send before considering the connection dead and notifying the application layer
             * probes to be sent. The value of TCP_KEEPCNT is an integer value between 1 and n, 
             * where n is the value of the systemwide tcp_keepcnt parameter. 
             * ~ >: cat /proc/sys/net/ipv4/tcp_keepalive_probes 
             * 9
             */
            if (setsockopt (sockfd, SOL_TCP, TCP_KEEPCNT, (char *) &opt, sizeof (opt)))
            {
                log_err("setsockopt() set TCP_KEEPCNT to %d failure: %s\n", opt, strerror(errno));
                return -5;
            }
        }
    }

    log_dbg("Set socket[%d] KEEPINTVL:%d  KEEPCNT:%d\n", sockfd, keepintvl, keepcnt);
    return 0;
}


