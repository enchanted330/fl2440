/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<Email:guowenxue@gmail.com QQ:281143292>
 *                  All rights reserved.
 *
 *       Filename:  worker.h
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(08/07/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "08/07/2012 11:38:06 AM"
 *                 
 ********************************************************************************/

#ifndef __WORKER_H
#define __WORKER_H

#include <stdio.h>
#include <unistd.h>
#include "protocol.h"

uchar test_usb(uchar *pacParm, int iParmLen);
uchar test_led(uchar *pacParm, int iParmLen);
TEST_BODY * find_body(PROT_WORK *test_list, uchar ucCmd);

#endif
