/*********************************************************************************
 *      Copyright:  (C) 2014 GuoWenxue<guowenxue@email.com>
 *                  All rights reserved.
 *
 *       Filename:  cvd_conf.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(11/19/2014)
 *         Author:  GuoWenxue <guowenxue@email.com>
 *      ChangeLog:  1, Release initial version on "11/19/2014 01:58:28 PM"
 *                 
 ********************************************************************************/

#include "cp_logger.h"
#include "cvd_conf.h"
#include "cp_iniparser.h"


int parser_cvd_conf(char *ini_name, cp_logger *logger, comport_sock_bind_t *cvd_bind)
{
    dictionary          *ini;
    char                *str;
    int                 val, i;

    if(!ini_name || !logger || !cvd_bind)
    {
        log_err("Invalid input arguments\n");
        return -1 ;
    }

    ini = iniparser_load(ini_name);
    if (ini==NULL)
    {
        log_err("cannot parse file: %s\n", ini_name);
        return -2 ;
    }

    /************************************************
     * Parser the Logger configuration
     ************************************************/

    log_nrml("Parser configure file %s with original logger file [%s]\n", ini_name, logger->file);

    str = iniparser_getstring(ini, "log:file", NULL);
    strncpy(logger->file, (strlen(str)>0 ? str : CVD_DEF_LOG_FILE), FILENAME_LEN);
    log_nrml("Set log file [%d] from configuration.\n", logger->level);

    /*If not set the log level from command line, then use the configure one*/
    if( !(logger->flag&CP_LOGGER_LEVEL_OPT) )
    {
        val = iniparser_getint(ini, "log:level", -1);
        logger->level = (-1==val ? LOG_LEVEL_NRML : val);
        log_nrml("Set log level[%d] from configuration.\n", logger->level);
    }

    /* Set the log maximum file size in the configure file */
    val = iniparser_getint(ini, "log:size", -1);
    logger->size = -1==val ? CVD_DEF_LOG_SIZE : val;
    log_nrml("Set log size [%dKiB] from configuration\n", logger->size);

    if ( cp_log_reopen() )
    {
        printf("Log file reopen failure, exit now...\n");
        return -3;
    }

    log_nrml("Configure Log file \"%s\" with level [%s], maximum size %d KiB.\n",
            logger->file, log_str[logger->level], logger->size);


    for(i=0; i<MAX_BIND_TUNNEL; i++)
    {
        char        key[64];

        snprintf(key, sizeof(key), "TUNNEL%d:comport", i+1);
        str = iniparser_getstring(ini, key, NULL);
        if( NULL!=str && strlen(str) > 0 )
        {
            strncpy(cvd_bind->comport_dev, str, DEVNAME_LEN);
            log_info("Configure comport: %s\n", cvd_bind->comport_dev);
        }
        else
        {
            log_err("Configure file lost serial port configuration\n");
            return -3;
        }

        /*-----------------------------------------------------------------------------
         *  Connect socket configuration
         *-----------------------------------------------------------------------------*/

        snprintf(key, sizeof(key), "TUNNEL%d:conn_host", i+1);
        str = iniparser_getstring(ini, key, NULL);
        if( NULL!=str && strlen(str) > 0 )
        {
            char      *ptr;
            ptr = strtok(str, ":");
            if(ptr)
            {
                strncpy(cvd_bind->sock.host, ptr, DOMAIN_MAX_LEN);
                if( NULL != (ptr=strtok(NULL, ":")) )
                {
                    cvd_bind->sock.port = atoi(ptr);
                }
                else
                {
                    log_err("Configure file lost connect port configuration\n");
                    return -4;
                }
            }
            else
            {
                log_err("Configure file lost connect host configuration\n");
                return -5;
            }
        }
        log_nrml("Bind tunnel serial[%s] <==> socket[%s:%d]\n", cvd_bind->comport_dev, cvd_bind->sock.host, cvd_bind->sock.port);
    }

    return 0;
}
