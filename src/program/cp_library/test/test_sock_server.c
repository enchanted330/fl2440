/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  test_sock_server.c
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


#define SERV_PORT1            8880
#define SERV_PORT2            8881

#define DOMAIN                "kkernel.oicp.net"
#define IPADDR                "192.168.1.78"

int service_route(CP_SOCK *sock)
{
    int           rv = 0;
    log_nrml("Process socket [%d %s:%d] get event [%d]\n", sock->fd, sock->laddr, sock->lport, sock->event.events);

    if(sock->event.events & EPOLLIN)
    {
        rv = cp_sock_recv(sock);
        if(0==rv)
        {
            log_nrml("Socket [%d %s:%d] remote socket [%s:%d] disconnect, remove it now\n", 
                    sock->fd, sock->laddr, sock->lport, sock->raddr, sock->rport);

            cp_fds_destroy_sock(sock);

            return 0;
        }
        else if(rv > 0)
        {
            log_nrml("Socket[%d] recv buffer %d bytes data:\n", sock->fd, sock->rbuf->len);
            //cp_log_dump(LOG_LEVEL_NRML, sock->rbuf->data, sock->rbuf->len);

            cp_string_move(sock->sbuf, sock->rbuf);

            log_nrml("Socket[%d] send buffer %d bytes data:\n", sock->fd, sock->sbuf->len);
            cp_sock_send(sock);
            cp_string_clear_data(sock->sbuf);
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
    CP_SOCK    *sock;

    //if (!cp_log_init(NULL, DBG_LOG_FILE, LOG_LEVEL_NRML, LOG_ROLLBACK_NONE) || cp_log_open())
    if (!cp_log_init(NULL, DBG_LOG_FILE, LOG_LEVEL_MAX, LOG_ROLLBACK_NONE) || cp_log_open())
        goto cleanup;

    cp_install_proc_signal();

    if( !(fds=cp_fds_init(NULL, CP_DEF_MAX_EVENTS, CP_DEF_FDS_TIMEOUT)) )
        goto cleanup;

    if( !(sock=cp_sock_server_regist(fds, NULL, SERV_PORT1, 0, service_route, NULL)) )
        goto cleanup;

    if( !(sock=cp_sock_server_regist(fds, NULL, SERV_PORT2, 0, service_route, NULL)) )
        goto cleanup;

    while( !g_cp_signal.stop )
    { 
        cp_fds_detect_event(fds);
        cp_fds_proc_event(fds);
        micro_second_sleep(10);
    }

cleanup:
    cp_sock_term_all_service_clear(fds);
    cp_log_term();
    return 0;
} /* ----- End of main() ----- */


