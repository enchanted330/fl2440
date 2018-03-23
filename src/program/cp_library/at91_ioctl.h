/*********************************************************************************
 *    Copyright:  (C) Guo Wenxue<guowenxue@gmail.com>
 *                All ringhts reserved.
 *
 *     Filename:  at91_ioctl.h
 *  Description:  ioctl() cmd argument for driver definition here
 *
 *      Version:  1.0.0(05/15/2012~)
 *       Author:  Guo Wenxue <guowenxue@gmail.com>
 *    ChangeLog:  1, Release initial version on "05/15/2012 04:28:09 PM"
 *
 ********************************************************************************/

#ifndef __AT91_IOCTL_H
#define __AT91_IOCTL_H

#include <asm/ioctl.h>
#ifndef __KERNEL__
#include <sys/ioctl.h>
#endif

/*===========================================================================
 *                Common ioctl command definition 
 *===========================================================================*/

#define PLATDRV_MAGIC           0x60

/*===========================================================================
 *                 ioctl command for all the drivers 0x01~0x0F
 *===========================================================================*/

/*args is enable or disable*/
#define SET_DRV_DEBUG               _IO (PLATDRV_MAGIC, 0x01)
#define GET_DRV_VER                 _IO (PLATDRV_MAGIC, 0x02)

/*===========================================================================
 *                 ioctl command for few ioctl() cmd driver 0x05~0x2F
 *===========================================================================*/

/* ADC driver */
#define GET_BATTERY_STAT            _IO (PLATDRV_MAGIC, 0x06)
#define GET_GMTUBE_VHIGH            _IO (PLATDRV_MAGIC, 0x08)
#define GET_GPRS_VBAT               _IO (PLATDRV_MAGIC, 0x09)
#define GET_ADC_DATA                _IO (PLATDRV_MAGIC, 0x10)
#define SET_ADC_INTERVEL            _IO (PLATDRV_MAGIC, 0x11)


/* Buzzer driver */
#define BUZZER_ON                   _IO (PLATDRV_MAGIC, 0x12)
#define BUZZER_BEEP                 _IO (PLATDRV_MAGIC, 0x13)
#define BUZZER_OFF                  _IO (PLATDRV_MAGIC, 0x14)
#define BUZZER_FREQ                 _IO (PLATDRV_MAGIC, 0x15)

/* Button driver */
#define GET_BUTTON_STATUS           _IO (PLATDRV_MAGIC, 0x17)

/* LED driver */
#define LED_OFF                     _IO (PLATDRV_MAGIC, 0x18)
#define LED_ON                      _IO (PLATDRV_MAGIC, 0x19)
#define LED_BLINK                   _IO (PLATDRV_MAGIC, 0x1A)
#define TURN_ALL_LED                _IO (PLATDRV_MAGIC, 0x1B)

/* Zigbee driver  */
#define ZIGBEE_RESET                _IO (PLATDRV_MAGIC, 0x1E)
#define ZIGBEE_STATUS               _IO (PLATDRV_MAGIC, 0x1F)

/*===========================================================================
 *                   ioctl command for GPS/GPRS driver 0x30~0x3F
 *===========================================================================*/
#define GSM_SET_POWER               _IO (PLATDRV_MAGIC, 0x30)  /* Set GPRS power On(1)/Off(0)/Reset(2) */
#define GSM_GET_POWER               _IO (PLATDRV_MAGIC, 0x31)  /* Get current GPRS power status */
#define GSM_GET_ADC                 _IO (PLATDRV_MAGIC, 0x32)  /* Get current GPRS power status */

#define GPS_SET_POWER               _IO (PLATDRV_MAGIC, 0x3A)  /* Set GPS power On(1)/Off(0) */
#define GPS_GET_POWER               _IO (PLATDRV_MAGIC, 0x3B)  /* Get current GPS power status */

/*===========================================================================
 *                   ioctl command for GM Tube driver 0x40~0x4F
 *===========================================================================*/
#define GM_SET_MEASURE_RADI         _IO (PLATDRV_MAGIC, 0x40)  /* Start/Stop GM tube radioation measurement */
#define GM_GET_MEASURE_DOSE         _IO (PLATDRV_MAGIC, 0x41)  /* Get sample radiation dose, arg=0:get last time, arg=1: Total*/
#define GM_SET_DUTY                 _IO (PLATDRV_MAGIC, 0x42)  /* Set GM tube PWM low duty */
#define GM_SET_INTERVAL             _IO (PLATDRV_MAGIC, 0x43)  /* Set GM tube timer interval */
#define GM_GET_VOLTAGE              _IO (PLATDRV_MAGIC, 0x44)  /* Get GM tube high voltage */

#define GM_SET_POWER                _IO (PLATDRV_MAGIC, 0x45)  /* Enable/Disable GM tube 3.3V power */
#define GM_SET_PWM_TC               _IO (PLATDRV_MAGIC, 0x46)  /* Start/Stop GM tube pwm output TC*/
#define GM_SET_MEASURE_TC           _IO (PLATDRV_MAGIC, 0x47)  /* Start/Stop GM tube measurement TC */
#define GM_SET_TIMER_TC             _IO (PLATDRV_MAGIC, 0x48)  /* Start/Stop GM tube timer TC*/

#endif                          /* End of __AT91_IOCTL_H */
