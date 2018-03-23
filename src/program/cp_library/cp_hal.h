/********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  cp_hal.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(12/05/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "12/05/2012 02:21:55 PM"
 *                 
 ********************************************************************************/
#ifndef __CP_HAL_H
#define __CP_HAL_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "at91_ioctl.h"

#define DEV_LED_PATH            "/dev/led"
#define DEV_GSM_PATH            "/dev/gprs"
#define DEV_GPS_PATH            "/dev/gps"
#define DEV_ZIGBEE_PATH         "/dev/zigbee"
#define DEV_GMTUBE_PATH         "/dev/gmtube"

#define OFF                     0
#define ON                      1
#define BLINK                   2
#define RESET                   2

#define MODE_SLOW               0
#define MODE_FAST               1

#define START                   1
#define STOP                    0

#define LED_SYS                 0   

extern int hal_turn_led_on(int led);
extern int hal_turn_led_off(int led);
extern int hal_turn_led_blink(int led, int mode);

extern int hal_turn_gprs_power(int status);
extern int hal_get_gprs_power(void);

extern int hal_turn_gps_power(int status);
extern int hal_get_gps_power(void);

#endif /* Endof __CP_HAL_H  */
