/*********************************************************************************
 *      Copyright:  (C) guowenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  module.c
 *    Description:  This is the GPRS Power/SIM card control source code
 *                 
 *        Version:  1.0.0(02/07/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "02/07/2012 03:08:25 PM"
 *                 
 ********************************************************************************/

#include "cp_gprs.h"
#include "cp_hal.h"
#include "cp_proc.h"
#include "cp_gsmmux.h"
#include "at91_ioctl.h"

int init_gsm_module(MODULE_INFO *module)
{
    int                   i=0;
    char                  devname[DEVNAME_LEN];

    log_info("Initialize GSM module context\n");

    memset(module, 0, sizeof(*module));

    module->event = REQ_POWER_RESET;  /* Request to power on GPRS */

    pthread_mutex_init(&module->lock, NULL); 

    if( !(module->gsmport=comport_init(GSM_DATAPORT, 115200, "8N1N")) )
    {
        return -1;
    }

    /*  Initialize the GPRS dataport pointer */
    module->comport_cnt = MAX_DATAPORT;
    for(i=0; i<module->comport_cnt; i++)
    {
        snprintf(devname, DEVNAME_LEN, "%s%d", CMUX_DATAPORT, i+CMUX_PORT_START_INDEX);
        module->comport[i] = comport_init(devname, 115200, "8N1N");
        if(NULL == module->comport[i])
            goto ERROR;

        //log_dbg("Initialise comport [%p] on %s on baudrate 115200 with 8N1N\n", module->comport[i], devname);
    } 
    
    return 0;

ERROR:
    while(i-- >= 0)
    {
        comport_term(module->comport[i]);
    }
    return -2;
}


/*
 *  Description: close all the GPRS control port device. 
 *
 * Return Value: NONE
 */
void close_gsm_dataport(MODULE_INFO *module)
{
    int  i;
    
    for(i=0; i<module->comport_cnt; i++)
    {
        log_nrml("Close GPRS dataport %s=>[fd:%d]\n", module->comport[i]->dev_name, module->comport[i]->fd);
        comport_close(module->comport[i]);
    }

    return ;
}

/*
 *  Description: open all the GPRS data port device. 
 *
 * Return Value: 0: Open successfully  !0: Open failure
 */
int open_gsm_dataport(MODULE_INFO *module)
{
    int i, retval = -1;

    for(i=0; i<module->comport_cnt; i++)
    {
        if( (retval=comport_open(module->comport[i])) < 0)
        {
            log_err("Open GPRS dataport %s failure: %d\n", module->comport[i]->dev_name, retval);
            goto ERROR;
        } 
        log_nrml("Open GPRS dataport[%p] %s=>[fd:%d]\n", module->comport, module->comport[i]->dev_name, module->comport[i]->fd);
    }

    return 0;

ERROR:
    close_gsm_dataport(module);
    return retval;
}


/*
 *  Description: Alloc a comport from the comport poor for used, for we must make sure 
 *               PPP thread can grasp a comport to work, so we will always preserved
 *               first comport for PPP thread.
 *              
 * Return Value: NULL: No comport left  !NULL: The alloced comport pointer
 */
COM_PORT *alloc_gsm_dataport(char *who, MODULE_INFO *module)
{
    static int     lock = 0;
    int            i;
    COM_PORT       *comport = NULL;

    if(lock)
        return NULL;

    lock = 1;

    /* Preserve first comport for PPP thread */
    for(i=0; i<module->comport_cnt; i++)
    {
        if(module->comport[i]->used != YES)
        {
            comport = module->comport[i];
            comport->used = YES;
            break;
        }
    }

    lock = 0;

    if(comport)
    {
        log_dbg("%s allocate GPRS dataport[%d] %s=>[fd:%d] ok\n", who, i, comport->dev_name, comport->fd);
        module->users++;
    }
    else
    {
        log_dbg("%s allocate GPRS dataport failure\n", who);
    }


    return comport;
}

/*
 *  Description: Free a comport to the comport poor when we no need it.
 *
 * Return Value: NONE
 */
void free_gsm_dataport(char *who, MODULE_INFO *module, COM_PORT  **comport)
{
    int            i;

    /* Preserve first comport for PPP thread */
    for(i=0; i<module->comport_cnt; i++)
    {
        if(module->comport[i] == *comport)
        {
            log_dbg("%s free GPRS dataport %s=>[fd:%d]\n", who, (*comport)->dev_name, (*comport)->fd);
            module->comport[i]->used = NO;
            module->users--;
            *comport = NULL;
            break;
        }
    }

    return ;
}


