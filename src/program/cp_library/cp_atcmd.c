/*********************************************************************************
 *      Copyright:  (C) guowenxue <guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  cp_atcmd.c 
 *    Description:  This is file is used to send AT command to GPRS module. 
 *                 
 *        Version:  1.0.0(02/02/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "02/02/2012 10:28:44 AM"
 *                 
 ********************************************************************************/

#include "cp_atcmd.h"
#include "cp_string.h"
#include "cp_logger.h"
#include "cp_time.h"

#ifndef ATCMD_REPLY_LEN 
#define ATCMD_REPLY_LEN          512
#endif

/*
 *  Description: Send the AT command which modem will only reply "OK" or "ERROR",
 *               such as AT,ATE0,AT+CNMP=2 etc.
 *
 * Return Value:  0: OK     -X: ERROR
 */
int send_atcmd_check_ok(COM_PORT *comport, char *atcmd, unsigned long timeout)
{
    int  retval;
    retval = send_atcmd(comport, atcmd, "OK\r\n", "ERROR\r\n", timeout, NULL, 0);

    return 0==retval ? 0 : -2;
}

/*
 *  Description: Send the AT command which will reply the value directly in a  
 *               single line, such as AT+CGMM, AT+CGSN
 *
 * Output Value: Buf: The AT command query value
 * Return Value:  0: OK     -X: ERROR
 *
 */
int send_atcmd_check_value(COM_PORT *comport, char *atcmd, unsigned long timeout, char *buf, int buf_len)
{
    int         i = 0;
    int         retval=0; 
    char        tmp[ATCMD_REPLY_LEN];
    char        *start;

    if(NULL==buf)
    {
        log_err("Function call arguments error!\n");
        return -1;
    }

    memset(tmp, 0, sizeof(tmp));
    retval = send_atcmd(comport, atcmd, "OK\r\n", "ERROR\r\n", timeout, tmp, sizeof(tmp));
    if( 0 != retval)
    {
        return -2;  /* AT command can not get reply  */
    }

    /* Parser the receive string to get the expect value*/
    if(NULL != (start=strchr(tmp, '\n')) )  
    {
        start ++;  /* Skip '\n' character */
        while(*start!='\r' && i<buf_len-1)
        {
            buf[i++] = *start; 
            start ++;
        }
    }
    buf[i] = '\0'; /* End of the string */

    return ('\0'==buf[0] ? -3 : 0);
}

/*
 *  Description: Send the AT command which will reply the value with a prefix "+CMD: "  
 *               single line, such as AT+CGMR  AT+CPIN? AT+CNMP? AT+CSQ
 *
 * Output Value: Buf: The AT command query value by remove "+XXX:" prefix
 * Return Value:  0: OK     -X: ERROR
 *
 */
int send_atcmd_check_request(COM_PORT *comport, char *atcmd, unsigned long timeout, char *buf, int buf_len)
{
    int         retval;
    int         i = 0;
    char        tmp[ATCMD_REPLY_LEN];
    char        *ptr = NULL;

    if(NULL==buf)
    {
        log_err("%s() call arguments error!\n", __FUNCTION__);
        return -1;
    }

    memset(tmp, 0, sizeof(tmp));
    retval = send_atcmd(comport, atcmd, "OK\r\n", "ERROR\r\n", timeout, tmp, sizeof(tmp));
    if( 0 != retval)
    {
        return -2;
    }

    /* Parser the receive string to get the expect value*/
    if(NULL != (ptr=strchr (tmp, ':')) )   
    {
        ptr += 2; /* Skip the ':' and SPACE character */ 
        while(*ptr!='\r' && i<buf_len-1)
        {
            buf[i++] = *ptr; 
            ptr ++;
        } 
    }
    buf[i] = '\0'; /* End of the string */

    return ('\0'==buf[0] ? -3 : 0);
}


/*
 *  Description: Send ATE0 command to the GSM module to check GSM module ready or not
 * Return Value: 0: Success  !0:Failure
 */
