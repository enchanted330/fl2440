/******************************************************************************** 
 *      Copyright:  (C) guowenxue <guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  ppp.c
 *    Description:  This file is the ppp thread file,used to do PPP dial up
 *
 *        Version:  1.0.0(02/02/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "02/02/2012 11:33:33 AM"
 *                 
 ********************************************************************************/
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include "cp_ppp.h"
#include "cp_string.h"
#include "cp_iniparser.h"
#include "cp_network.h"
//#include "gsmd.h"

/*
 * Description: Check PPP10 device exist or not, and set the PPP context status
 *
 *
 * Return Value: 0: PPP10 device exist    -1:PPP10 device not exist
 *
 */
int check_ppp_interface(char *ppp_dev)
{
    int           fd = -1;
    int           retval = -1;
    char          buf[512];

    if ((fd = open("/proc/net/dev", O_RDONLY, 0666)) < 0)
    {
        return -1;      /* System error, take as PPP10 device not exist too */
    }

    while (read(fd, buf, sizeof(buf)) > 0)
    {
        if (NULL != strstr(buf, ppp_dev))
        {
            retval = 0;
            break;
        }
    }

    close(fd);
    return retval;
}

int check_ppp_ipaddr(PPPD_INFO  *pppd_info, char *ppp_dev)
{
    int                   fd;
    struct ifreq          ifr;
    struct sockaddr_in    *sin;  

    if(check_ppp_interface(ppp_dev))
        return -2;

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    strcpy(ifr.ifr_name, ppp_dev);

    if ((ioctl(fd, SIOCGIFADDR, (caddr_t) & ifr, sizeof(struct ifreq))) < 0)
    {
        log_fatal("System call ioctl() with SIOCGIFADDR failure.\n", ppp_dev);
        return -1;
    }

    sin = (struct sockaddr_in *)&ifr.ifr_addr;
    strcpy(pppd_info->netaddr, (const char *)inet_ntoa(sin->sin_addr));
    log_nrml("Get %s local IP address: %s\n", ppp_dev, pppd_info->netaddr);

    if ((ioctl(fd, SIOCGIFDSTADDR, (caddr_t) & ifr, sizeof(struct ifreq))) < 0)
    {
        log_fatal("System call ioctl() with SIOCGIFDSTADDR failure.\n", ppp_dev);
        return -1;
    }
    sin = (struct sockaddr_in *)&ifr.ifr_dstaddr;
    strcpy(pppd_info->ptpaddr, (const char *)inet_ntoa(sin->sin_addr));
    log_nrml("Get %s remote IP address: %s\n", ppp_dev, pppd_info->ptpaddr);

    return 0;
}

int check_ppp_stat(PPP_STAT  *ppp_stat, char *ppp_dev)
{
    int                 retval;
    FILE                *fp;
    char                *ptr;
    char                buf[512];

    if( NULL == (fp=fopen(PPP_PROC_NET_DEV, "r")) ) 
    {
        log_err("Can not open %s.\n", PPP_PROC_NET_DEV);
        return -1;
    }

    fgets(buf, sizeof(buf), fp);
    fgets(buf, sizeof(buf), fp);

    while (fgets(buf, sizeof(buf), fp))
    {
        ptr=strstr(buf, ppp_dev);
        if(NULL == ptr)
        {
            log_err("Can not find %s interface statistic data in %s\n", ppp_dev, PPP_PROC_NET_DEV);
            retval = -1;
            break;
        }
        else
        {
             ptr = strchr(ptr, ':');
             ptr++;
             sscanf(ptr, "%llu%llu%lu%lu%lu%lu%lu%lu%llu%llu%lu%lu%lu%lu%lu%lu",
                     &(ppp_stat->ullRx_Bytes),
                     &(ppp_stat->ullRx_Packets),
                     &(ppp_stat->ulRx_Errors),
                     &(ppp_stat->ulRx_Dropped),
                     &(ppp_stat->ulRx_FIFO_Errors),
                     &(ppp_stat->ulRx_Frame_Errors),
                     &(ppp_stat->ulRx_Compressed),
                     &(ppp_stat->ulRx_Multicast),
                     &(ppp_stat->ullTx_Bytes),
                     &(ppp_stat->ullTx_Packets),
                     &(ppp_stat->ulTx_Errors),
                     &(ppp_stat->ulTx_Dropped),
                     &(ppp_stat->ulTx_FIFO_Errors),
                     &(ppp_stat->ulCollisions),
                     &(ppp_stat->ulTx_Carrier_Errors),
                     &(ppp_stat->ulTx_Compressed)
                   );
             retval = 0;
             break;
        }
    }

    if(0 == retval )
        log_dbg("Check PPP network traffic: Tx %llu bytes, Rx %llu bytes.\n", ppp_stat->ullTx_Bytes, ppp_stat->ullRx_Bytes);
    fclose(fp);
    return retval;
}

