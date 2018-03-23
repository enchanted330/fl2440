/*********************************************************************************
 *      Copyright:  (C) 2014 Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  cvd_main.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(11/18/2014)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "11/18/2014 10:30:53 PM"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <sys/types.h>       /* basic system data types */
#include <sys/socket.h>      /* basic socket definitions */
#include <sys/epoll.h> /* epoll function */

#include "cp_comport.h"
#include "cp_socket.h"
#include "cp_logger.h"
#include "cp_proc.h"
#include "cvd_conf.h"
#include "cvd_main.h"
#include "version.h"

int epoll_add_socket(int epfd, cp_sock_t *sock);
int epoll_add_comport(int epfd, COM_PORT *comport, char *comport_dev);
void epoll_del_fd(int epfd, int fd);

static void prog_usage(const char *progname)
{
    banner(progname); 
    
    printf("Usage: %s [OPTION]...\n", progname);
    printf("Receive date from a serial port and transfer the data to remote server by socket.\n");
    printf("\nMandatory arguments to long options are mandatory for short options too:\n");

    printf(" -d[debug   ]  Running in debug mode\n");
    printf(" -l[level   ]  Set the log level as [0..%d]\n", LOG_LEVEL_MAX-1);
    printf(" -h[help    ]  Display this help information\n");
    printf(" -v[version ]  Display the program version\n"); 
    return ;
}

int main(int argc, char **argv)
{
    int                    i, epfd, nfds, nbytes;
    comport_sock_bind_t    cvd_bind;
    cp_logger              *logger;
    struct epoll_event     evts[MAXEPOLLSIZE];
    int                    opt;
    int                    debug = 0;
    char                   pid_file[64] = { 0 }; /*  The file used to record the PID */
    char                   *conf_file = CVD_DEF_CONF_FILE;
    char                   *log_file;
    int                    log_level = LOG_LEVEL_TRACE;
    const char             *progname=NULL;

    struct option long_options[] = {
        {"conf", required_argument, NULL, 'c'},
        {"debug", no_argument, NULL, 'd'},
        {"level", required_argument, NULL, 'l'},
        {"version", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };

    progname = basename(argv[0]);
    /* Parser the command line parameters */
    while ((opt = getopt_long(argc, argv, "c:dl:vh", long_options, NULL)) != -1)
    {
        switch (opt)
        {
            case 'c':  /* Set configure file */
                conf_file = optarg;
                break;

            case 'd': /* Set debug running */
                debug = 1;
                log_file = DBG_LOG_FILE;
                break;

            case 'l': /* Set the log level */
                i = atoi(optarg);
                log_level = i>LOG_LEVEL_MAX ? LOG_LEVEL_MAX-1 : LOG_LEVEL_MAX-1;
                break;

            case 'v':  /* Get software version */
                banner(progname); /* Defined in version.h */
                return EXIT_SUCCESS;

            case 'h':  /* Get help information */
                prog_usage(progname);
                return 0;

            default:
                break;
        } /*  end of "switch(opt)" */
    }

    memset(&cvd_bind, 0, sizeof(cvd_bind));

    if( !debug )
    {
        snprintf(pid_file, sizeof(pid_file), "/var/run/%s.pid", progname);
        if( check_daemon_running(pid_file) )
        {
            printf("Programe already running, exit now.\n");
            return -1;
        }
    }

    if( !(logger=cp_log_init(NULL, log_file, log_level, 0)) || cp_log_open()<0  )
    {
        printf("Init logger system failed, program exit now...\n");
        return -1;
    }

    if( parser_cvd_conf(conf_file, logger, &cvd_bind) < 0)
    {
        printf("Parser configure file '%s' failed, program exit now...\n", conf_file);
        return -1;
    }

    cvd_bind.comport = comport_init(cvd_bind.comport_dev, 115200, "8N1N");
    if( !cvd_bind.comport )
    {
        log_err("Init comport '%s' as 115200 8N1N failure\n", cvd_bind.comport_dev);
        return -2;
    }

    if( (epfd=epoll_create(MAXEPOLLSIZE)) < 0)
    {
        log_err("epoll_create failure: %s\n", strerror(errno));
        goto CleanUp;
    }


    if( !debug )
    {
        if( set_daemon_running(pid_file) )
        {
            log_fatal("Set program \"%s\" running as daemon failure.\n", progname);
            goto CleanUp;
        }
    }

    cp_install_proc_signal();

    while( !g_cp_signal.stop )
    {
        /* Open comport if it's not opened  */
        if( !cvd_bind.comport->is_connted )
        {
            epoll_add_comport(epfd, cvd_bind.comport, cvd_bind.comport_dev);
        }

        /* Connect to host if not connected  */
        if( SOCK_STAT_CONNECTED != cvd_bind.sock.status )
        {
            epoll_add_socket(epfd, &cvd_bind.sock);
        }

        /* Epoll wait the added socket event  */
        nfds=epoll_wait(epfd, evts, MAXEPOLLSIZE, EPOLL_TIMEOUT);
        if (nfds < 0)
        {
            log_err("epoll_wait get error: %s\n", strerror(errno));
            continue;
        }

        for (i=0; i<nfds; ++i)
        {
            /* Comport get data arriving event happened */
            if ( evts[i].data.fd == cvd_bind.comport->fd ) 
            {
                nbytes = comport_recv(cvd_bind.comport, cvd_bind.buf, sizeof(cvd_bind.buf), COMPORT_TIMEOUT);
                if( nbytes>0 )
                {
                    if( SOCK_STAT_CONNECTED == cvd_bind.sock.status )
                    {
                        log_dbg("Transfer data from comport to socket\n");
                        write(cvd_bind.sock.fd, cvd_bind.buf, nbytes);
                    }
                    else
                    {
                        log_err("Receive data from serial port [%s] but socket to [%s:%d] disconnect.\n", 
                                cvd_bind.comport->dev_name, cvd_bind.sock.host, cvd_bind.sock.port);
                    }
                }
            }
            /* Socket get data arriving event happened */
            else if ( evts[i].data.fd == cvd_bind.sock.fd ) 
            {
                nbytes = read(cvd_bind.sock.fd, cvd_bind.buf, sizeof(cvd_bind.buf));
                /* Socket disconnected  */
                if(nbytes == 0)
                {
                    log_warn("Socket[%d] connect to [%s:%d] disconnect\n", 
                            cvd_bind.sock.fd, cvd_bind.sock.host, cvd_bind.sock.port);
                    epoll_del_fd(epfd, evts[i].data.fd);
                    cp_sock_close(&cvd_bind.sock);
                }
                /* Socket read error  */
                else if(nbytes < 0)
                {
                    log_warn("Read data from socket[%d] get error: %s, close socket now.\n", 
                            cvd_bind.sock.fd, strerror(errno));
                    epoll_del_fd(epfd, evts[i].data.fd);
                    cp_sock_close(&cvd_bind.sock);
                }
                /* Socket read ok then transmiter to serial port  */
                else if( nbytes>0 )
                {
                    if( cvd_bind.comport->is_connted)
                    {
                        log_dbg("Transfer data from socket to comport\n");
                        write(cvd_bind.comport->fd, cvd_bind.buf, nbytes);
                    }
                    else
                    {
                        log_err("Receive data from socket to [%s:%d] but serial port [%s] disconnect.\n", 
                                cvd_bind.sock.host, cvd_bind.sock.port, cvd_bind.comport->dev_name);
                    }
                }
            }
        }
    }

CleanUp:
    comport_term(cvd_bind.comport);
    cp_log_term();

    return 0;
}

void epoll_del_fd(int epfd, int fd)
{
    struct epoll_event     ev;

    log_warn("Remove fd[%d] from epoll\n", fd);
    ev.events = EPOLLIN | EPOLLET;
    epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &ev);
}