int atcmd_check_at_ready(COM_PORT *comport)
{
    int    retval;
    retval=send_atcmd_check_ok(comport, "ATE0\r", 500);
    if(retval)
    {
        log_err("ATE0 check AT command ready             [FAILED]\n"); 
    }
    else
    log_nrml("ATE0 check AT command ready           [OK]\n"); 

    return retval;
}

/*
 *  Description: Send AT+CPIN? command to the GSM module to check SIM card exist or not
 * Return Value: 0: Success  !0:Failure
 */
int atcmd_check_sim_valid(COM_PORT *comport)
{
    int         retval = 0; 
    char        recv_buf[ATCMD_REPLY_LEN];

    retval = send_atcmd_check_request(comport, "AT+CPIN?\r", 800, recv_buf, sizeof(recv_buf));
    if(0x00 != retval)
    {
        retval = 1;
        log_warn("AT+CPIN? Check SIM Validation:          [FAILED]\n");
        return retval;
    }

    if(strstr(recv_buf, "READY"))
    {
        log_nrml("AT+CPIN? Check SIM Validataion:       [OK]\n");
        return 0;
    }
    else
        return -1;
}

/*  Send AT+CSQ command to check GPRS signal*/
int atcmd_check_gprs_signal(COM_PORT *comport)
{
    char        recv_buf[ATCMD_REPLY_LEN]; 
    int         retval;
    int         signal = -1;
    
    memset(recv_buf, 0, sizeof(recv_buf));
    retval = send_atcmd_check_request(comport, "AT+CSQ\r", 3000, recv_buf, sizeof(recv_buf));
    if( 0 !=  retval)
    {
        log_err("AT+CSQ Check Signal Strength:          [FAILED]\n");
        return retval;
    } 

    split_string_to_value(recv_buf, "%d,%d", &signal, NULL);

    log_nrml("AT+CSQ Check Signal Strength:         [%d]\n", signal);

    return signal;
}

/* Send AT+CREG? command to check SIM card register status */
int atcmd_check_gprs_register(COM_PORT *comport)
{
    int         retval, stat = -1;
    char        recv_buf[ATCMD_REPLY_LEN]; 

    memset(recv_buf, 0, sizeof(recv_buf));
    retval = send_atcmd_check_request(comport, "AT+CREG?\r", 3000, recv_buf, sizeof(recv_buf));
    if( 0 !=  retval)
    {
        log_err("AT+CREG? Check SIM card Register:      [FAILED]\n");
        return stat;
    } 
    
    split_string_to_value(recv_buf, "%d,%d", NULL, &stat);
    log_nrml("AT+CREG? Check SIM Card Register:     [%d]\n", stat);

    return stat;
}


int atcmd_check_gprs_carrier(COM_PORT *comport, char *carrier)
{
    int               retval;
    char              recv_buf[ATCMD_REPLY_LEN];
 
    if(carrier == NULL)
    {
        return -1;
    }

    retval = send_atcmd_check_request(comport, "AT+COPS?\r", 5000, recv_buf, sizeof(recv_buf));
    if(retval) 
    {
        log_warn("AT+COPS? Check SIM Card Carrier:        [FAILED]\n");
        return retval;
    }

    split_string_to_value(recv_buf, "%d,%d,%s,%d", NULL, NULL, carrier, NULL);
    del_char_from_string(carrier, '\"');

    log_nrml("AT+COPS? Check SIM Card Carrier:      [%s]\n", carrier);

    return 0;
}

int atcmd_check_gprs_mcc_mnc(COM_PORT *comport, char *mcc_mnc)
{
    int               retval;
    char              recv_buf[ATCMD_REPLY_LEN];
    int               status;
    char              mcc[5];
    char              mnc[5];
    char              *ptr = NULL;

    retval=send_atcmd_check_ok(comport, "AT+QENG=1\r", 1000);
    if(retval) 
    {
        log_err("Send AT command AT+QENG=1 failure\n");
        return retval;
    }

    retval = send_atcmd(comport, "AT+QENG?\r", "OK\r\n", "ERROR\r\n", 5000, recv_buf, sizeof(recv_buf));
    if(retval) 
    {
        log_err("AT+QENG? Check GPRS MCC and MNC failure\n");
        return retval;
    }

    /* AT+QENG? respond value:
     * +QENG: 1,0
     * +QENG: 0,460,00,7108,80c4,554,41,-95,18,50,0,10,x,x,x,x,x,x,x  
     */
    if(NULL != (ptr=strrchr (recv_buf, ':')) )   
    {
        split_string_to_value(ptr, "%d,%s,%s,%s", &status, mcc, mnc, NULL);
        if(!status)
        {
            sprintf(mcc_mnc, "%s-%s", mcc, mnc);
            log_nrml("AT+QGSMLOC=1 Check GPRS Location MCC-MNC: %s\n", mcc_mnc);
            return 0;
        }
    }

    printf("ptr: %s\n", ptr);

    return -1;
}