int kill_pppd_process(COM_PORT *comport)
{
    int           pid = -1;

    while( (pid=check_pppd_pid(comport->dev_name)) > 0) 
    {
        log_nrml("Terminate pppd process [pid=%d]\n", pid);
        kill(pid, SIGTERM); 
        sleep(1); /* Wait pppd process exit */
    }

    return 0;
}

int stop_ppp_connect(PPP_CTX *ppp_ctx)
{
    log_warn("Stop PPP connection now.\n");

    if(check_ppp_interface(ppp_ctx->dev))
        return 0;

    kill_pppd_process(ppp_ctx->comport);

    if(ppp_ctx->comport)
    {
        free_gsm_dataport("stop_ppp_connect()", ppp_ctx->module, &(ppp_ctx->comport));
    }
    return 0;
}

int get_apn_conf(char *ini_name, char *ini_key, APN_ACCOUNT *apn, unsigned char type)
{
    dictionary       *ini ;
    char             tmp[64];
    char             *str;
            
    if(NULL==ini_name || NULL==ini_key || NULL==apn)
    {       
        return -1;
    }       
        
    ini = iniparser_load(ini_name);
    if (ini==NULL)
    {   
        log_err("cannot load default APN configure file: %s\n", ini_name);
        return -2 ;
    }       
    log_dbg("Parser default APN configure file %s\n", ini_name);
            
    /* Parser APN configure */
    snprintf(tmp, 64, "%s:%s", ini_key, APN_3G==type?"3g_apn":"apn");
    if( !(str=iniparser_getstring(ini, tmp, NULL)) )
    {       
        log_err("cannot find APN setting for SIM [%s] in file: %s\n", ini_key, ini_name);
        memset(apn, 0, sizeof(APN_ACCOUNT));
        return -1;
    }           
    else        
    {       
        strncpy(apn->apn, str, APN_LEN);
    }
    
    /* Paser APN username information  */
    snprintf(tmp, 64, "%s:%s", ini_key, APN_3G==type?"3g_uid":"uid");
    str = iniparser_getstring(ini, tmp, NULL);
    strncpy(apn->uid, str, UID_LEN);


    /* Paser APN password information  */
    snprintf(tmp, 64, "%s:%s", ini_key, APN_3G==type?"3g_pwd":"pwd");
    str = iniparser_getstring(ini, tmp, NULL);
    strncpy(apn->pwd, str, PWD_LEN);

    iniparser_freedict(ini);
    return 0;
}


int select_apn(PPP_CTX *ppp_ctx, char *key)
{
    int                 retval;
    unsigned char       type = APN_2G;

    log_nrml("GPRS module work on GPRS mode, select APN now.\n"); 

    retval = get_apn_conf(APN_DEF_CONF_FILE, key, &(ppp_ctx->apn), type);
    if(!retval)
    {
        log_err("Get default APN [%s] from %s ok\n", ppp_ctx->apn.apn, APN_DEF_CONF_FILE);
        return 0;
    }

    return -1;
}

#define PPPD_CMD_LEN  512
int start_ppp_dial_cmd(COM_PORT  *comport, APN_ACCOUNT *apn, char *ppp_dev)
{
    char                tmp[64];
    char                pppd_cmd[PPPD_CMD_LEN];
    pid_t               pid;

    if( !comport || !comport->is_connted )
    {
        log_err("GPRS module data port %s is not opened.\n", comport->dev_name);
        return -1;
    }

    if( !apn || strlen(apn->apn)<=0 )
    {
        log_err("No valid APN was set.\n");
        return -1;
    }

    log_nrml("Start PPP connection with APN \"%s\" now.\n", apn->apn);

    memset(pppd_cmd, 0, PPPD_CMD_LEN);
    snprintf(pppd_cmd, PPPD_CMD_LEN, "%s -d %s ", PPPD_DIAL_SCRIPTS, comport->dev_name);

    snprintf(tmp, sizeof(tmp), " -a %s ", apn->apn);
    strncat(pppd_cmd, tmp, PPPD_CMD_LEN);

    if( strlen(apn->uid) > 0 )
    {
        snprintf(tmp, sizeof(tmp), " -u %s ", apn->uid);
        strncat(pppd_cmd, tmp, PPPD_CMD_LEN);
    }
    
    if( strlen(apn->pwd) > 0 )
    {
        snprintf(tmp, sizeof(tmp), " -p %s ", apn->pwd); 
        strncat(pppd_cmd, tmp, PPPD_CMD_LEN);
    }

    if( strlen(apn->auth) > 0 )
    {
        snprintf(tmp, sizeof(tmp), " -v %s ", apn->auth);
        strncat(pppd_cmd, tmp, PPPD_CMD_LEN);
    }

    strncat(pppd_cmd, ppp_dev, PPPD_CMD_LEN);

    log_nrml("ppp dial up command: %s\n", pppd_cmd);

    pid = fork();
    if( 0 == pid ) /* Child process */
    {
        system(pppd_cmd); 
        exit(0);
    }
    else if (pid < 0)
    {
        log_err("Create new process failure.\n"); 
        return -1;
    }
    else
    {
        log_dbg("pppd program already running.\n");
        micro_second_sleep(1000);  
    }

    return 0;
}