int epoll_add_comport(int epfd, COM_PORT *comport, char *comport_dev)
{
    int                    rv = -1;
    struct epoll_event     ev;

    if( comport_open(comport) < 0 )
    {
        rv = -2;
        log_err("Open comport '%s' as 115200 8N1N failure\n", comport_dev);
        goto failed;
    }

    ev.data.fd = comport->fd;
    ev.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, comport->fd, &ev) < 0) 
    {
        rv = -3;
        log_err("epoll add comport fd[%d] failure\n", comport->fd);
        goto failed;
    }

    log_nrml("Add comport [%s] to epoll ok\n", comport_dev);

    return 0;

failed:
    comport->fd = -1;
    return rv;
}


int epoll_add_socket(int epfd, cp_sock_t *sock)
{
    struct epoll_event     ev;

    if(SOCK_STAT_CONNECTED == sock->status)
        return 0;

    /* Try to connect to remote server again */
    cp_sock_connect(sock);

    if( SOCK_STAT_CONNECTED != sock->status )
    {
        return -1;
    }

    ev.data.fd = sock->fd;
    ev.events = EPOLLIN | EPOLLET;
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, sock->fd, &ev) < 0) 
    {
        log_err("epoll add socket[%d] failure\n", sock->fd);
        return -2;
    }

    log_nrml("Add socket[%d] connect to [%s:%d] to epoll ok\n", sock->fd, sock->host, sock->port);
    return 0;
}