int atcmd_check_gprs_location(COM_PORT *comport, GPRS_LOCATION *loc)
{
    int               retval;
    char              recv_buf[ATCMD_REPLY_LEN];
    int               status;
    
    retval=send_atcmd_check_ok(comport, "AT+QIFGCNT=0\r", 1000);
    if(retval) 
    {
        log_warn("Send AT command AT+QIFGCNT=0 failure\n");
        return retval;
    }

    retval = send_atcmd_check_request(comport, "AT+QGSMLOC=1\r", 10000, recv_buf, sizeof(recv_buf));
    if(retval) 
    {
        log_warn("AT+QGSMLOC=1 Check GPRS Location:         [FAILED]\n");
        return retval;
    }

    /* +QGSMLOC: 0,114.389210,30.500380,2013/01/16,14:03:12, it's GMT time  */
    split_string_to_value(recv_buf, "%d,%s,%s,%s,%s", &status, loc->longitude, loc->latitude, loc->date, loc->time);

    if(status) /* Get LOC failure */
    {
        memset(loc->longitude, 0, sizeof(loc->longitude));
        memset(loc->latitude, 0, sizeof(loc->latitude));
        memset(loc->date, 0, sizeof(loc->date));
        memset(loc->time, 0, sizeof(loc->time));
        log_warn("AT+QGSMLOC=1 Check GPRS Location failure: %s\n", recv_buf);
        return -1;
    }

    log_nrml("GPRS location result=%d latitude,longitude: [%s,%s]\n", status, loc->latitude, loc->longitude);
    log_nrml("GPRS Date and time: %s,%s\n", loc->date, loc->time);

    return retval;
}


int atcmd_set_network_mode(COM_PORT *comport, int mode)
{
    int              retval;
    char             atcmd[64]={0}; 

    sprintf (atcmd, "AT+CNMP=%d\r", mode); 
    if(0 != (retval=send_atcmd_check_ok(comport, atcmd, 3000)) )
    {
        log_warn("AT+CNMP Set Network Mode as %d:      [FAILED]\n", mode);
        return retval;
    } 
    log_nrml("AT+CNMP=%d Set Network Mode:          [OK]\n", mode);

    /* AT+CNAOP=?: 0->Automatic, 1->GSM,WCDMA, 2->WCDMA,GSM */
    strncpy (atcmd, "AT+CNAOP=2\r", sizeof(atcmd)); 
    if(0 != (retval=send_atcmd_check_ok(comport, atcmd, 3000)) )
    {
        log_warn("AT+CNAOP=2 Set Acquisitions order preference to WCDMA,GSM     [FAILED]\n");
        return retval;
    } 
    log_nrml("AT+CNAOP=2 Set Network Preference     [OK]\n");

    return 0;
}

int atcmd_check_gprs_name(COM_PORT *comport, char *name)
{
    int               retval;
    char              recv_buf[ATCMD_REPLY_LEN];
 
    if(name == NULL)
    {
        return -1;
    }

    retval = send_atcmd_check_value(comport, "AT+CGMM\r", 5000, recv_buf, sizeof(recv_buf));
    if(retval) 
    {
        log_warn("AT+CGMM Check GPRS Module Name:         [FAILED]\n");
        return retval;
    }

    strcpy(name, recv_buf);

    log_nrml("AT+CGMM Check GPRS Module Name:       [%s]\n", name);

    return 0;
}


