/********************************************************************************
 *      Copyright:  (C) 2016 Guo Wenxue<guowenxue@aliyun.com>
 *                  All rights reserved.
 *
 *       Filename:  kbd_driver.h
 *    Description:  This head file is for s3c keyboard driver
 *
 *        Version:  1.0.0(07/26/2016)
 *         Author:  Guo Wenxue <guowenxue@aliyun.com>
 *      ChangeLog:  1, Release initial version on "07/26/2016 06:54:47 PM"
 *                 
 ********************************************************************************/

#ifndef  _KBD_DRIVER_H_
#define  _KBD_DRIVER_H_

/* keyboard hardware informtation structure definition */
typedef struct s3c_kbd_info_s
{
    int                     code;      /* input device key code  */
    int                     nIRQ;      /* keyboard IRQ number*/
    unsigned int            setting;   /* keyboard IRQ Pin Setting*/
    unsigned int            gpio;      /* keyboard GPIO port */
} s3c_kbd_info_t;

/* keyboard platform device private data structure */
typedef struct s3c_kbd_platform_data_s
{
    s3c_kbd_info_t         *keys;
    int                    nkeys;
} s3c_kbd_platform_data_t;

#endif   /* ----- #ifndef _KBD_DRIVER_H_  ----- */

