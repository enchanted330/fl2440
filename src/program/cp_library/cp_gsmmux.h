/********************************************************************************
 *      Copyright:  (C) 2013 Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  cp_gsmmux.h
 *    Description:  This head file is application for GSM0710(A.K.A CMUX) protocal 
 *                  which implement in Linux kernel
 *
 *        Version:  1.0.0(01/16/2013~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "01/16/2013 05:43:48 PM"
 *                 
 ********************************************************************************/

#ifndef __CP_GSMMUX_H_
#define __CP_GSMMUX_H_

#include "cp_atcmd.h"

struct gsm_config
{
    unsigned int adaption;
    unsigned int encapsulation;
    unsigned int initiator;
    unsigned int t1;
    unsigned int t2;
    unsigned int t3;
    unsigned int n2;
    unsigned int mru;
    unsigned int mtu;
    unsigned int k;
    unsigned int i;
    unsigned int unused[8];   /* Padding for expansion without breaking stuff */
};

#define GSMIOC_GETCONF      _IOR('G', 0, struct gsm_config)
#define GSMIOC_SETCONF      _IOW('G', 1, struct gsm_config)

#define N_GSM0710           21  /*   GSM 0710 Mux */

int attach_gsm0710(COM_PORT *comport);
//int detach_gsm0710(COM_PORT *comport);

#endif /* __CP_GSMMUX_H_  */
