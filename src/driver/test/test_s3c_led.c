/*********************************************************************************
 *      Copyright:  (C) 2014 Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  test_s3c_led.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(03/15/2014)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "03/15/2014 02:03:22 PM"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>


#define LED_CNT        4
#define DEVNAME_LEN    10

#define PLATDRV_MAGIC             0x60
#define LED_OFF                   _IO (PLATDRV_MAGIC, 0x18)
#define LED_ON                    _IO (PLATDRV_MAGIC, 0x19)

/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    int           i;
    int           fd[LED_CNT];
    char          dev_name[DEVNAME_LEN]={0,0,0,0};

    for(i=0; i<LED_CNT; i++)
    {
        snprintf(dev_name, sizeof(dev_name), "/dev/led%d", i);
        fd[i] = open(dev_name, O_RDWR, 0755);
        if(fd[i] < 0)
            goto err;
    }

    while(1)
    {
        for(i=0; i<LED_CNT; i++)
        {
            ioctl(fd[i], LED_ON);
        }
        sleep(1);

        for(i=0; i<LED_CNT; i++)
        {
            ioctl(fd[i], LED_OFF);
        }
        sleep(1);
    }

    for(i=0; i<LED_CNT; i++)
    {
        close(fd[i]);
    }

    return 0;

err:
    for(i=0; i<LED_CNT; i++)
    {
        if(fd[i] >= 0)
        {
            close(fd[i]);
        }
    }
    return -1;
} /* ----- End of main() ----- */

