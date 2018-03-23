/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  test_client_sock.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(10/30/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "10/30/2012 01:15:18 PM"
 *                 
 ********************************************************************************/

#include "cp_network.h"
#include "cp_logger.h"
#include "cp_proc.h"
#include "cp_time.h"


//#define SERV_ADDR              "192.168.1.175"
#define SERV_ADDR              "192.168.1.78"
//#define SERV_ADDR              "kkernel.oicp.net"
#define SERV_PORT              8880

int service_route(CP_SOCK *sock)
{
    int      rv;

    if( !sock ) 
        return -1;

    printf("Client socket [%d] get event: %d\n", sock->fd, sock->event.events);
    if(sock->event.events & EPOLLIN)
    {
        rv = cp_sock_recv(sock);
        if(!rv)
        {
            log_nrml("Socket [%d %s:%d] remote socket [%s:%d] disconnect, remove it now\n",
                    sock->fd, sock->laddr, sock->lport, sock->raddr, sock->rport);

            cp_fds_destroy_sock(sock);
            g_cp_signal.stop = 1;

            return 0;
        }
        else if(rv > 0)
        {
            log_nrml("Socket [%d %s:%d] receive [%d] bytes data:\n", sock->fd, sock->laddr, sock->lport, sock->rbuf->len);
            cp_log_dump(LOG_LEVEL_DEBUG, sock->rbuf->data, sock->rbuf->len);
            cp_string_clear_data(sock->rbuf);
        }
    }

    return 0;
}


/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    CP_FDS     *fds;
    CP_SOCK    *sock = NULL;

    if ( !cp_log_init(NULL, DBG_LOG_FILE, LOG_LEVEL_NRML, LOG_ROLLBACK_NONE) || cp_log_open() )
    //if ( !cp_log_init(NULL, DBG_LOG_FILE, LOG_LEVEL_MAX, LOG_ROLLBACK_NONE) || cp_log_open() )
        return -1;

    cp_install_proc_signal();

    if( !(fds=cp_fds_init(NULL, CP_DEF_MAX_EVENTS, CP_DEF_FDS_TIMEOUT)) )
        return -2;

    while( !g_cp_signal.stop )
    {
        sock = cp_sock_connect_regist(fds, sock, SERV_ADDR, SERV_PORT, 0, service_route);
        if( sock && sock->status==SOCK_STAT_CONNECTING)
        {
            /* Connect is in progress */
            sleep(1);
            continue;
        }
        else if(!sock || sock->status!=SOCK_STAT_CONNECTED)
        {
            break;
        }

        cp_string_cstrcpy(sock->sbuf, "Hello World!\n");
        printf("Send buffer data: %s", cp_string_data(sock->sbuf));
        cp_sock_send(sock);

        cp_fds_detect_event(fds);

        cp_fds_proc_event(fds);

        micro_second_sleep(100);
    }

    cp_sock_term_all_service_clear(fds);

    cp_log_term();

    return 0;
} /* ----- End of main() ----- */


