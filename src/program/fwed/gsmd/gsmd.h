/********************************************************************************
 *      Copyright:  (C) guowenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  gsmd.h
 *    Description:  This is the rmwe(Radiation Moniter Worker Engine) gsmd applet head file
 *
 *        Version:  1.0.0(02/02/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "02/02/2012 11:33:33 AM"
 *                 
 ********************************************************************************/

#ifndef __GSMD_H_
#define __GSMD_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <signal.h>

#include "cp_logger.h"
#include "cp_string.h"
#include "cp_proc.h"
#include "version.h"
#include "cp_ppp.h"
#include "cp_gprs.h"

#ifndef PID_FILE_PATH
#define PID_FILE_PATH         "/var/run/" 
#endif

#define GSMD_DEF_CONF_FILE    "/apps/rmwe/etc/gsmd.conf"
#define GSMD_DEF_LOG_FILE     "/logs/gsmd.log"

#define GSMD_DEF_LOG_LEVEL    LOG_LEVEL_NRML
#define GSMD_LOG_FILE_SIZE    12  /* Unit KiByte */

extern int gsmd_main(int argc, char **argv);

#endif /* End of __GSMD_H_ */
