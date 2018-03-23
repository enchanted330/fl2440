/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  cp_network.c
 *    Description:  This file is the high level network library API
 *                 
 *        Version:  1.0.0(10/31/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "10/31/2012 11:22:38 AM"
 *                 
 ********************************************************************************/
#include <netinet/in.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h>

#include "cp_sock.h"
#include "cp_fds.h"
#include "cp_network.h"
#include "cp_network.h"
#include "cp_proc.h"

/*  Description: Configure the NIC MAC/IP address
 *   Input args: $nic: configure for which NIC(Network Interface Card): eth0, eth1 
 *               $mac: MAC address        $ip: IP address
 * Return Value: 0: Test OK, <0: Failed
 */
int nic_network_config(char *nic, char *mac, char *ip)
{
    if(!nic)
        return -1;

    /*MAC address format: aa:bb:cc:dd:ee:ff */
    if(mac && 17==strlen(mac))
    {
        /* shutdown the ethernet and clear the original IP address  */
        exec_system_cmd("ifconfig %s down", nic, nic);
        sleep(1);
        exec_system_cmd("ifconfig %s hw ether %s up", nic, mac);
    }

    /* IP address specified and valid  */
    if(ip && inet_addr(ip))
    {
        exec_system_cmd("ifconfig %s down && ifconfig %s 0.0.0.0", nic, nic);
        sleep(2);
        exec_system_cmd("ifconfig %s %s up", nic, ip);
    }

    return 0;
}


int nic_get_ipaddr(char *nic, char *ipaddr)
{
    int                   fd;           
    struct ifreq          ifr;          
    struct sockaddr_in    *sin;         

    if( !nic || !ipaddr )
        return -1;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    strcpy(ifr.ifr_name, nic);

    if ((ioctl(fd, SIOCGIFADDR, (caddr_t)&ifr, sizeof(struct ifreq))) < 0)
    {  
        log_fatal("System call ioctl() with SIOCGIFADDR get %s IP address failure.\n", nic);
        return -2;
    }                                   

    sin = (struct sockaddr_in *)&ifr.ifr_addr;
    strcpy(ipaddr, (const char *)inet_ntoa(sin->sin_addr)); 

    log_dbg("Get %s local IP address: %s\n", nic, ipaddr); 

    return 0;
}

int nic_get_ptpaddr(char *nic, char *ptpaddr)
{
    int                   fd;           
    struct ifreq          ifr;          
    struct sockaddr_in    *sin;         

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    strcpy(ifr.ifr_name, nic);

    if ((ioctl(fd, SIOCGIFDSTADDR, (caddr_t) & ifr, sizeof(struct ifreq))) < 0)
    {
        log_fatal("System call ioctl() with SIOCGIFDSTADDR get %s IP address failure.\n", nic);
        return -1;
    }

    sin = (struct sockaddr_in *)&ifr.ifr_dstaddr;                                                                                   
    strcpy(ptpaddr, (const char *)inet_ntoa(sin->sin_addr));
    log_dbg("Get %s remote IP address: %s\n", nic, ptpaddr);

    return 0;
}


/*  Description: Use ping do network test
 *   Input args: $from: : use which NIC(Network Interface Card: eth0, eth1) or 
 *               source IP address do ping test. if it's NULL, use default route
 *               $ping_ip:  The ping test destination IP address
 *  Output args: NONE
 * Return Value: >=0 ping test packet lost percent, <0: failure
 */
int network_ping_test(char *from, char *ping_ip)
{
    FILE                 *fp;
    char                 cmd[256];
    char                 buf[512];
    int                  lost_percent = 100;
    unsigned long        tx_packets = 0;
    unsigned long        rx_packets = 0;

    memset(cmd, 0, sizeof(cmd));
    if(from)
    {
        snprintf(cmd, sizeof(cmd), "ping -W1 -c5 -s4 %s -I %s", ping_ip, from);
    }
    else
    {
        snprintf(cmd, sizeof(cmd), "ping -W1 -c5 -s4 %s", ping_ip);
    }

    if (NULL == (fp = popen(cmd, "r")))
    {
        return -2;
    }

    while (NULL != fgets(buf, sizeof(buf), fp))
    {
        if (strstr(buf, "transmitted"))
        {
            split_string_to_value(buf, "%l,%l,%d", &tx_packets, &rx_packets, &lost_percent);
            break;
        }
    }

    pclose(fp);
    return lost_percent;
}


int cp_sock_accept_regist(CP_SOCK *serv_sock)
{
    CP_SOCK                *new_sock = NULL;
    int                    rv = 0;

    if(!serv_sock)
    {
        log_err("Invalude input arguments\n");
        return -1;
    }

    log_dbg("Accept and regist new client from server [%d] on [%s:%d] now\n", 
            serv_sock->fd, serv_sock->laddr, serv_sock->lport);

    if(serv_sock->accept_cnt >= serv_sock->max_client)
    {
        log_err("No more client resource aviable on server socket [%d]\n", serv_sock->fd);
        goto exit_out;
    }

    if( !(new_sock=cp_sock_init(NULL, CP_SOCK_RCVBUF, CP_SOCK_SNDBUF, CP_SOCK_KEEPINTVL, CP_SOCK_KEEPCNT)) )
    {
        log_err("Create a new socket contex for accept new client failed.\n");
        goto exit_out;
    }

    new_sock->serv_sock = serv_sock;
    new_sock->fds = serv_sock->fds;
    new_sock->privt = serv_sock->privt;
    new_sock->cbfunc = serv_sock->client_func; 
    new_sock->mode = CP_SOCK_MODE_ACCEPT;

    /* add accept new client into the socket registry list */
    if( cp_fds_add_sock_registry(new_sock) )
    {
        rv = -2;
        goto term_sock;
    }

    /* accept the new client from server socket */
    if( cp_sock_accept(serv_sock, new_sock) )
    {
        rv = -3;
        goto  del_regist;
    }

    if( cp_add_epoll_event(new_sock) )
    {
        rv =  -4;
        goto del_regist;
    }
    
    log_dbg("Accept and regist new client from server [%d] on [%s:%d] ok\n", 
            serv_sock->fd, serv_sock->laddr, serv_sock->lport);

    return 0;

del_regist:
    cp_fds_del_sock_registry(new_sock);

term_sock:
    if(new_sock) 
        cp_sock_term_clear(new_sock);

exit_out:
    log_err("Accept and regist new client from server [%d] on [%s:%d] failed\n", 
            serv_sock->fd, serv_sock->laddr, serv_sock->lport);

    cp_fds_del_sock_task(serv_sock);
    return rv;
}

CP_SOCK *cp_sock_connect_regist(CP_FDS *fds, CP_SOCK *sock, char *raddr, int rport, int lport, CP_SOCK_EVENT_CALLBACK service_route)
{
    if(!fds || !raddr || rport<0)
    {
        log_err("Invalude input arguments\n");
        goto exit_out;
    }

    if( !sock && !(sock=cp_sock_init(sock, CP_SOCK_RCVBUF, CP_SOCK_SNDBUF, CP_SOCK_KEEPINTVL, CP_SOCK_KEEPCNT)) )
    { 
        goto exit_out;
    }

    if( SOCK_STAT_CONNECTED == sock->status )
        return sock; /* already ok now  */

    if( !(sock->flag&FLAG_SOCK_REGISTRY) ) 
    {   
        sock->fds = fds;
        sock->cbfunc = service_route;
        sock->mode = CP_SOCK_MODE_CONNECT;
        strncpy(sock->laddr, "0.0.0.0", sizeof(sock->laddr));
        log_dbg("Connect and regist socket to remote [%s:%d] now\n", raddr, rport);

        if( cp_fds_add_sock_registry( sock) )
            goto term_sock; /* regist to the list failed */
    }   

    if ( cp_sock_connect(sock, raddr, rport, lport) )
    {
        log_dbg("socket [%d] connect failed, start to remove registry it now.\n", sock->fd);
        goto del_regist;
    }
    else
    {
        if( cp_add_epoll_event(sock) )
            goto del_regist;
    }

    return sock;

del_regist:
    cp_fds_del_sock_registry(sock);

term_sock:
    cp_fds_del_sock_task(sock);
    if(sock)
        cp_sock_term(sock); 

exit_out:
    log_err("Connect and regist new socket to remote server [%s:%d] failed\n", raddr, rport);
    return NULL;
}

CP_SOCK *cp_sock_server_regist(CP_FDS *fds, char *addr, int port, int max_client, CP_SOCK_EVENT_CALLBACK service_route, void *data)
{
    int  rv = 0;

    CP_SOCK   *sock = NULL;

    log_dbg("Regist socket server listen on [%s:%d] now\n", addr?addr:"local", port);

    if(!fds || port < 0)   
    {
        rv = -1;
        log_err("Invalude input arguments\n");
        goto exit_out;
    }

    if( !(sock=cp_sock_init(NULL, CP_SOCK_RCVBUF, CP_SOCK_SNDBUF, CP_SOCK_KEEPINTVL, CP_SOCK_KEEPCNT)) )
    {
        rv = -2;
        goto exit_out;
    }

    INIT_LIST_HEAD(&sock->accept_list);
    sock->fds = fds;
    sock->cbfunc = cp_sock_accept_regist;
    sock->client_func = service_route;
    sock->privt = data;
    sock->mode = CP_SOCK_MODE_LISTEN;

    if( cp_fds_add_sock_registry(sock) )
    {
        rv = -3;
        goto term_sock;
    }

    /* If specified maximum clients <=0, then use the default one */
    sock->max_client = max_client<=0 ? CP_DEF_MAX_CLIENTS : max_client;

    /*  If specified maximum clients larger than FDS monitor maxevents, then use the default one */
    sock->max_client = max_client<fds->max_event ? sock->max_client : CP_DEF_MAX_CLIENTS;

    if( cp_sock_listen(sock, addr, port, sock->max_client) )
    {
        rv = -4;
        goto del_regist;
    }

    /* Add server socket FD to the epoll monitor list */
    if( cp_add_epoll_event(sock) )
    {
        rv = -5;
        goto del_regist;
    }

    log_nrml("Registe socket[%d] server listen on [%s:%d] ok.\n", sock->fd, sock->laddr, sock->lport);
    return sock;

del_regist:
    cp_fds_del_sock_registry(sock);

term_sock:
    cp_fds_del_sock_task(sock);
    if(sock)
        cp_sock_term_clear(sock);

exit_out:
    log_err("Registe socket server listen on [%s:%d] failed.\n", addr, port);
    return NULL;
}

void cp_sock_term_all_service(CP_FDS *fds)
{
    if(!fds)
        return;

    log_dbg("Terminate all the service now\n");

    cp_sock_term_all_task(fds);
    cp_sock_term_all_server(fds);
    cp_sock_term_all_client(fds);

    cp_fds_term(fds);
    log_warn("Terminate all the service ok\n");
}

