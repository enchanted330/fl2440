/*********************************************************************************
 *      Copyright:  (C) 2013 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  cp_gsmmux.c
 *    Description:  This file is the application for GSM0710(A.K.A CMUX) protocal 
 *                  which implement in Linux kernel 
 *                 
 *        Version:  1.0.0(01/16/2013~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "01/16/2013 05:37:15 PM"
 *                 
 ********************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include "cp_logger.h"
#include "cp_gsmmux.h"

int attach_gsm0710(COM_PORT *comport)
{
    int                    ldisc = N_GSM0710;
    struct gsm_config      c;

    log_nrml("Attach GMUX on GSM port \"%s\" now\n", comport->dev_name);

    if( send_atcmd_check_ok(comport, "AT+CMUX=0\r", 3000) )
    {
        log_err("Send \"AT+CMUX=0\" command check %s failure\n", comport->dev_name);
        return -1;
    } 

    if (ioctl(comport->fd, TIOCSETD, &ldisc) < 0)
    {
        log_err("Can not attach N_GSM0710 to comport \"%s\" \n", comport->dev_name);
        return -2;
    }


    ioctl(comport->fd, GSMIOC_GETCONF, &c);

    /*  we are initiator and need encoding 0 (basic) */
    c.initiator = 1;
    c.encapsulation = 0;
    
    /* our modem defaults to a maximum size of 127 bytes */
    c.mru = 127;
    c.mtu = 127; 
    
    /* set the new configuration */
    ioctl(comport->fd, GSMIOC_SETCONF, &c);

    log_nrml("Attach GMUX on GSM port \"%s\" OK\n", comport->dev_name);
    sleep(1);

    return 0;
}


#if 0
int detach_gsm0710(COM_PORT *comport)
{
    log_nrml("Dettach GMUX on GSM port \"%s\" now\n", comport->dev_name);
    return 0;
}
#endif
