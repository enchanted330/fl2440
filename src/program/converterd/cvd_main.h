/********************************************************************************
 *      Copyright:  (C) 2014 GuoWenxue<guowenxue@email.com>
 *                  All rights reserved.
 *
 *       Filename:  cvd_main.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(11/19/2014)
 *         Author:  GuoWenxue <guowenxue@email.com>
 *      ChangeLog:  1, Release initial version on "11/19/2014 02:35:33 PM"
 *                 
 ********************************************************************************/

#ifndef __CVD_MAIN_H_
#define __CVD_MAIN_H_

#include "cp_comport.h"
#include "cp_socket.h"

#define CVD_DEF_CONF_FILE  "/apps/etc/converterd.conf"

#define MAXEPOLLSIZE       2
#define BUF_SIZE           4096

#define EPOLL_TIMEOUT      2000
#define COMPORT_TIMEOUT    100


typedef struct comport_sock_bind_s
{
    char               buf[BUF_SIZE];              /*  Comport/Socket receive buffer  */ 
    char               comport_dev[DEVNAME_LEN];   /*  Receive/Transmit comport device name  */

    COM_PORT           *comport;                   /*  Receive/Transmit comport */ 
    cp_sock_t          sock;                       /*  Connected socket */
} comport_sock_bind_t;  /* ---  end of struct comport_sock_bind_s  ---*/



#endif
