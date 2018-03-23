/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  cp_led_buzzer.c
 *    Description:  This file is is the HAL(Hardware Abstraction Layer) for 
 *                  AT91SAM9xxx family devices.
 *                 
 *        Version:  1.0.0(12/05/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "12/05/2012 01:26:33 PM"
 *                 
 ********************************************************************************/

#include "cp_hal.h"

int hal_turn_led_on(int  led)
{
    int         fd = -1;

    if((fd=open(DEV_LED_PATH, O_RDWR)) < 0)
    {
        return -1;
    }

    ioctl(fd, LED_ON, led);

    close(fd);
    return 0;
}

int hal_turn_led_off(int led)
{
    int         fd = -1;

    if((fd=open(DEV_LED_PATH, O_RDWR)) < 0)
    {
        return -1;
    }

    ioctl(fd, LED_OFF, led);

    close(fd);
    return 0;
}

int hal_turn_led_blink(int led, int mode)
{
    int         fd = -1;
    int         arg = led|(mode<<3);

    if((fd=open(DEV_LED_PATH, O_RDWR)) < 0)
    {
        return -1;
    }

    ioctl(fd, LED_BLINK, arg);

    close(fd);
    return 0;
}


/*
 * Description: Turn GPRS ON or OFF
 *  Input Args: RESET(2)/ON(1)/OFF(0)
 */
int hal_turn_gprs_power(int status)
{
    int         fd = -1;
    int         rv = 0;

    if((fd=open(DEV_GSM_PATH, O_RDWR)) < 0)
    {
        return -1;
    }

    if( (rv=ioctl(fd, GSM_SET_POWER, status)) < 0)
    {
        close(fd);
        return -2;
    }

    close(fd);
    return rv;
}

/*
 *  Description: Get current GPRS power status
 * Return Value: ON(1)/OFF(0) 
 */
int hal_get_gprs_power(void)
{
    int         fd = -1;
    int         status;

    if((fd=open(DEV_GSM_PATH, O_RDWR)) < 0)
    {
        return -1;
    }

    if( (status=ioctl(fd, GSM_GET_POWER, 0)) < 0)
    {
        close(fd);
        return -2;
    }

    close(fd);
    return status;
}

/*
 * Description: Turn GPS ON or OFF
 *  Input Args: ON(1)/OFF(0) 
 */
int hal_turn_gps_power(int status)
{
    int         fd = -1;
    int         rv = 0;

    if((fd=open(DEV_GPS_PATH, O_RDWR)) < 0)
    {
        return -1;
    }

    if( (rv=ioctl(fd, GPS_SET_POWER, status)) < 0)
    {
        close(fd);
        return -2;
    }

    close(fd);
    return rv;
}

/*
 *  Description: Get current GPS power status
 * Return Value: ON(1)/OFF(0) 
 */
int hal_get_gps_power(void)
{
    int         fd = -1;
    int         status;

    if((fd=open(DEV_GPS_PATH, O_RDWR)) < 0)
    {
        return -1;
    }

    if( (status=ioctl(fd, GPS_GET_POWER, 0)) < 0)
    {
        close(fd);
        return -2;
    }

    close(fd);
    return status;
}


/*
 * Description: Reset zigbee module
 */
int hal_reset_zigbee(void)
{
    int         fd = -1;

    if((fd=open(DEV_ZIGBEE_PATH, O_RDWR)) < 0)
    {
        return -1;
    }

    if( ioctl(fd, ZIGBEE_RESET, 0) < 0)
    {
        close(fd);
        return -2;
    }

    close(fd);
    return 0;
}

/*
 *  Description: Get Zigbee status
 */
int hal_get_zigbee_status(void)
{
    int         fd = -1;
    int         status;

    if((fd=open(DEV_ZIGBEE_PATH, O_RDWR)) < 0)
    {
        return -1;
    }

    if( (status=ioctl(fd, ZIGBEE_STATUS, 0)) < 0)
    {
        close(fd);
        return -2;
    }

    close(fd);
    return status;
}

/*
 * Description: Start/Stop Radiation Moniter sample
 *  Input Args: Start(1)/Stop(0) the sample 
 *  Return Val: If status=1(start), it will return this time sample dose
 *              If status=0(stop), it will return last time sample dose
 */
int hal_set_gmtube(status)
{
    int         fd = -1;
    int         dose = 0;

    if((fd=open(DEV_GMTUBE_PATH, O_RDWR)) < 0)
    {
        return -1;
    }

    if( (dose=ioctl(fd, GM_SET_MEASURE_RADI, status)) < 0)
    {
        close(fd);
        return -2;
    }

    close(fd);
    return dose;
}


/*
 * Description: Start/Stop Radiation Moniter sample
 *  Input Args: Start(1)/Stop(0) the sample 
 *  Return Val: Return last time sample dose
 */
int hal_get_gmtube_dose(void)
{
    int         fd = -1;
    int         dose = 0;

    if((fd=open(DEV_GMTUBE_PATH, O_RDWR)) < 0)
    {
        return -1;
    }

    if( (dose=ioctl(fd, GM_GET_MEASURE_DOSE, 0)) < 0)
    {
        close(fd);
        return -2;
    }

    close(fd);
    return dose;
}

