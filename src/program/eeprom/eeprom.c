/*********************************************************************************
 *      Copyright:  (C) 2017 LingYun I.o.T Studio  <www.iot-yun.com>
 *                  All rights reserved.
 *
 *       Filename:  eeprom.c
 *    Description:  This file is used to write SN,MAC,Ower into EEPROM(AT24C02)
 *                 
 *        Version:  1.0.0(08/24/2017)
 *         Author:  Guo Wenxue <guowenxue@iot-yun.com>
 *      ChangeLog:  1, Release initial version on "08/24/2017 02:35:28 AM"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <libgen.h>
#include <getopt.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "version.h"

#define EEPROM_SIZE            256
#define EEPROM_DEVPATH         "/sys/bus/i2c/devices/0-0050/eeprom"
#define HWINFO_FILE            "/info/.hwinfo"

#define SN_OFFSET              0x0
#define SN_LEN                 14  /* Serial Number: LY-FL2440-001 */

#define MAC_OFFSET             0x10
#define MAC_LEN                18  /* MAC address  : 00:05:69:00:00:01 */

#define USER_OFFSET            0x30
#define USER_LEN               24  /* Ower name    : guowenxue  */

#define MODE_WR                0
#define MODE_RD                1

int write_eeprom(int fd, int offset, char *data, int len);
int read_eeprom(int fd, int offset, char *buf, int size);
int write_hwinfo_file(const char *file_path, char *sn, char *mac, char *user);

static void prog_usage(const char *progname)
{
    banner(progname); 
    
    printf("Usage: %s [OPTION]...\n", progname);
    printf("This program used to write/read SN,MAC,Name into/from EEPROM\n");

    printf(" -w[write   ]  Write SN,MAC,Name into EEPROM\n");
    printf(" -r[read    ]  read SN,MAC,Name from EEPROM\n");

    printf(" -s[sn      ]  Specify intgegrate serial number, such as 10. Will use it generate SN and MAC address.\n");
    printf(" -u[sn      ]  Specify user name, string such as \"lingyun\"\n");

    printf(" -h[help    ]  Display this help information\n");
    printf(" -v[version ]  Display the program version\n");
    return ;
}


