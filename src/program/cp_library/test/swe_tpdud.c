/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  tpdud.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(10/30/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "10/30/2012 01:15:18 PM"
 *                 
 ********************************************************************************/

#include <unistd.h>
#include <getopt.h>
#include <libgen.h>

#include "cp_common.h"
#include "cp_network.h"
#include "cp_logger.h"
#include "cp_proc.h"
#include "cp_time.h"
#include "swe_tpdud.h"
#include "version.h"

static void stop_tpdu_service(SWE_TPDU *tpdu)
{
    if(!tpdu)
        return ;
    
    log_dbg("release tpdu contex [%p]\n", tpdu);

    if( tpdu->dlink ) 
    { 
        log_nrml("stop tpdu service for downlink socket [%p:%d]\n", tpdu->dlink, tpdu->dlink->fd);
        cp_fds_destroy_sock_clear(tpdu->dlink);
        tpdu->dlink = NULL;
    } 
        
    if( tpdu->ulink ) 
    { 
        log_nrml("stop tpdu service for uplink socket [%p:%d]\n", tpdu->ulink, tpdu->ulink->fd);
        cp_fds_destroy_sock_clear(tpdu->ulink);
        tpdu->ulink = NULL;
    } 
    
    t_free(tpdu);
}

int proc_uplink_event(CP_SOCK *ulink)
{
    int           rv;
    CP_SOCK       *dlink = NULL;
    SWE_TPDU      *tpdu = NULL; 

    if(!ulink || !(tpdu=ulink->privt) || !(dlink=tpdu->dlink))
    {
        log_err("tpdu address:[%p] ulink address:[%p] dlink address:[%p]\n", tpdu, ulink, dlink);
        return -1;
    }

    if(ulink->status == SOCK_STAT_DISCONNECT)
    {
        goto stop_service;
    }

    if(ulink->event.events & CP_SOCK_EVENT_READ)
    {
        rv = cp_sock_recv(ulink);
        if( 0==rv )
        {
            log_warn("Socket [%d %s:%d] remote socket [%s:%d] disconnect, remove it now\n", 
                    ulink->fd, ulink->laddr, ulink->lport, ulink->raddr, ulink->rport);
            goto stop_service;
        }
        else if(rv > 0)
        {
            log_nrml("Socket [%d %s:%d] receive %d bytes data:\n", ulink->fd, ulink->laddr, ulink->lport, rv);
            cp_log_dump(LOG_LEVEL_DEBUG, ulink->rbuf->data, ulink->rbuf->len);

            if(ulink->rbuf->len > 0)
            { 
                cp_string_move(dlink->sbuf, ulink->rbuf); 
                cp_sock_send(dlink);
            }
        }
        else
        {
            log_err("Socket [%d %s:%d] receive data failed: %s\n", ulink->fd, ulink->laddr, ulink->lport, strerror(errno));
        }
    }
    else if(ulink->event.events & CP_SOCK_EVENT_IDLE_TIMEOUT)
    {
        log_warn("uplink socket [%d] is idle and to-do something here\n");
    }
    else if(ulink->event.events & CP_SOCK_EVENT_MSG_TIMEOUT)
    {
        log_warn("uplink socket [%d] message is timeout and to-do something here\n");
    }



    return 0;
    
stop_service:
    stop_tpdu_service(tpdu);
    return 0;
}

/* Connect to hostsim */
int tpdu_connect_uplink(SWE_TPDU *tpdu)
{
    CP_SOCK       *ulink;
    CP_SOCK       *dlink;  

    if(!tpdu || !(dlink=tpdu->dlink))
    {
        log_err("tpdu address [%p] dlink address [%p]\n", tpdu, dlink);
        return -1;
    }

    ulink = tpdu->ulink;
    
    ulink=cp_sock_connect_regist(dlink->fds, ulink, tpdu->raddr, tpdu->rport, 0, proc_uplink_event); 
    if( !ulink ) /* connect failure */
    {
        /* the uplink connect failure or connect already, then we should 
         * remove the downlink socket from task list  */
        log_err("connect to remote [%s:%d] failure\n", tpdu->raddr, tpdu->rport);
        tpdu->ulink = NULL;
        return -1;
    }
    else /* connecting in progress */
    { 
        ulink->privt = tpdu; 
        tpdu->ulink = ulink;
        log_info("connect to remote [%s:%d] in progress\n", tpdu->raddr, tpdu->rport);
        cp_fds_add_sock_task(dlink);
        dlink->event.events = 0;  /* clear the events flag */
    }

    return 0;
}

int tpdu_service_route(CP_SOCK *dlink)
{
    int            rv = 0;

    SWE_TPDU       *tpdu;

    if( !dlink )
        return -1;

    tpdu = dlink->privt;

    /* If it's first data incoming from the dlink, then we should 
     * malloc a TPDU work context now and save it */
    if( !tpdu ) 
    { 
        if( !(tpdu=t_malloc(sizeof(*tpdu))) ) 
        {
            log_err("Malloc TPDU work contex failure\n");
            goto stop_service;
        }
        else
        {
            SWE_TPDU       *serv_tpdu;
            log_info("malloc TPDU work contex [%p]\n", tpdu);

            serv_tpdu = dlink->serv_sock->privt;
            dlink->privt = tpdu; 
            tpdu->dlink = dlink;
            strcpy(tpdu->raddr, serv_tpdu->raddr);
            tpdu->rport = serv_tpdu->rport;
        }
    }

    log_dbg("Process socket [%d %s:%d] with event [%d]\n", dlink->fd, dlink->laddr, dlink->lport, dlink->event.events);

    /* If there is data incoming from this socket, now we receive the data */
    if(dlink->event.events & CP_SOCK_EVENT_READ)
    {
        rv = cp_sock_recv(dlink);
        if( rv == 0 )
        {
            log_warn("Socket [%d %s:%d] remote socket [%s:%d] disconnect, remove it now\n", 
                    dlink->fd, dlink->laddr, dlink->lport, dlink->raddr, dlink->rport);
            goto stop_service;
        }
        else if( rv < 0)
        {
            log_err("Socket [%d %s:%d] receive data failed: %s\n", dlink->fd, dlink->laddr, dlink->lport, strerror(errno));
            goto stop_service;
        }

        /* Receive data OK */
        log_nrml("Socket [%d %s:%d] receive %d bytes data:\n", dlink->fd, dlink->laddr, dlink->lport, rv);
        cp_log_dump(LOG_LEVEL_DEBUG, dlink->rbuf->data, dlink->rbuf->len);
    }
    else if(dlink->event.events & CP_SOCK_EVENT_HUP)
    {
        log_warn("socket [%d] remote already disconnect\n", dlink->fd);
        dlink->status = SOCK_STAT_DISCONNECT;
        goto stop_service;
    }
    else if(dlink->event.events & CP_SOCK_EVENT_IDLE_TIMEOUT)
    {
        log_warn("socket [%d] is idle and to-do something here\n");
    }
    else if(dlink->event.events & CP_SOCK_EVENT_MSG_TIMEOUT)
    {
        log_warn("socket [%d] message is timeout and to-do something here\n");
    }

    /* If we have receive some data already, now we need do TPDU parser to 
     * get the remote server address and port */
    if( dlink->rbuf->len > 0)
    {
        SWE_TPDU    *arg = dlink->serv_sock->privt;
        /* To-Do: parser downlink data here and get the uplink IP address and port */
        tpdu->rport = arg->rport;
        strncpy(tpdu->raddr, arg->raddr, sizeof(tpdu->raddr));
    }
        
    /* If not connect to the remote host or it's still connecting, then we do it again  */
    if( !tpdu->ulink || (tpdu->ulink->status==SOCK_STAT_CONNECTING))
    {
        log_nrml("tpdu dlink: %p\n", tpdu->dlink);
        if( tpdu_connect_uplink(tpdu) < 0 )
            goto stop_service;
    }

    /* If connect to the remote host already, now we send the data to the remote host  */
    if( !tpdu->ulink || (tpdu->ulink->status==SOCK_STAT_CONNECTED))
    {
        log_dbg("connect to remote [%s:%d] ok and send %d bytes data\n", tpdu->raddr, tpdu->rport, dlink->rbuf->len);
        cp_fds_del_sock_task(dlink);
        if(dlink->rbuf->len > 0)
        {
            cp_string_move(tpdu->ulink->sbuf, dlink->rbuf);
            cp_sock_send(tpdu->ulink);  /* send the data to the remote host  */
            return 0;
        }
    }

    return 0;

stop_service:
    stop_tpdu_service(tpdu);
    tpdu = NULL;
    return 0;
}

void print_usage(char *progname)
{
    version(progname); 
    printf("Usage: %s [OPTION]...\n", progname);
    printf(" %s is a handle program to create a tunnel between an incoming and outgoing FD\n", progname); 

    printf("\nMandatory arguments to long options are mandatory for short options too:\n"); 
    
    printf(" -l[loglevel]  To configure the logger level [0(DIS)...8(MAX)], default 4[NRML]\n");
    printf(" -p[port    ]  To configure the listening port, default [%d]\n", TPDUD_PORT);
    printf(" -i[RemoteIP]  To configure the remote/outgoing IP, default [%s]\n", HOSTSIM_IP);
    printf(" -o[outgoing]  To configure the remote/outgoing Port, default [%d]\n", HOSTSIM_PORT);
    printf(" -h[help    ]  Display this help information\n");
    printf(" -v[version ]  Display the program version\n");
    printf("\n"); 
    
    return ;
}


/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    CP_FDS         *fds;
    CP_SOCK        *sock;
    SWE_TPDU       tpdu = {.lport=TPDUD_PORT, .raddr=HOSTSIM_IP, .rport=HOSTSIM_PORT};
    int            opt;
    int            loglevel = LOG_LEVEL_NRML;

    struct option  long_options[] = 
    { 
        {"loglevel", required_argument, NULL, 'l'},
        {"port", required_argument, NULL, 'p'},
        {"host", required_argument, NULL, 'i'},
        {"outgoing", required_argument, NULL, 'o'},
        {"version", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };

    while ((opt = getopt_long(argc, argv, "l:p:i:o:vh", long_options, NULL)) != -1)
    { 
        switch (opt)
        {
            case 'l':
                loglevel = atoi(optarg);
                break;

            case 'p':
                tpdu.lport = atoi(optarg);
                break;
            
            case 'i':
                strncpy(tpdu.raddr, optarg, sizeof(tpdu.raddr));
                break; 
            
            case 'o': 
                tpdu.rport = atoi(optarg);
                break;

            case 'v':
                version(basename(argv[0])); 
                return EXIT_SUCCESS;

            case 'h':  
                print_usage(basename(argv[0]));
                return EXIT_SUCCESS; 
            
            default: 
                break; 
        }
    }

    //if (!cp_log_init(NULL, "tpdud.log", LOG_LEVEL_MAX, LOG_ROLLBACK_NONE) || cp_log_open())
    //if (!cp_log_init(NULL, "tpdud.log", loglevel, LOG_ROLLBACK_SIZE) || cp_log_open())
    if (!cp_log_init(NULL, DBG_LOG_FILE, LOG_LEVEL_MAX, LOG_ROLLBACK_NONE) || cp_log_open())
    //if (!cp_log_init(NULL, DBG_LOG_FILE, loglevel, LOG_ROLLBACK_NONE) || cp_log_open())
        goto cleanup;

    cp_install_proc_signal();

    if( !(fds=cp_fds_init(NULL, CP_DEF_MAX_EVENTS, CP_DEF_FDS_TIMEOUT)) )
        goto cleanup;

    if( !(sock=cp_sock_server_regist(fds, NULL, tpdu.lport, 0, tpdu_service_route, (void *)&tpdu)) )
        goto cleanup;

    while( !g_cp_signal.stop )
    {
        cp_fds_detect_event(fds);
        cp_fds_proc_event(fds);
        cp_sock_detect_timeout(fds);
        micro_second_sleep(10);
    }

cleanup:
    cp_sock_term_all_service_clear(fds);
    cp_log_term(); 
    return 0;
} /* ----- End of main() ----- */


