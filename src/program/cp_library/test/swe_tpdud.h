/********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  tpdud.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(11/19/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "11/19/2012 11:21:05 AM"
 *                 
 ********************************************************************************/

#ifndef __SWE_TPDUD_H
#define __SWE_TPDUD_H

#include <cp_sock.h>

#define HOSTSIM_PORT          9002
#define HOSTSIM_IP            "192.168.3.6"

#define TPDUD_PORT            8000
#define UPLINK_ADDR           HOSTSIM_IP
#define UPLINK_PORT           HOSTSIM_PORT

#define FLAG_TPDU_INIT        (1<<0)
#define FLAG_TPDU_STOP        (1<<7)

typedef struct _SWE_TPDU
{
    unsigned char       flag;
    char                raddr[32]; /* uplink remote listen port */
    int                 lport;     /* dlink  local listen port  */
    int                 rport;     /* uplink remote IP address  */

    CP_SOCK             *dlink;    /* listen port accept socket */
    CP_SOCK             *ulink;    /* connect to hostsim socket */
} SWE_TPDU;


#endif

