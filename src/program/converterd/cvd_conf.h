/********************************************************************************
 *      Copyright:  (C) 2014 GuoWenxue<guowenxue@email.com>
 *                  All rights reserved.
 *
 *       Filename:  cvd_conf.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(11/19/2014)
 *         Author:  GuoWenxue <guowenxue@email.com>
 *      ChangeLog:  1, Release initial version on "11/19/2014 02:28:07 PM"
 *                 
 ********************************************************************************/
#ifndef __CVD_CONF_
#define __CVD_CONF_

#include "cvd_main.h"

#define MAX_BIND_TUNNEL    1

#define DEF_SERIAL_PORT    "/dev/ttyS1"
//#define DEF_CONN_HOST      "192.168.1.78"
#define DEF_CONN_HOST      "emblinux.com"
#define DEF_CONN_PORT      9001

#define CVD_DEF_LOG_FILE   "/var/log/converterd.log"
#define CVD_DEF_LOG_SIZE   8

extern int parser_cvd_conf(char *ini_name, cp_logger *logger, comport_sock_bind_t *cvd_bind);

#endif