int atcmd_check_gprs_version(COM_PORT *comport, char *version)
{
    int               retval;
    char              recv_buf[ATCMD_REPLY_LEN];
 
    if(version == NULL)
    {
        return -1;
    }

    retval = send_atcmd_check_request(comport, "AT+CGMR\r", 5000, recv_buf, sizeof(recv_buf));
    if(retval) 
    {
        log_warn("AT+CGMR Check GPRS Module Version:      [FAILED]\n");
        return retval;
    }

    strcpy(version, recv_buf);
    log_nrml("AT+CGMR Check GPRS Module Version:    [%s]\n", version);

    return 0;
}

int atcmd_check_gprs_iemi(COM_PORT *comport, char *iemi)
{
    int               retval;
    char              recv_buf[ATCMD_REPLY_LEN];
 
    if(iemi == NULL)
    {
        return -1;
    }

    retval = send_atcmd_check_value(comport, "AT+CGSN\r", 5000, recv_buf, sizeof(recv_buf));
    if(retval) 
    {
        log_warn("AT+CGSN Check GPRS Module IEMI:      [FAILED]\n");
        return retval;
    }

    strcpy(iemi, recv_buf);
    log_nrml("AT+CGSN Check GPRS Module IEMI:       [%s]\n", iemi);

    return 0;
}


int atcmd_check_gprs_network(COM_PORT *comport, int *network)
{
    int               retval;
    char              recv_buf[ATCMD_REPLY_LEN];
 
    if(network == NULL)
    {
        return -1;
    }

    retval = send_atcmd_check_request(comport, "AT+CNSMOD?\r", 5000, recv_buf, sizeof(recv_buf));
    if(retval) 
    {
        log_warn("AT+CNSMOD Check Network Mode:        [FAILED]\n");
        return retval;
    }

    split_string_to_value(recv_buf, "%d,%d", NULL, network);

    log_nrml("AT+CNSMOD? Check Network Mode:        [%d]\n", *network);
    return 0;
}


int atcmd_set_apn(COM_PORT *comport, char *apn)
{
    char             atcmd[64]={0};
    int              retval; 
    
    sprintf (atcmd, "AT+CGDCONT=1,\"IP\",\"%s\"\r", apn);
    if(0 != (retval=send_atcmd_check_ok(comport, atcmd, 2000)) )
    {
        log_err("AT+CGDCONT Set APN as \"%s\"         [FAILED]\n", apn); 
        return retval;
    }

    log_nrml("AT+CGDCONT Set APN as \"%s\"          [OK]\n");
    return retval;
}


unsigned char at_match (char *p_pcStr, char *p_pcMatch)
{
    char                    acBuf [256],
                            *pcStart = NULL,
                            *pcTab   = NULL;

    pcStart = p_pcMatch; 
    
    while (0 != pcStart)
    {
        memset (acBuf, 0x00, sizeof (acBuf)); 

        pcTab = strchr (pcStart, 9);    // Find for TAB 
        if (0 != pcTab)
        {
            if (pcTab != pcStart)
            {
                strncpy (acBuf, pcStart, pcTab - pcStart);
            } 
            pcStart = (0 != *(++pcTab)) ? pcTab : 0;
        }
        else
        {
            strcpy (acBuf, pcStart); 
            pcStart = NULL;
        } 
        if (0 != acBuf [0] && 0 != strstr (p_pcStr, acBuf))
        {
            return 0x00;
        }
    } 
    return 0x01;
}

 
/*=========================================================================================================
 * Parameter Description:  
 *     COM_PORT    *comport:  The GPRS module data port(/dev/ttyS2);  
 *     char          *atCmd:  The AT command which will be sent to GPRS module
 *     char         *expect:  The EXPECT reply string by GPRS module for the AT command, such as "OK"
 *     char          *error:  The ERROR  reply string by GPRS module for the AT command, such as "ERROR"
 *   unsigned long  timeout:  Read from data port timeout value
 *     char           reply:  The AT command reply output buffer
 *     int        reply_len:  The AT command reply output buffer length
 *
 * Return Value:
 *     int           retval:   0->command send OK and "expect" string mached. !0->failure
 *     char        *content:   The AT command reply string by modem.
 *
 *=========================================================================================================*/

