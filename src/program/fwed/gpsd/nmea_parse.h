/*
 *
 * NMEA library
 * URL: http://nmea.sourceforge.net
 * Author: Tim (xtimor@gmail.com)
 * Licence: http://www.gnu.org/licenses/lgpl.html
 * $Id: parse.h 4 2007-08-27 13:11:03Z xtimor $
 *
 */

#ifndef __NMEA_PARSE_H__
#define __NMEA_PARSE_H__

#include "nmea_info.h"

#define NMEA_DEF_PARSEBUFF  (1024)
#define NMEA_MIN_PARSEBUFF  (256)

#define NMEA_CONVSTR_BUF    (256)
#define NMEA_TIMEPARSE_BUF  (256)

/*
 * Distance units
 */

#define NMEA_TUD_YARDS      (1.0936)        /* *< Yeards, meter * NMEA_TUD_YARDS = yard */
#define NMEA_TUD_KNOTS      (1.852)         /* *< Knots, kilometer / NMEA_TUD_KNOTS = knot */
#define NMEA_TUD_MILES      (1.609)         /* *< Miles, kilometer / NMEA_TUD_MILES = mile */

/*
 * Speed units
 */

#define NMEA_TUS_MS         (3.6)           /* *< Meters per seconds, (k/h) / NMEA_TUS_MS= (m/s) */


#ifdef  __cplusplus
extern "C" {
#endif

int     nmea_atoi(const char *str, int str_sz, int radix);
double  nmea_atof(const char *str, int str_sz);
int     nmea_scanf(const char *buff, int buff_sz, const char *format, ...);

int nmea_pack_type(const char *buff, int buff_sz);
int nmea_find_tail(const char *buff, int buff_sz, int *res_crc);
int nmea_get_item_line(const char *buff, int buff_sz, char *line, int *line_len);

int nmea_parse_GPGGA(const char *buff, int buff_sz, nmeaGPGGA *pack);
int nmea_parse_GPGSA(const char *buff, int buff_sz, nmeaGPGSA *pack);
int nmea_parse_GPGSV(const char *buff, int buff_sz, nmeaGPGSV *pack);
int nmea_parse_GPRMC(const char *buff, int buff_sz, nmeaGPRMC *pack);
int nmea_parse_GPVTG(const char *buff, int buff_sz, nmeaGPVTG *pack);

void nmea_GPGGA2info(nmeaGPGGA *pack, nmeaINFO *info);
void nmea_GPGSA2info(nmeaGPGSA *pack, nmeaINFO *info);
void nmea_GPGSV2info(nmeaGPGSV *pack, nmeaINFO *info);
void nmea_GPRMC2info(nmeaGPRMC *pack, nmeaINFO *info);
void nmea_GPVTG2info(nmeaGPVTG *pack, nmeaINFO *info);

#ifdef  __cplusplus
}
#endif

#endif /* __NMEA_PARSE_H__ */
