/********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  cp_network.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(10/31/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "10/31/2012 11:24:06 AM"
 *                 
 ********************************************************************************/
#ifndef __CP_NETWORK_H
#define __CP_NETWORK_H

#include "cp_fds.h"
#include "cp_sock.h"

#define CP_DEF_MAX_CLIENTS           1024
#define CP_DEF_TIMEOUT               10

extern int nic_network_config(char *nic, char *mac, char *ip);
extern int nic_get_ipaddr(char *nic, char *ipaddr);
extern int nic_get_ptpaddr(char *nic, char *ptpaddr);
extern int network_ping_test(char *from, char *ping_ip);

extern int cp_sock_accept_regist(CP_SOCK *serv_sock);

extern CP_SOCK *cp_sock_connect_regist(CP_FDS *fds, CP_SOCK *sock, char *raddr, int rport, int lport, CP_SOCK_EVENT_CALLBACK service_route);

extern CP_SOCK *cp_sock_server_regist(CP_FDS *fds, char *addr, int port, int max_client, CP_SOCK_EVENT_CALLBACK service_route, void *data);

extern void cp_sock_term_all_service(CP_FDS *fds);
#define cp_sock_term_all_service_clear(fds) {cp_sock_term_all_service(fds); fds=NULL;}

#endif

