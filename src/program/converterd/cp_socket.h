/********************************************************************************
 *      Copyright:  (C) 2014 Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  cp_socket.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(11/19/2014)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "11/19/2014 12:16:45 AM"
 *                 
 ********************************************************************************/
#ifndef __CP_SOCKET_H_
#define __CP_SOCKET_H_

#include <sys/types.h> 
#include <sys/socket.h>

#define DOMAIN_MAX_LEN             128

#define SOCK_STAT_INIT             0
#define SOCK_STAT_CONNECTING       1
#define SOCK_STAT_CONNECTED        2
#define SOCK_STAT_DISCONNECT       3

typedef struct cp_sock_s 
{
    char               host[DOMAIN_MAX_LEN];      /*  Connect/Listen hostname or IP address */
    int                port;                      /*  Connect/Listen server port  */
    int                fd;                        /*  Connected/Listen socket fd  */ 
    int                status;                    /*  Socket connected or not  */
    struct sockaddr    saddr;                     /*  sockaddr for none-block connect */

} cp_sock_t;  /*---  end of struct cp_sock_s  ---*/

extern int cp_sock_connect(cp_sock_t *sock);
extern void cp_sock_close(cp_sock_t *sock);
extern void cp_sock_term(cp_sock_t *sock);
#endif

