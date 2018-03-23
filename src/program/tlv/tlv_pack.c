/*********************************************************************************
 *      Copyright:  (C) 2014 Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  sms_tlv.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(03/19/2014)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "03/19/2014 09:13:00 AM"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "tlv_pack.h"
#include "crc-itu-t.h"

void print_buf(char *prfx, unsigned char *buf, int len)
{
#ifdef TLV_PACK_DEBUG
    int              i;

    if(prfx)
        printf("[%d] bytes %s", len, prfx);

    for(i=0; i<len; i++)
    {
        printf("0x%02x ", buf[i]);
    }
    printf("\n");
#endif
}


int ushort_to_bytes(unsigned char *bytes, unsigned short val)
{
    int              size  = sizeof (unsigned short);
    int              i = 0;

    if( !bytes )
        return 0;

    while (size)
    {
        *(bytes + --size) = (val >> ((8 * i++) & 0xFF));
    }

    return sizeof(unsigned short);
}

int uint_to_bytes(unsigned char *bytes, unsigned int val)
{
    int              size  = sizeof (unsigned int);
    int              i = 0;

    if( !bytes )
        return 0;

    while (size)
    {
        *(bytes + --size) = (val >> ((8 * i++) & 0xFF));
    }

    return sizeof(unsigned int);
}


unsigned short bytes_to_ushort(unsigned char *bytes, int len)
{
    int            i = 0;
    unsigned short val = 0;

    if( !bytes || len > sizeof(unsigned short) )
        return 0;

    for(i=0; i<len; i++)
    {
        val += bytes[i];

        if (i < len - 1)
            val = val << 8;
    }

    return val;
}


unsigned int bytes_to_uint(unsigned char *bytes, int len)
{
    int            i = 0;
    unsigned int   val = 0;

    if( !bytes || len > sizeof(unsigned int) )
        return 0;

    for(i=0; i<len; i++)
    {
        val += bytes[i];
        if (i < len - 1)
            val = val << 8;
    }

    return val;
}


int tlv_encode_tl(unsigned char *obuf, unsigned short tag, unsigned short len)
{
    int           oft = 0;

    ushort_to_bytes(&obuf[oft], tag);
    oft += TLV_TAG_SIZE;

    ushort_to_bytes(&obuf[oft], len);
    oft += TLV_LEN_SIZE;

    return oft;
}

int tlv_decode_tl(unsigned char *ibuf, unsigned short *tag, unsigned short *len)
{
    int           oft = 0;

    *tag = bytes_to_ushort(&ibuf[oft], TLV_TAG_SIZE);
    oft += TLV_TAG_SIZE;

    *len = bytes_to_ushort(&ibuf[oft], TLV_LEN_SIZE);
    oft += TLV_LEN_SIZE;

    return oft;
}

int tlv_pack(unsigned char *tlv_obuf, int obuf_size, unsigned short tag, unsigned short len, unsigned char *val)
{
    int              oft = 0;
    unsigned short   crc = 0;

    if( !tlv_obuf )
        return 0;

    /* If $val is NULL means TLV don't have value, then set length be 0  */
    len = !val ? 0 : len;

    /* TLV packet message output buffer is too small  */
    if( obuf_size < (TLV_MIN_SIZE+len) )
        return 0;

    /* Fill Tag and Length  */
    oft += tlv_encode_tl(tlv_obuf, tag, len);

    /* Fill Value  */
    memcpy(&tlv_obuf[TLV_HEAD_SIZE], val, len);
    oft += len;

    crc = crc_itu_t(TLV_MGIC_CRC, tlv_obuf, oft);

    oft += ushort_to_bytes(&tlv_obuf[oft], crc);

    return oft;
}

int tlv_unpack(unsigned char *tlv_msg, int msg_len, unsigned short *tag, unsigned short *len, unsigned char *val)
{
    int              oft = 0;
    unsigned short   crc = 0;

    if(!tlv_msg || !tag || !len || !val || msg_len<TLV_MIN_SIZE)
        return -1;

    /* Decode the Tag and length  */
    oft = tlv_decode_tl(tlv_msg, tag, len);

    /* Last two bytes in $tvl_msg is CRC value, which is calculate TLV */
    crc = bytes_to_ushort(&tlv_msg[oft+*len], sizeof(short));
    if(crc != crc_itu_t(TLV_MGIC_CRC, tlv_msg, TLV_HEAD_SIZE+*len) )
    {
        return -2;
    }

    /* be careful $val maybe overflow, so $val buffer size should be larger enuf */
    memcpy(val, &tlv_msg[oft], *len);

    return 0;
}

#define _TEST_MAIN_
#ifdef _TEST_MAIN_

#include <stdio.h>
#define TAG_TEST    0xBEEF

int main(int argc, char **argv)
{
    unsigned char     tlv_buf[TLV_BUF_SIZE];
    int               tlv_len ;
    char              *str="hello world!";

    unsigned short    tag, len;
    unsigned char     msg[50];

    tlv_len = tlv_pack(tlv_buf, sizeof(tlv_buf), TAG_TEST, (unsigned short)strlen(str), (unsigned char *)str);
    if(tlv_len < 0)
    {
        printf("tlv_pack failure\n");
        return -1;
    }

    if( tlv_unpack(tlv_buf, tlv_len, &tag, &len, msg) < 0)
    {
        printf("tlv_unpack failure\n");
        return -2;
    }

    printf("tlv_unpack: tag=0x%04x len=%u value: %s\n", tag, len, msg);

    return 0;
}
#endif

