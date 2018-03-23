/********************************************************************************
 *      Copyright:  (C) guowenxue <guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  atcmd.h
 *    Description:  This is the head file for atcmd.c
 *
 *        Version:  1.0.0(05/15/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "05/15/2012 05:31:10 PM"
 *                 
 ********************************************************************************/

#ifndef __CP_ATCMD_H
#define __CP_ATCMD_H

#include "cp_comport.h"

/* AT+CPSI command return important result*/
typedef struct _GPRS_LOCATION
{
    char            longitude[15];
    char            latitude[15];
    char            date[15];
    char            time[15];
    char            mcc_mnc[16];/*   Mobile Country Code, China is 460 */
} GPRS_LOCATION;

typedef struct _REGISTER_INFO
{
    int             type;       /*  SIM card register type: REG_HOMEWORK,REG_SEARCHING... */
    int             signal;     /*  GPRS signal */ 
    char            carrier[64];/*  Network operator */
    GPRS_LOCATION   loc;        /*  Location */
} REGISTER_INFO;


typedef struct _HARDWARE_INFO
{
    char            model[64];  /*  AT+CGMM check GPRS module model */
    char            mrev[64];   /*  AT+CGMR check GPRS module revision */
    char            iemi[64];   /*  AT+CGSN check GPRS module IEMI(International Mobile station Equipment Identity) number */
} HW_INFO;

extern int send_atcmd(COM_PORT *st_comport, char *atCmd, char *expect, char *error, 
       unsigned long timeout, char *reply, int reply_len);

extern int send_atcmd_check_ok(COM_PORT *comport, char *atcmd, unsigned long timeout);
extern int send_atcmd_check_value(COM_PORT *comport, char *atcmd, unsigned long timeout, char *buf, int buf_len);
extern int send_atcmd_check_request(COM_PORT *comport, char *atcmd, unsigned long timeout, char *buf, int buf_len);


extern int atcmd_check_at_ready(COM_PORT *comport); /* Send ATE0 command */
extern int atcmd_check_sim_valid(COM_PORT *comport); /* Send AT+CPIN? command */
extern int atcmd_check_gprs_signal(COM_PORT *comport); /* Send AT+CSQ command */
extern int atcmd_check_gprs_register(COM_PORT *comport);
extern int atcmd_check_gprs_carrier(COM_PORT *comport, char *carrier);
extern int atcmd_check_gprs_name(COM_PORT *comport, char *name);
extern int atcmd_check_gprs_version(COM_PORT *comport, char *version);
extern int atcmd_check_gprs_iemi(COM_PORT *comport, char *iemi);
extern int atcmd_check_gprs_network(COM_PORT *comport, int *network);
extern int atcmd_check_gprs_location(COM_PORT *comport, GPRS_LOCATION *location);
extern int atcmd_check_gprs_mcc_mnc(COM_PORT *comport, char *mcc_mnc);

extern int atcmd_set_network_mode(COM_PORT *comport, int mode);
extern int atcmd_set_apn(COM_PORT *comport, char *apn);

#endif /* _CP_ATCMD_H */