int send_atcmd(COM_PORT *comport, char *atCmd, char *expect, char *error,
              unsigned long timeout, char *reply, int reply_len)
{
    int                  retval = -1; 
    unsigned long        delay = 200;
    unsigned long        gap = 300;
    unsigned long        ulStartTime; 
    
    int                  iCmdLen = 0,
                         iRecvLen = 0,
                         iRecvSize = 0,
                         iSize = 0;

    
    char                 acRecv[1024];
    char                 *pcRecvPtr = NULL; 
    
    if(comport->is_connted != 0x01) /* Comport not opened */
    {
        log_dbg("Comport not opened.\n");
        return  -1;
    }

#if 0
    /*=========================================
     *=  Pause a while before send AT command =
     *=========================================*/
    if(0 != delay)
    {
        ulStartTime = time_now();
        while (time_elapsed(ulStartTime) < delay)
        {
            micro_second_sleep(1);
        }
    }
#endif

    /*====================
     *=  Throw Rubbish   =
     *====================*/
    ulStartTime = time_now(); 
    memset(&acRecv, 0, sizeof(acRecv));

    while (time_elapsed(ulStartTime) < delay)
    {
        iSize = comport_recv(comport, acRecv, 1, 50);
        if(iSize <= 0)
        {
            break;
        } 
        micro_second_sleep(1);
    }

    /*====================
     *=  Send AT command =
     *====================*/

    iCmdLen = strlen(atCmd);
    retval = comport_send (comport, atCmd, iCmdLen);
    if (0 != retval)
    {
        retval = 0x02;
        goto  CleanUp;
    }

    /*===================================================
     *=  Pause a while before read command response. 
     *===================================================*/
    if(0 != gap)
    {
        ulStartTime = time_now();
        while (time_elapsed(ulStartTime) < gap)
        {
            micro_second_sleep(1);
        }
    }
    
    memset (acRecv, 0, sizeof (acRecv));
    pcRecvPtr = acRecv;
    iRecvLen = 0;
    iRecvSize = sizeof (acRecv);

    retval = -1;
    ulStartTime = time_now();

    while (time_elapsed(ulStartTime) < timeout)
    {
        if ( iRecvLen < (iRecvSize-1) )
        {
            iSize = comport_recv (comport, pcRecvPtr, 1, 50); 
            if (iSize >0)
            {
                iRecvLen += iSize;
                pcRecvPtr += iSize;
                acRecv [iRecvSize-1] = 0;

                /*========================================
                 * Match the received with expect String =
                 *========================================*/
                if(NULL != expect)
                {
                    if (0x00 == at_match(acRecv, expect))
                    {
                        retval = 0;
                        goto CleanUp;
                    }
                } 
                
                /*======================================== 
                 * Match the received with error String  = 
                 *========================================*/
                if(NULL != error)
                {
                    if (0x00 == at_match(acRecv, error))
                    { 
                        retval = -3;
                        goto CleanUp;
                    }
                } 
            } /*End of (iSize > 0)  */ 
        } /* End of (iRecvLen < (iRecvSize-1))  */ 
        
        micro_second_sleep(1);
    } /* End of time_elapsed(ulStartTime) < timeout */


    if(NULL==expect)
        retval = 0x00;
    else
        retval = -4; 

CleanUp:
    //printf("acRecv:\n %s\n", acRecv);
    if( NULL != reply)
    {
        strncpy(reply, acRecv, reply_len);
    }

#if 1  /* Log the command result to log system  */
    {
        char   log[512] = {0};
        snprintf(log, 512, "Send AT command: \"%s\" get reply \"%s\"", atCmd, acRecv);
        int i = 0;
        for (i=0; i<512; i++)
        {
            if('\r'==log[i] || '\n'==log[i] || '\t'==log[i]) 
            {
                log[i]=' '; 
            }
            else if (0 == log[i]) 
            {
                break;
            } 
        }
        log_info("%s\n", log);
    }
#endif 
    
    return retval;
}
