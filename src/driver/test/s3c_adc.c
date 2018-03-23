/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  adc_test.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(04/01/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "04/01/2012 02:51:19 PM"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <errno.h>
#include "plat_ioctl.h"
int main(void)
{
    int adc_fd;
    int adc_value, adc_size;
    int i;

    adc_fd = open("/dev/adc", O_RDONLY);
    if (adc_fd < 0)
    {
        perror("open device adc");
        exit(1);
    }

    adc_size = read(adc_fd, &adc_value, sizeof(adc_value));
    printf("adc default channel value : %d\n", adc_value);
    for (i = 0; i < 4; i++)
    {
        ioctl(adc_fd, ADC_SET_CHANNEL, &i);
        adc_size = read(adc_fd, &adc_value, sizeof(adc_value));
        printf("adc  channel %d value : %d\n", i, adc_value);
    }

    close(adc_fd);
    return 0;
}