int check_pppd_pid(char *find_key)
{
    char       buf[10];
    char       cmd[128];
    FILE       *fp = NULL;
    int        pid = -1;

    snprintf(cmd, sizeof(cmd), "ps | grep -v grep | grep pppd | grep %s | awk '{print $1}'", find_key); 

    fp = popen(cmd, "r");
    if(NULL == fp)
    {
        log_fatal("popen() to check the pppd process ID failure.\n");
        return -1;
    }

    memset(buf, 0, sizeof(buf));
    fgets(buf, sizeof(buf), fp);

    pid = atoi(buf); 

    if(pid <= 1)
    {
        log_err("Get pppd program running pid failure\n");
        pid = -1;
    }
#if 0
    else
    {
        log_trace("pppd program running pid is [%d]\n", pid);
    }
#endif

    pclose(fp);
    return pid;
}

void set_ppp_disconnect(PPP_CTX *ppp_ctx)
{
    ppp_ctx->status = DISCONNECT; 
    ppp_ctx->pppd_info.start_time = 0; 
    
    memset(&(ppp_ctx->pppd_info), 0, sizeof(PPPD_INFO)); 
    memset(&(ppp_ctx->ppp_stat), 0, sizeof(PPP_STAT)); 
    ppp_ctx->network = PPP_STOP;
}


int ppp_context_init(PPP_CTX *ppp_ctx, MODULE_INFO *module)
{
    memset(ppp_ctx, 0, sizeof(*ppp_ctx));

    ppp_ctx->thread_id = pthread_self();
    ppp_ctx->module = module;
    strncpy(ppp_ctx->dev, PPP_INTERFACE_NAME, sizeof(ppp_ctx->dev));
    strncpy(ppp_ctx->ping_ip, DEF_PING_DST, sizeof(ppp_ctx->ping_ip));
    ppp_ctx->ping_interval  = PING_INTERVAL_TIME;
    set_ppp_disconnect(ppp_ctx);

    log_nrml("Initialize PPP thread context ok\n"); 
    return 0;
}

void ppp_context_term(PPP_CTX *ppp_ctx)
{
    stop_ppp_connect(ppp_ctx);
    set_ppp_disconnect(ppp_ctx);

    log_nrml("Terminate PPP thread context ok\n"); 
}

int start_ppp_connect(PPP_CTX *ppp_ctx)
{
    int                 retval;
    MODULE_INFO         *module = ppp_ctx->module; 

    if( !ppp_ctx->comport ) 
    {
        ppp_ctx->comport=alloc_gsm_dataport("start_ppp_connect()", ppp_ctx->module);
        if(!ppp_ctx->comport)
            return -1;

        log_dbg("PPP thread alloc GPRS dataport %s=>[fd:%d]\n", ppp_ctx->comport->dev_name, ppp_ctx->comport->fd);
    }

    
    switch(ppp_ctx->status)
    { 
        case DISCONNECT: /*  Select the APN now */
            set_ppp_disconnect(ppp_ctx);
            retval = select_apn(ppp_ctx, module->reg.loc.mcc_mnc);
            if(!retval)
            {
                log_nrml("Select APN successfully, go to next stage.\n");
                ppp_ctx->fail_cnt = 0; 
                ppp_ctx->status++;
            }
            else
            {
                log_warn("Select APN failure, request to reset GPRS module\n");
                ppp_ctx->fail_cnt ++; 
                set_module_event(module, REQ_POWER_RESET);
                break;
            }

        case SELECTED_APN: /*  Select the APN now */
            log_warn("Run pppd program to start PPP connection now\n"); 
            ppp_ctx->network = PPP_CONN;
            start_ppp_dial_cmd(ppp_ctx->comport, &(ppp_ctx->apn), ppp_ctx->dev);
            ppp_ctx->status++; 
            break;
    } 
    
    return 0;
}

