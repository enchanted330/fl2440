/********************************************************************************
 *      Copyright:  (C) guowenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  zigd.h
 *    Description:  This is the rmwe(Radiation Moniter Worker Engine) zigd applet head file
 *
 *        Version:  1.0.0(02/02/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "02/02/2012 11:33:33 AM"
 *                 
 ********************************************************************************/

#ifndef __ZIGD_H_
#define __ZIGD_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <signal.h>

#include "cp_comport.h"
#include "cp_logger.h"
#include "cp_string.h"
#include "cp_proc.h"
#include "version.h"

#ifndef PID_FILE_PATH
#define PID_FILE_PATH         "/var/run/" 
#endif

#define ZIGD_DEF_LOG_FILE     "/logs/zigd.log"
#define ZIGD_DEF_LOG_LEVEL    LOG_LEVEL_NRML
#define ZIGD_LOG_FILE_SIZE    12  /* Unit KiByte */

#define ZIGD_DATAPORT         "/dev/ttyS1"

#define POWER_OFF             0
#define POWER_ON              1

typedef struct __ZIGD_CONTEX
{
    unsigned char     pwr_status;
    COM_PORT          *comport;
} ZIGD_CONTEX;

extern int zigd_main(int argc, char **argv);

#endif /* End of __ZIGD_H_ */
