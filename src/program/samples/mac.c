/*********************************************************************************
 *      Copyright:  (C) 2012 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  test.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(10/25/2012~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "10/25/2012 02:35:30 PM"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAC_HEX_LEN    6
#define MAC_STR_LEN    18

int mac_str2hex(char *mac, char *buf, int len)
{
    if(!mac ||!buf || len<6 || !strchr(mac, ':') )
        return -1;

    sscanf(mac, "%02x:%02x:%02x:%02x:%02x:%02x", 
            &buf[0], &buf[1], &buf[2], &buf[3], &buf[4], &buf[5]);

    return 0;
}


char *mac_hex2str(char *hex_mac, char *buf, int len)
{
    if( !hex_mac || !buf || len<6 )
        return NULL;

    snprintf(buf, len, "%02x:%02x:%02x:%02x:%02x:%02x", 
            hex_mac[0],hex_mac[1],hex_mac[2],hex_mac[3],hex_mac[4],hex_mac[5]);

    return buf;
}

/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    int i;
    char *mac="00:11:22:33:44:55";
    char hex_mac[MAC_HEX_LEN];
    char str_mac[MAC_STR_LEN];

    memset(hex_mac, 0, sizeof(hex_mac));
    mac_str2hex(mac, hex_mac, sizeof(hex_mac));

    printf("MAC in HEX mode:");
    for(i=0; i<MAC_HEX_LEN; i++)
        printf("0x%02x ", hex_mac[i]);
    printf("\n");

    memset(str_mac, 0, sizeof(str_mac));
    mac_hex2str(hex_mac, str_mac, sizeof(str_mac));
    printf("MAC in STR mode: %s\n", str_mac);

    return 0;
} /* ----- End of main() ----- */

