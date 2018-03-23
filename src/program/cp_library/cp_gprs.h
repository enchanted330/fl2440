/********************************************************************************
 *      Copyright:  (C) guowenxue <guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  module.h
 *    Description:  This is the GPRS module head file
 *
 *        Version:  1.0.0(02/02/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "02/02/2012 11:33:33 AM"
 *                 
 ********************************************************************************/

#ifndef __CP_GPRS_H
#define __CP_GPRS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "cp_common.h"
#include "cp_logger.h"
#include "cp_comport.h"
#include "cp_atcmd.h"
#include "cp_time.h"
#include "cp_hal.h"

#define YES                   1
#define NO                    0

#define GSM_CTRLPORT          "/dev/gprs"
#define GSM_DATAPORT          "/dev/ttyS2"

#define MAX_DATAPORT          4  /* CMUX driver will generate 4 TTY port */
#define CMUX_PORT_START_INDEX 1
#define CMUX_DATAPORT         "/dev/gsmtty"

/*GPRS module power status control request event*/
enum
{
    REQ_EVENT_NONE = 0,
    REQ_POWER_OFF,
    REQ_POWER_ON,
    REQ_POWER_RESET,
};

/* GPRS Module Ready status */
enum
{
    NOT_READY,
    MODULE_READY,    /* GPRS AT command active and SIM card valid */
    CHECK_HWINFO,    /* Get GPRS hardware Information */
    CHECK_REGISTRY,  /* Check GPRS register network information */
    ALL_READY,
};

/* AT+CREG? command show SIM card register status */
enum
{
    REG_UNREGIST = 0, /* not registered, ME is not currently searching a new operator to register to */
    REG_HOMEWORK,     /* registered, home network */
    REG_SEARCHING,    /* not registered, but ME is currently searching a new operator to register to  */
    REG_DENIED,       /* registration denied */
    REG_UNKNOW,       /* unknow */
    REG_ROAMING,      /* registered, roaming */
};

/* AT+CNMP=? command to set GPRS module work on which mode */
#define MODE_AUTO     2   /* Automatic */
#define MODE_GSM      13  /* GSM Only */
#define MODE_WCDMA    14  /* WCDMA Only */

/* AT+CNSMOD? command show network system mode */
enum
{
    NS_NONE = 0,      /* No service */
    NS_GSM,           /* GSM */
    NS_GPRS,          /* GPRS */
    NS_EGPRS,         /* EGPRS(EDGE) */
    NS_WCDMA,         /* WCDMA */
    NS_HSDPA,         /* HSDPA only */
    NS_HSUPA,         /* HSUPA only */
    NS_HSPA,          /* HSDPA and HSUPA */
};

typedef struct __MODULE_INFO
{
    int             users;     /* How many users use the module now  */
    unsigned char   ready;     /* SIM card regist and can work or not */
    unsigned char   event;     /* Request to set GPRS power REQ_POWER_ON, REQ_POWER_OFF or REQ_POWER_RESET */
    unsigned char   pwr_status; /* Current module power status */

    pthread_mutex_t lock;      /* Module control mutex lock */

    HW_INFO         hw;        /* GPRS Hardware information */
    REGISTER_INFO   reg;       /* SIM card register network information */

    COM_PORT        *gsmport;  /* The GSM hardware UART port  */

    int             comport_cnt; /* GPRS data channel count */
    COM_PORT        *comport[MAX_DATAPORT]; /* CMUX driver generate CMUX port */
} MODULE_INFO;


extern int init_gsm_module(MODULE_INFO *module);
extern int  open_gsm_dataport(MODULE_INFO *module);
extern void close_gsm_dataport(MODULE_INFO *module);
extern COM_PORT *alloc_gsm_dataport(char *who, MODULE_INFO *module);
extern void free_gsm_dataport(char *who, MODULE_INFO *module, COM_PORT  **comport);

extern int power_on_module(MODULE_INFO *module);
extern int power_off_module(MODULE_INFO *module);

extern int atcmd_check_ready(MODULE_INFO *module);
extern int atcmd_inspect_status(MODULE_INFO *module);
extern int atcmd_check_power_on(COM_PORT *comport);
//unsigned char atcmd_inspect(MODULE_INFO *module, COM_PORT *comport);

extern void set_module_event(MODULE_INFO *module, int request);
extern int set_module_event_power(MODULE_INFO  *module);

extern void ctrl_thread_start(void *thread_arg);
extern void ctrl_thread_term(void *thread_arg);

#endif /* End of __CP_GPRS_H */