/*
 *  Description: Power on the GPRS module and open all the gprs data port. For when GPRS power off,
 *               the GPRS module USB convert serial comport device lost, so we must bind them together.
 * Return Value: 0: Open successfully  !0: Open failure
 */
int power_on_module(MODULE_INFO *module)
{
    int     rv = 0;
    log_dbg("Turn on GPRS module now\n");

    if( ON == (module->pwr_status=hal_get_gprs_power()) )
    {
        rv = 0;
        log_dbg("GPRS module already power on\n");
        goto cleanup;
    }

    if( (rv=hal_turn_gprs_power(ON)) || ON!=hal_get_gprs_power())
    {
        log_err("Turn on GPRS module failure: rv=%d power=%d\n", rv, hal_get_gprs_power());
        rv = -1;
        goto cleanup;
    }

    micro_second_sleep(500);

    if( comport_open(module->gsmport) < 0)
    {
        log_err("Open GSM serial port [%s] failure\n", module->gsmport->dev_name);
        return -2;
    }

    if( !atcmd_check_power_on(module->gsmport) ) 
    {
        log_err("Send ATE0 check GPRS module AT command failure\n", module->gsmport->dev_name);
        return -2;
    }

    attach_gsm0710(module->gsmport);

    if( 0 != open_gsm_dataport(module) )
    {
        rv = -3;
        log_err("Open GPRS module comport failure\n");
        goto cleanup;
    }

cleanup:

    if(!rv)
    {
        module->ready = MODULE_READY;
        log_nrml("Turn on GPRS module succesfully\n");
    }
    else
    {
        module->ready = NOT_READY;
        log_nrml("Turn on GPRS module failed, rv=%d\n", rv);
    }

    module->ready = 0!=rv ? NOT_READY : MODULE_READY;
    memset(&(module->reg), 0, sizeof(REGISTER_INFO));
    module->pwr_status = hal_get_gprs_power();

    return rv;
}

/*
 *  Description: Power off the GPRS module and close all the gprs data port. For when GPRS power off,
 *               the GPRS module USB convert serial comport device lost, so we must bind them together.
 * Return Value: 0: Open successfully  !0: Open failure
 */
int power_off_module(MODULE_INFO *module)
{
    if( OFF  == (module->pwr_status=hal_get_gprs_power()) )
    {
        log_dbg("GPRS module already power off\n");
        return 0;
    }

    while(module->users)
    {
        log_warn("%d application still use the GPRS module dataport, wait released...\n", module->users);
        sleep(1);
    }

    log_nrml("Turn off GPRS module now\n");
    close_gsm_dataport(module);

    comport_close(module->gsmport);
    hal_turn_gprs_power(OFF);

    module->ready = NOT_READY;
    memset(&(module->reg), 0, sizeof(REGISTER_INFO));
    module->pwr_status = hal_get_gprs_power();

    micro_second_sleep(800);
    return 0;
}

void set_module_event(MODULE_INFO *module, int request)
{
    log_dbg("Set the module event request[%d]\n", request);
    pthread_mutex_lock(&module->lock);
    module->event = request;
    pthread_mutex_unlock(&module->lock);
}

/*
 *  Description: Set the GPRS power status according to the module request event. 
 *               the GPRS module USB convert serial comport device lost, so we must bind them together.
 * Return Value: 0:Set GPRS power status successfully  !0: Open failure
 */
int set_module_event_power(MODULE_INFO *module)
{
    int           rv = 0;
    switch (module->event)
    { 
        case REQ_POWER_ON:
            rv = power_on_module(module);
            break; 

        case REQ_POWER_OFF: 
            rv = power_off_module(module);
            break; 
        
        case REQ_POWER_RESET: 
            rv = power_off_module(module);
            rv = power_on_module(module); 
            break;
    } 
    
    if( rv )
    {
        log_nrml("Response for the GPRS request event %d failure, rv=%d\n", module->event, rv);
        return rv;
    }
    else
    { 
        log_nrml("Response for the GPRS request event %d and clear this event ok\n", module->event);
        set_module_event(module, REQ_EVENT_NONE);
        return 0;
    }
}


int atcmd_inspect_status(MODULE_INFO *module)
{
    COM_PORT        *comport;
    REGISTER_INFO   *reg = &(module->reg);   /* SIM card Register information  */ 
    char            *who = "atcmd_inspect_status()";

    static unsigned long start_time ;

    /* Every 10 seconds we will inspect it */
    if(start_time && time_elapsed(start_time) < 10000)
    {
        return 0;
    }

    start_time= time_now();

    comport = alloc_gsm_dataport(who, module);
    if(NULL != comport)
    {
        log_dbg("Alloc dataport %s=>[fd:%d]\n", comport->dev_name, comport->fd);
    }
    else
        return -1;

    reg->signal = atcmd_check_gprs_signal(comport);

    atcmd_check_gprs_location(comport, &reg->loc);

    log_dbg("Free dataport %s=>[fd:%d]\n", comport->dev_name, comport->fd);
    free_gsm_dataport(who, module, &comport);

    return 0;
}


