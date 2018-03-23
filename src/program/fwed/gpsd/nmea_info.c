/*
 *
 * NMEA library
 * URL: http://nmea.sourceforge.net
 * Author: Tim (xtimor@gmail.com)
 * Licence: http://www.gnu.org/licenses/lgpl.html
 * $Id: sentence.c 17 2008-03-11 11:56:11Z xtimor $
 *
 */

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#include "nmea_parse.h"
#include "nmea_info.h"
#include "cp_logger.h"

void nmea_zero_GPGGA(nmeaGPGGA *pack)
{
    memset(pack, 0, sizeof(nmeaGPGGA));
    nmea_time_now(&pack->utc);
    pack->ns = 'N';
    pack->ew = 'E';
    pack->elv_units = 'M';
    pack->diff_units = 'M';
}

void nmea_zero_GPGSA(nmeaGPGSA *pack)
{
    memset(pack, 0, sizeof(nmeaGPGSA));
    pack->fix_mode = 'A';
    pack->fix_type = NMEA_FIX_BAD;
}

void nmea_zero_GPGSV(nmeaGPGSV *pack)
{
    memset(pack, 0, sizeof(nmeaGPGSV));
}

void nmea_zero_GPRMC(nmeaGPRMC *pack)
{
    memset(pack, 0, sizeof(nmeaGPRMC));
    nmea_time_now(&pack->utc);
    pack->status = 'V';
    pack->ns = 'N';
    pack->ew = 'E';
    pack->declin_ew = 'E';
}

void nmea_zero_GPVTG(nmeaGPVTG *pack)
{
    memset(pack, 0, sizeof(nmeaGPVTG));
    pack->dir_t = 'T';
    pack->dec_m = 'M';
    pack->spn_n = 'N';
    pack->spk_k = 'K';
}

void nmea_zero_INFO(nmeaINFO *info)
{
    memset(info, 0, sizeof(nmeaINFO));
    nmea_time_now(&info->utc);
    info->sig = NMEA_SIG_BAD;
    info->fix = NMEA_FIX_BAD;
}

void nmea_time_now(nmeaTIME *stm)
{
    time_t lt;
    struct tm *tt;

    time(&lt);
    tt = gmtime(&lt);

    stm->year = tt->tm_year;
    stm->mon = tt->tm_mon;
    stm->day = tt->tm_mday;
    stm->hour = tt->tm_hour;
    stm->min = tt->tm_min;
    stm->sec = tt->tm_sec;
    stm->hsec = 0;
}

/* buf sample: "$GPRMC,134325.000,A,3029.8871,N,11423.1932,E,0.26,258.74,210113,,,A*"  */
int nmea_parse_line(char *buf, int buf_sz, nmeaINFO *info)
{
    int       type;
    void      *pack;

    type = nmea_pack_type(buf+1, buf_sz); /* Skip '$' */
    switch (type) 
    {
        case GPGGA:
            if( !(pack = malloc(sizeof(nmeaGPGGA))) ) 
            {
                goto mem_fail;
            }

            if( nmea_parse_GPGGA(buf, buf_sz, (nmeaGPGGA *)pack) )
            {
                nmea_GPGGA2info(pack, info);
            }
            free(pack);
            break;

        case GPGSA:
            if( !(pack = malloc(sizeof(nmeaGPGSA))) ) 
            {
                goto mem_fail;
            }

            if( nmea_parse_GPGSA(buf, buf_sz, (nmeaGPGSA *)pack) ) 
            {
                nmea_GPGSA2info(pack, info);
            }
            free(pack);
            break; 
        
        case GPGSV:
            if( !(pack = malloc(sizeof(nmeaGPGSV))) ) 
            {
                goto mem_fail;
            } 
            
            if( nmea_parse_GPGSV(buf, buf_sz, (nmeaGPGSV *)pack) )
            {
                nmea_GPGSV2info(pack, info);
            }
            free(pack);
            break;

        case GPRMC:
            if( !(pack = malloc(sizeof(nmeaGPRMC))) ) 
            {
                goto mem_fail;
            } 
            
            if( nmea_parse_GPRMC(buf, buf_sz, (nmeaGPRMC *)pack) )
            {
                nmea_GPRMC2info(pack, info);
            }
            free(pack);
            break;

        case GPVTG:
            if( !(pack = malloc(sizeof(nmeaGPVTG))) ) 
            {
                goto mem_fail;
            } 
            
            if( nmea_parse_GPVTG(buf, buf_sz, (nmeaGPVTG *)pack) )
            {
                nmea_GPVTG2info(pack, info);
            }
            free(pack);
            break;

        default:
            break;
    }

    return 0;

mem_fail:
    log_err("Insufficient memory!\n");
    return -1;
}

int nmea_parse(char *data, int len, nmeaINFO *info)
{
    int       line_len, nbytes; 
    char      line[512];

    while(len > 0)
    {
        memset(line, 0, sizeof(line));
        nbytes = nmea_get_item_line(data, len, line, &line_len);

        /* Not a integrated line */
        if( line_len< 0 )
            break;

        if( line_len > 0 )
        {
            nmea_parse_line(line, line_len, info);
        }

        data += nbytes;
        len -= nbytes;
    }

    return 0;
}
