/*********************************************************************************
 *      Copyright:  (C) 2017 Guo Wenxue<guowenxue@aliyun.com>
 *                  All rights reserved.
 *
 *       Filename:  eeprom.c
 *    Description:  This file is used to write SN,MAC,Ower into EEPROM(AT24C02)
 *                 
 *        Version:  1.0.0(08/24/2017)
 *         Author:  Guo Wenxue <guowenxue@aliyun.com>
 *      ChangeLog:  1, Release initial version on "08/24/2017 02:35:28 AM"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>

#ifndef I2C_M_WR
#define I2C_M_WR               0
#endif


/* Internally organized with 32 pages of 8 bytes each.
 * Device Addressing: MSB [1 0 1 0 A2 A1 A0 R/W]: 0x50
 */
#define I2C_SLAVE_ADDR         0x50
#define MSG_BUFSIZE            128


#define SN_OFFSET              0x0
#if 0
#define SN_LEN                 11  /* Serial Number: 001@FL2440 */
#else
#define SN_LEN                 8  /* Serial Number: 001@FL2440 */
#endif

#define MAC_OFFSET             0x10
#define MAC_LEN                18  /* MAC address  : 00:11:22:33:44:55 */

#define NAME_OFFSET            0x30
#define NAME_LEN               24  /* Ower name    : guowenxue  */



int i2c_msg_init(struct i2c_rdwr_ioctl_data  *i2c_msg_data)
{
    i2c_msg_data->nmsgs=2; 
    i2c_msg_data->msgs=(struct i2c_msg*) malloc( i2c_msg_data->nmsgs*sizeof(struct i2c_msg) );
    if(!i2c_msg_data->msgs)
    {
        printf("malloc for I2C eeprom message data failrue\n");
        return  -1;
    }

    i2c_msg_data->msgs[0].buf=(unsigned char*)malloc(MSG_BUFSIZE);
    if( !i2c_msg_data->msgs[0].buf )
    {
        printf("malloc for I2C eeprom message buf[0] failrue\n");
        return  -2;
    }

    i2c_msg_data->msgs[1].buf=(unsigned char*)malloc(MSG_BUFSIZE);
    if( !i2c_msg_data->msgs[1].buf )
    {
        printf("malloc for I2C eeprom message buf[1] failrue\n");
        return  -2;
    }


    return 0;
}

void i2c_msg_term(struct i2c_rdwr_ioctl_data  *i2c_msg_data)
{
    free( i2c_msg_data->msgs[0].buf );
    free( i2c_msg_data->msgs[1].buf );
    free( i2c_msg_data->msgs );
}



/********************************************************************************
 *  Description:
 *   Input Args:
 *  Output Args:
 * Return Value:
 ********************************************************************************/
int main (int argc, char **argv)
{
    int                            fd = -1;
    struct i2c_rdwr_ioctl_data     eeprom_data;

    int                            offset = SN_OFFSET;

    if(argc == 2)
    {
        offset = atoi(argv[1]);
    }


    if( (fd=open("/dev/i2c-0", O_RDWR)) < 0 )
    {
        printf("Open EEPROM I2C device failure: %s\n", strerror(errno));
        return -1;
    }

    ioctl(fd,I2C_TIMEOUT, 2);
    ioctl(fd,I2C_RETRIES, 2);

    if( i2c_msg_init(&eeprom_data) < 0 )
    {
        printf("Initial I2C message context failure\n");
        return -2;
    }


    /*+------------------------------------+
      |  Write serial number into EEPROM   |
      +------------------------------------+*/
    eeprom_data.nmsgs=1;

    eeprom_data.msgs[0].addr=I2C_SLAVE_ADDR;
    eeprom_data.msgs[0].flags= I2C_M_WR; 
    eeprom_data.msgs[0].len=SN_LEN+1;
    eeprom_data.msgs[0].buf[0]=offset; /* EEPROM internal offset address  */

    memset(eeprom_data.msgs[0].buf, 0, MSG_BUFSIZE);
    strncpy((char *)(eeprom_data.msgs[0].buf+1), "001@FL2440", MSG_BUFSIZE-1);
    //strncpy( (char *)(eeprom_data.msgs[0].buf+1), "001@FL2440" , MSG_BUFSIZE-1);
    //strncpy( (char *)(eeprom_data.msgs[0].buf+1), "@abcdefghijk@" , MSG_BUFSIZE-1);


    {
        int i;
        printf("write %d bytes data into eeprom\n", eeprom_data.msgs[0].len-1);
        for(i=0; i<SN_LEN; i++)
        {
            printf("%02x ", eeprom_data.msgs[0].buf[i+1]);
        }
        printf("\n");
    
    }

    if( ioctl(fd, I2C_RDWR, (unsigned long)&eeprom_data ) <  0 )
    {
        printf("ioctl write data into EEPROM failure: %s\n", strerror(errno));
        goto cleanup;
    }

    sleep(1);


#if 1
    /*+------------------------------------+
      |   Read serial number from EEPROM   |
      +------------------------------------+*/
    eeprom_data.nmsgs=2;

    eeprom_data.msgs[0].addr=I2C_SLAVE_ADDR;
    eeprom_data.msgs[0].flags= I2C_M_WR; 
    eeprom_data.msgs[0].len=1;
    //eeprom_data.msgs[0].buf[0]=SN_OFFSET; /* EEPROM internal offset address  */
    eeprom_data.msgs[0].buf[0]=0; /* EEPROM internal offset address  */


    eeprom_data.msgs[1].addr=I2C_SLAVE_ADDR;
    eeprom_data.msgs[1].flags= I2C_M_RD; 
    eeprom_data.msgs[1].len=128;
    memset(eeprom_data.msgs[1].buf, 0, MSG_BUFSIZE);


    if( ioctl(fd, I2C_RDWR, (unsigned long)&eeprom_data ) <  0 )
    {
        printf("ioctl read data from EEPROM failure: %s\n", strerror(errno));
        goto cleanup;
    }

    {
        int i;
        printf("read %d bytes data from eeprom\n", eeprom_data.msgs[1].len);
        for(i=0; i<eeprom_data.msgs[1].len; i++)
        {
            printf("%02x ", eeprom_data.msgs[1].buf[i]);
        }
        printf("\n");
    
    }
#endif

cleanup:
    i2c_msg_term(&eeprom_data);

    close(fd);

    return 0;
} 