/*
 * Description: Check the AT command and SIM card ready or not
 * Input value: times:  Max check times
 *Return Value: 1: Already OK   0: Still on test  -1: Failure
 *
 */ 
int atcmd_check_module_ready(COM_PORT *comport, int times)
{
    int           rv;
    static int    fail_cnt = 0;

    /* Send ATE0 to check GPRS module AT ready or not  */
    rv = atcmd_check_at_ready(comport);
    if(rv) 
        goto cleanup;

    /* Send AT+CPIN? to check SIM card valid or not  */
    rv = atcmd_check_sim_valid(comport);

cleanup:
    if(rv)
    {
        if(fail_cnt < times)
        {
            fail_cnt ++;
            return 0;
        }
        else
        {
            fail_cnt = 0;
            return -1;
        }
    }
    else
    {
        fail_cnt = 0;
        return 1;
    }
}

int atcmd_check_power_on(COM_PORT *comport)
{
    int           i, rv = 0;

    for(i=0; i<10; i++)
    {
        if( !atcmd_check_at_ready(comport) )
        {
            rv = 1;
            break;
        }
    }

    return rv;
}

/*
 * Some preset command here, such as set Module as 2G mode
 */ 
int atcmd_module_preset(COM_PORT *comport)
{
    int           retval = 0;

    /* Send AT+COPS=0 to set GPRS module auto select an operator  */
    if(0 != (retval=send_atcmd_check_ok(comport, "AT+COPS=0\r", 3000)) )
    {
        log_warn("AT+COPS=0 Set Auto Select Carrier:     [FAILED]\n");
        return -2;
    } 
    log_nrml("AT+COPS=0 Set Auto Select Carrier:   [OK]\n");

    return retval;
}


int atcmd_check_hwinfo(COM_PORT *comport, HW_INFO *hw, int times)
{
    int           rv;
    static int    fail_cnt = 0;

    /* Send AT+CGMM to check GPRS module hardware name and type */
    rv = atcmd_check_gprs_name(comport, hw->model);
    if(rv)
        goto cleanup;

    /* Send AT+CGMR to check GPRS module hardware version */
    rv = atcmd_check_gprs_version(comport, hw->mrev);
    if(rv)
        goto cleanup;

    /* Send AT+CGSN to check GPRS module hardware IEMI number*/
    rv = atcmd_check_gprs_iemi(comport, hw->iemi);

cleanup:
    if(rv)
    {
        if(fail_cnt < times)
        {
            fail_cnt ++;
            return 0;
        }
        else
        {
            fail_cnt = 0;
            return -1;
        }
    }
    else
    {
        fail_cnt = 0;
        return 1;
    }
}


int atcmd_check_regist(COM_PORT *comport, REGISTER_INFO *reg, int times)
{
    int           rv = 0;
    static int    fail_cnt = 0;

    /* Send AT+CSQ to check GPRS signal strength */
    reg->signal = atcmd_check_gprs_signal(comport);
    if(reg->signal<2 && reg->signal>31)
    {
        rv = -1;
        goto cleanup;
    }

    /* Send AT+CREG? to check SIM card regist to network or not */
    reg->type = atcmd_check_gprs_register(comport);
    
    if(REG_HOMEWORK==reg->type || REG_ROAMING== reg->type)
    {
        /* SIM card register successfully */
        log_nrml("SIM card register successfully.\n");
        rv = 0;
    }
    else if(REG_DENIED==reg->type)
    {
        /* SIM card can not register, so request to switch SIM card */
        log_err("SIM card regist denied.\n");
        rv = -2;
        goto cleanup;
    }
    else
    {
        log_err("SIM card regist failure.\n");
        rv = -3;
        goto cleanup;
    }

    rv = atcmd_check_gprs_carrier(comport, reg->carrier);
    if(rv)
    {
        rv = -4;
        log_err("Check SIM card acrrier failure.\n");
        goto cleanup;
    }

    rv = atcmd_check_gprs_mcc_mnc(comport, reg->loc.mcc_mnc);
    if(rv)
    {
        rv = -4;
        log_err("Check SIM card register MCC-MNC failure\n");
        goto cleanup;
    }


cleanup:
    if(rv)
    {
        if(fail_cnt < times)
        {
            fail_cnt ++;
            return 0;
        }
        else
        {
            fail_cnt = 0;
            return -1;
        }
    }
    else
    {
        fail_cnt = 0;
        return 1;
    }

}