int inspect_network_status(PPP_CTX *ppp_ctx)
{
    PPPD_INFO              *pppd_info = &(ppp_ctx->pppd_info);
    PPP_STAT               *ppp_stat = &(ppp_ctx->ppp_stat);
    int                    lost_percent;
    unsigned long long     last_tx_bytes= ppp_stat->ullTx_Bytes;
    unsigned long long     last_rx_bytes= ppp_stat->ullRx_Bytes;

    if( CONNECTED != ppp_ctx->status)
    {
        ppp_ctx->pppd_info.ping_time = 0;
        memset(ppp_stat, 0, sizeof(PPP_STAT));

        pppd_info->pid = check_pppd_pid(ppp_ctx->comport->dev_name);
        if(pppd_info->pid <= 0)
        {
            /*  pppd can not connect in 30s, then set it to DISCONNECT status */
            if(time_elapsed(pppd_info->start_time) >= PPPD_DIAL_TIMEOUT)
            {
                /* Sleep for 2 seconds, make sure the exit pppd process release the TTY device */
                micro_second_sleep(2000);
                log_err("pppd process exit when do ppp dialing, set status to DISCONNECT.\n");
                set_ppp_disconnect(ppp_ctx);
            }
        }
        else
        {
            if(strlen(pppd_info->netaddr) <=0 )
            {
                check_ppp_ipaddr(pppd_info, ppp_ctx->dev);
                log_dbg("pppd running process ID [%d].\n", pppd_info->pid);
            }
        }
    }

    if(  time_elapsed(pppd_info->ping_time) >= ppp_ctx->ping_interval*1000  )
    {
        if(ppp_ctx->fail_cnt >= MAX_PPP_FAIL_CNT)
        {
            /* Sleep for 2 seconds, make sure the exit pppd process release the TTY device */
            micro_second_sleep(2000);
            log_warn("PPP network inspect status failure, set status to DISCONNECT.\n");
            set_ppp_disconnect(ppp_ctx);
            return 0;
        }

        lost_percent = network_ping_test(ppp_ctx->dev, ppp_ctx->ping_ip);
        ppp_ctx->network = lost_percent<=60 ? PPP_GOOD : PPP_BAD;

        if(lost_percent <= 80)
        {
            check_ppp_stat(ppp_stat, ppp_ctx->dev);
            if(last_tx_bytes==ppp_stat->ullTx_Bytes || last_rx_bytes==ppp_stat->ullRx_Bytes)
            {
                log_warn("PPP interface traffic not increased, maybe ppp disconnected?\n");
                ppp_ctx->fail_cnt++;
            }
            else
            {
                log_nrml("Set PPP connection status to CONNECTED.\n");
                set_ppp_disconnect(ppp_ctx);
                ppp_ctx->fail_cnt=0;
            }
        }
        else
        {
            ppp_ctx->fail_cnt++;
        }
        ppp_ctx->pppd_info.ping_time = time_now();
    }

    return 0;
}


void *ppp_thread_workbody(void *thread_arg)
{
    PPP_CTX             ppp_ctx; 

    MODULE_INFO         *module;

    module = (MODULE_INFO *)thread_arg;
    ppp_context_init(&ppp_ctx, module);

    log_nrml("GPRS PPP thread start running\n");

    while( !g_cp_signal.stop )
    {
        if(ALL_READY != module->ready)
        {
            if(ppp_ctx.status>=CONNECTING)
            {
                log_nrml("Stop exist PPP connection and free TTY data port\n");
                stop_ppp_connect(&ppp_ctx);
            }

            /*  GPRS not ready, sleep for 200ms and check again */
            micro_second_sleep(200);
            continue;
        }
        else
        {
            if(ppp_ctx.status>=CONNECTING)
            {
                inspect_network_status(&ppp_ctx);
            }
            else
            {
                start_ppp_connect(&ppp_ctx);
            }
        }

        sleep(1);
    }

    ppp_context_term(&ppp_ctx);
    pthread_exit(0);
    return NULL;
}

int start_thread_ppp(MODULE_INFO *module)
{
    pthread_t           tid;

    return thread_start(&tid, ppp_thread_workbody, module);
}
