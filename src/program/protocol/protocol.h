/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<Email:guowenxue@gmail.com QQ:281143292>
 *                  All rights reserved.
 *
 *       Filename:  protocol.h
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(08/07/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "08/07/2012 11:38:06 AM"
 *                 
 ********************************************************************************/

#ifndef __PROTOCOL_H
#define __PROTOCOL_H

typedef unsigned char uchar;
extern uchar g_ucdebug;
#define dbg_print(format,args...)  if(0x00!=g_ucdebug)  { printf(format, ##args); }

typedef uchar (TEST_BODY)(uchar *pucParam, int iParmLen);
typedef struct __PROT_WORK
{
  uchar            ucCmd;
  TEST_BODY        *test_body; 
} PROT_WORK;

enum
{
   CMD_NONE,
   CMD_LED,
   CMD_USB,
   CMD_SD,
   CMD_RTC,
};

#endif