int main (int argc, char **argv)
{
    int                          fd = -1;
    int                          sn = 0;
    char                        *user = NULL;
    char                         buf[EEPROM_SIZE];

    const char                  *progname=NULL;
    int                          opt;
    int                          rwmode = MODE_RD;

    struct option long_options[] = 
    {
        {"write", no_argument, NULL, 'w'},
        {"read", no_argument, NULL, 'r'},
        {"sn", required_argument, NULL, 's'},
        {"user", required_argument, NULL, 'u'},
        {"version", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };

    progname = basename(argv[0]);

    /*  Parser the command line parameters */
    while ((opt = getopt_long(argc, argv, "rws:u:vh", long_options, NULL)) != -1)
    {
        switch (opt)
        {
            case 'w':  /*  Set configure file */
                rwmode = MODE_WR;
                break; 

            case 's':  /*  serial number */
                sn = atoi(optarg);
                break; 
            
            case 'u':  /*  user name */
                user= optarg;
                break; 
            
            case 'v':  /*  Get software version */
                banner(progname); /*  Defined in version.h */
                return EXIT_SUCCESS;

            case 'h':  /*  Get help information */
                prog_usage(progname);
                return 0; 
            
            default:
                break;
        } /*   end of "switch(opt)" */
    }


    if( (fd=open(EEPROM_DEVPATH, O_RDWR)) < 0 )
    {
        printf("Open EEPROM device '%s' failure: %s\n", EEPROM_DEVPATH, strerror(errno));
        return -1;
    }

    if( MODE_WR == rwmode )
    { 
        if( sn && user )
        {
            /* Clear current  EEPROM */
            memset( buf, 0, sizeof(buf) );
            if( write_eeprom(fd, 0, buf, sizeof(buf)) < 0)
            {
                printf("Clear current EEPROM content failure\n");
                goto cleanup;
            } 
        }
        
        if( sn )
        {
            /* Generate and write serial number into EEPROM  */ 
            snprintf(buf, SN_LEN, "LY-FL2440-%03X", sn);
            if( write_eeprom(fd, SN_OFFSET, buf, SN_LEN) < 0)
            {
                printf("Write serial number into EEPROM failure\n");
                goto cleanup;
            }


            /* Generate and write MAC address into EEPROM  */ 
            snprintf(buf, MAC_LEN, "00:05:69:00:00:%02d", sn);
            if( write_eeprom(fd, MAC_OFFSET, buf, MAC_LEN) < 0)
            {
                printf("Write MAC address into EEPROM failure\n");
                goto cleanup;
            }
        } 
        
        if( user )
        {
            /* Write board user name  */
            if( write_eeprom(fd, USER_OFFSET, user, strlen(user)) < 0)
            {
                printf("Write user name into EEPROM failure\n");
                goto cleanup;
            }
        }
    }
    else if( MODE_RD == rwmode )
    { 
        char                   sn[SN_LEN];
        char                   mac[MAC_LEN];
        char                   user[USER_LEN];

        /*  Read serial number from EEPROM  */ 
        memset(sn, 0, sizeof(sn));
        if( read_eeprom(fd, SN_OFFSET, sn, SN_LEN) < 0)
        {
            printf("Read serial number from EEPROM failure\n");
            goto cleanup;
        }
        printf("Serial Number: %s\n", sn); 
        
        /*  Read MAC address from EEPROM  */ 
        memset(mac, 0, sizeof(mac));
        if( read_eeprom(fd, MAC_OFFSET, mac, MAC_LEN) < 0)
        {
            printf("Read MAC address from EEPROM failure\n");
            goto cleanup; 
        }
        printf("MAC Address  : %s\n", mac);

        
        /*  Read user name from EEPROM  */ 
        memset(user, 0, sizeof(user));
        if( read_eeprom(fd, USER_OFFSET, user, USER_LEN) < 0)
        {
            printf("Read user name from EEPROM failure\n");
            goto cleanup;
        }
        
        printf("User Name    : %s\n", user);

        write_hwinfo_file(HWINFO_FILE, sn, mac, user);
    }


cleanup:
    close(fd);

    return 0;
} 


int write_eeprom(int fd, int offset, char *data, int len)
{
    if( fd<0 || (offset<0||offset>=EEPROM_SIZE) || !data || len<=0 )
    {
        printf("Invalid input arguments for %s()\n", __func__);
        return -1;
    }

    lseek(fd, offset, SEEK_SET);

    if( write(fd, data, len) != len )
    {
        printf("write data into EEPROM failure: %s\n", strerror(errno));
        return -2;
    
    }

    return 0;
}


int read_eeprom(int fd, int offset, char *buf, int size)
{
    if( fd<0 || (offset<0||offset>=EEPROM_SIZE) || !buf || size<=0 )
    {
        printf("Invalid input arguments for %s()\n", __func__);
        return -1;
    }

    lseek(fd, offset, SEEK_SET);

    if( read(fd, buf, size) != size )
    {
        printf("write data into EEPROM failure: %s\n", strerror(errno));
        return -2;
    
    }

    return 0;
}

int write_hwinfo_file(const char *file_path, char *sn, char *mac, char *user)
{
    int           fd = -1;
    char          buf[256];

    if( !file_path || !sn || !mac || !user )
    {
        printf("Invalid input arguments for %s()\n", __func__);
        return -1;
    }

    if( (fd=open(file_path, O_RDWR|O_CREAT|O_TRUNC, 0644)) < 0 )
    {
        printf("Open hardware information file '%s' failure: %s\n", file_path, strerror(errno));
        return -2;
    }

    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "SN=%s\nMAC=%s\nUSER=%s\n", sn, mac, user);

    write(fd, buf, strlen(buf));

    close(fd);

    return 0;
}


