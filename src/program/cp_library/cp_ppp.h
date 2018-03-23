/********************************************************************************
 *      Copyright:  (C) guowenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  ppp.h
 *    Description:  This head file is for PPP dial up thread.
 *
 *        Version:  1.0.0(02/17/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "02/17/2012 11:11:54 AM"
 *                 
 ********************************************************************************/
#ifndef __PPP_H
#define __PPP_H

#include <netinet/in.h>

#include "cp_comport.h"
#include "cp_proc.h"
#include "cp_gprs.h"

#define PPPD_DIAL_TIMEOUT             20000  /* 20 seconds */
#define PING_INTERVAL_TIME            10000  /* 10 seconds */

#define DEFAULT_PING_INTERVAL         30
#define APN_LEN                       128
#define UID_LEN                       64
#define PWD_LEN                       64
    
#define APN_3G                        3
#define APN_2G                        2
    
#define PPPD_DIAL_SCRIPTS             "/apps/tools/ifup-ppp"
#define PPP_INTERFACE_NAME            "ppp10"
#define PPP_PROC_NET_DEV              "/proc/net/dev"

#define DEF_PING_DST                  "4.2.2.2"

#define APN_DEF_CONF_FILE             "/apps/etc/ppp/default_apn.conf"

enum
{
    DISCONNECT = 0,
    SELECTED_APN,
    CONNECTING,
    CONNECTED,
};


typedef struct __APN_ACCOUNT
{   
    char            apn[APN_LEN];
    char            uid[UID_LEN];    
    char            pwd[PWD_LEN];
    char            auth[10];  /*  PAP or CHAP */
} APN_ACCOUNT;  

typedef struct __PPPD_INFO
{
    char            netaddr[INET_ADDRSTRLEN];
    char            ptpaddr[INET_ADDRSTRLEN];
    unsigned long   start_time;
    unsigned long   ping_time;
    pid_t           pid;
} PPPD_INFO;

typedef struct __PPP_STAT
{
    unsigned long long ullRx_Packets;   // total packets received 
    unsigned long long ullTx_Packets;   // total packets transmitted
    unsigned long long ullRx_Bytes; // total bytes received   
    unsigned long long ullTx_Bytes; // total bytes transmitted     

    unsigned long ulRx_Errors;  // bad packets received   
    unsigned long ulTx_Errors;  // packet transmit problems
    unsigned long ulRx_Dropped; // no space in linux buffers
    unsigned long ulTx_Dropped; // no space available in linux
    unsigned long ulRx_Multicast;   // multicast packets received   
    unsigned long ulRx_Compressed;
    unsigned long ulTx_Compressed;
    unsigned long ulCollisions;

    /* detailed rx_errors */
    unsigned long ulRx_Length_Errors;
    unsigned long ulRx_Over_Errors; // receiver ring buff overflow
    unsigned long ulRx_CRC_Errors;  // recved pkt with crc error
    unsigned long ulRx_Frame_Errors;    // recv'd frame alignment error
    unsigned long ulRx_FIFO_Errors; // recv'r fifo overrun    
    unsigned long ulRx_Missed_Errors;   // receiver missed packet  

    /* detailed tx_errors */
    unsigned long ulTx_Aborted_Errors;
    unsigned long ulTx_Carrier_Errors;
    unsigned long ulTx_FIFO_Errors;
    unsigned long ulTx_Heartbeat_Errors;
    unsigned long ulTx_Window_Errors;
} PPP_STAT;

#define MAX_PPP_FAIL_CNT             3

#define PPP_STOP                     0  /* PPP not work, but signal good */
#define SIG_WEAK                     1  /* PPP not work and signal is very weak */
#define PPP_CONN                     2  /* PPP is connecting  */
#define PPP_BAD                      3  /* PPP is connected, but network/signal not good */
#define PPP_GOOD                     4  /* PPP is connected and network/signal good */
typedef struct __PPP_CTX
{
    unsigned char      enable;       /* Enable PPP thread running or not, RFU */
    unsigned char      status;       /* Current PPP connection status */
    unsigned char      network;      /* PPP network status: PPP_FAIL,PPP_BAD,PPP_GOOD */
    char               dev[10];      /* PPP dial up device name, such as PPP10  */

    pthread_t          thread_id;    /* PPP Thread ID, RFU */

    PPPD_INFO          pppd_info;    /* pppd process information */
    PPP_STAT           ppp_stat;     /* PPP network statistic */
    unsigned char      fail_cnt;     /* PPP failure count */

    APN_ACCOUNT        apn;          /* PPP dial up APN */

    char               ping_ip[INET_ADDRSTRLEN];
    long               ping_interval;

    COM_PORT           *comport;
    MODULE_INFO        *module;
} PPP_CTX;

int get_apn_conf(char *ini_name, char *ini_key, APN_ACCOUNT *apn, unsigned char type);
int check_pppd_pid(char *find_key);
int check_ppp_interface(char *ppp_inf);
int start_ppp_dial_cmd(COM_PORT  *comport, APN_ACCOUNT *apn, char *ppp_inf);
int check_ppp_ipaddr(PPPD_INFO  *pppd_info, char *ppp_inf);
int check_ppp_stat(PPP_STAT  *ppp_stat, char *ppp_inf);
int stop_ppp_connect(PPP_CTX *ppp_ctx);

int start_thread_ppp(MODULE_INFO *module);

#endif /*  End of __PPP_H */ 
