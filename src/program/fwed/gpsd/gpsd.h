/********************************************************************************
 *      Copyright:  (C) guowenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  gpsd.h
 *    Description:  This is the rmwe(Radiation Moniter Worker Engine) gpsd applet head file
 *
 *        Version:  1.0.0(02/02/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "02/02/2012 11:33:33 AM"
 *                 
 ********************************************************************************/

#ifndef __GPSD_H_
#define __GPSD_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <sys/types.h>
#include <signal.h>

#include "cp_comport.h"
#include "cp_logger.h"
#include "cp_proc.h"
#include "nmea_info.h"
#include "version.h"

#ifndef PID_FILE_PATH
#define PID_FILE_PATH         "/var/run/" 
#endif

#define GPSD_DEF_LOG_FILE     "/logs/gpsd.log"
#define GPSD_DEF_LOG_LEVEL    LOG_LEVEL_NRML
#define GPSD_LOG_FILE_SIZE    12  /* Unit KiByte */

#define GPS_DATAPORT          "/dev/ttyS4"

#define POWER_OFF             0
#define POWER_ON              1

typedef struct __GPSD_CONTEX
{
    unsigned char     pwr_status;
    nmeaINFO          info;
    COM_PORT          *comport;
} GPSD_CONTEX;

extern int gpsd_main(int argc, char **argv);

#endif /* End of __GPSD_H_ */
