/*********************************************************************************
 *      Copyright:  (C) 2013 Guo Wenxue<guowenxue@gmail.com>  
 *                  All rights reserved.
 *
 *       Filename:  test_hh.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(01/28/2013~)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "01/28/2013 03:35:36 PM"
 *                 
 ********************************************************************************/

#include <libgen.h>
#include "cp_comport.h"
#define HH_DATAPORT     "/dev/ttySSHHR"

/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    int             i;
    COM_PORT        *comport = NULL;
    unsigned char   sbuf[10]={0x02, 0x48, 0x32, 0x03, 0x20, 0x00, 0x00, 0xF6, 0xBE, 0x03};
    unsigned char   rbuf[10];
#if 0
    char            *HH_DATAPORT = NULL;

    if(argc != 2)
    {
        printf("Usage: %s [device name]\n", basename(argv[0]));
        return 0;
    }
    HH_DATAPORT = argv[1];
#endif

    if( !(comport=comport_init(HH_DATAPORT, 115200, "8N1N")) )
    {
        printf("Initialise comport %s failure\n", HH_DATAPORT);
        return -1;
    }

    if(comport_open(comport)<0)
    {
        printf("Open comport %s failure\n", HH_DATAPORT);
        return -1;
    }

    if(comport_send(comport, (char *)sbuf, sizeof(sbuf)) < 0)
    {
        printf("Send 10 bytes data to %s failure\n", HH_DATAPORT);
        return -1;
    }
    printf("Send %d bytes data to %s:\n>>", sizeof(sbuf), HH_DATAPORT);
    for(i=0; i<10; i++)
    {
        printf("%02x ", sbuf[i]);
    }
    printf("\n");


    memset(rbuf, 0, sizeof(rbuf));
    if((i=comport_recv(comport, (char *)rbuf, sizeof(rbuf), 5000)) < 0)
    {
        printf("Receive 10 bytes data to %s failure\n", HH_DATAPORT);
        return -1;
    }

    printf("Receive %d bytes data from %s:\n<<", i, HH_DATAPORT);
    for(i=0; i<10; i++)
    {
        printf("%02x ", rbuf[i]);
    }
    printf("\n");

    comport_term(comport);

    return 0;
} /* ----- End of main() ----- */