int atcmd_check_network_info(COM_PORT *comport, REGISTER_INFO *reg)
{
    int           retval = 0;

    /* Send AT+COPS to check SIM card registed network carrier */
    retval = atcmd_check_gprs_carrier(comport, reg->carrier);
    if(retval) return -1;

    /* Send AT+QGSMLOC=1 to check GPRS module location */
    retval = atcmd_check_gprs_location(comport, &(reg->loc));
    if(retval) return -3;

    return retval;
}


int atcmd_check_ready(MODULE_INFO *module)
{ 
    int             rv;
    COM_PORT        *comport;
    char            *who = "atcmd_check_ready()";

    log_nrml("AT command check GPRS registry status now\n");
    /* Alloc a comport to send AT command */
    comport = alloc_gsm_dataport(who, module);
    if(NULL == comport)
    {
        log_warn("Alloc dataport for %s() failure.\n", __FUNCTION__);
        return -1;
    }
    log_dbg("Alloc dataport %s=>[fd:%d]\n", comport->dev_name, comport->fd);

    switch(module->ready)
    {
        case MODULE_READY:
            rv = atcmd_check_module_ready(comport, 10);
            if(rv > 0)
            {
                log_nrml("GPRS AT command ready and SIM card valid successfully\n");
                module->ready++;   /* Continue to check, not break */
            }
            else if(rv == 0)
            {
                log_nrml("GPRS AT command ready and SIM card valid check continue...\n");
            }
            else if(rv < 0)
            {
                log_nrml("GPRS AT command ready and SIM card valid check failed\n");
                set_module_event(module, REQ_POWER_RESET);
            }
            break;

        case CHECK_HWINFO:
            rv = atcmd_check_hwinfo(comport, &(module->hw), 10);
            if(rv > 0)
            {
                log_nrml("Check the GPRS Module hardware information successfully\n");
                module->ready++;   /* Continue to check, not break */
            }
            else if(rv == 0)
            {
                log_nrml("Check the GPRS Module hardware information continue...\n");
            }
            else if(rv < 0)
            {
                log_nrml("Check the GPRS Module hardware information failed...\n");
                set_module_event(module, REQ_POWER_RESET);
            }
            break;

        case CHECK_REGISTRY:
            rv = atcmd_check_regist(comport, &(module->reg), 10);
            if(rv > 0)
            {
                log_nrml("Check the SIM card registry successfully\n");
                module->ready++;   /* Continue to check, not break */
            }
            else if(rv == 0)
            {
                log_nrml("Check the SIM card registry continue...\n");
            }
            else if(rv < 0)
            {
                log_nrml("Check the SIM card registry failed\n");
                set_module_event(module, REQ_POWER_RESET);
            }
            break;
    } 
    
    free_gsm_dataport(who, module, &comport);
    return rv;
}


/*
 * Description: Terminate the control thread work context 
 * Return Value: NONE
 */
void ctrl_thread_term(void *thread_arg)
{
    int                i=0;
    MODULE_INFO        *module = (MODULE_INFO *)thread_arg;

    log_nrml("start terminate GPRS main(control) thread\n");
    
    power_off_module(module);

    for(i=0; i<module->comport_cnt; i++)
    {
        comport_term(module->comport[i]);
    } 
}


/* Description: Continue the main thread as the control thread, which used to control/inspect the GPRS 
 *              module power status. If it exit, then whole program exit.So when it exit, we must 
 *              make sure all other threads exit.
 *
 *Return Value: NONE
 */
void ctrl_thread_start(void *thread_arg)
{
    MODULE_INFO        *module = (MODULE_INFO *)thread_arg;

    log_nrml("GPRS power control thread start running\n");

    while( !g_cp_signal.stop )
    {
        /*  If someone request to change the power status, then set the power*/
        if(module->event)
        {
            set_module_event_power(module);
        }

        if(ALL_READY == module->ready)
        {
            /* If GPRS already registed then we just need to inspect the GPRS 
             * module status, such as signal, location */
            atcmd_inspect_status(module);
        }
        else
        {
            /* If GPRS module just already power on, then we use AT command to 
             * check the SIM card can work or not now. */
            if( ON == module->pwr_status )
            {
                atcmd_check_ready(module);
            }
        }

        sleep(1);
    }

    /* thread exit  */
    ctrl_thread_term(thread_arg);

    return ;
}
