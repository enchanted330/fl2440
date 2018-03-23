/********************************************************************************
 *      Copyright:  (C) 2014 Guo Wenxue<guowenxue@gmail.com>
 *                  All rights reserved.
 *
 *       Filename:  tlv_pack.h
 *    Description:  This head file 
 *
 *        Version:  1.0.0(03/19/2014)
 *         Author:  Guo Wenxue <guowenxue@gmail.com>
 *      ChangeLog:  1, Release initial version on "03/19/2014 08:53:03 AM"
 *                 
 ********************************************************************************/

#ifndef  _TLV_PACK_H_
#define  _TLV_PACK_H_

//#define TLV_PACK_DEBUG
#define TLV_MGIC_CRC           0xEC60

/* Common TLV message API */

/* TLV message format:
 * <2B Tag> <2B Length> <nB Value> <2B CRC>
 */ 
#define TLV_TAG_SIZE           2 
#define TLV_LEN_SIZE           2  
#define TLV_CRC_SIZE           2  

#define TLV_HEAD_SIZE          (TLV_TAG_SIZE+TLV_LEN_SIZE)
#define TLV_MIN_SIZE           (TLV_HEAD_SIZE+TLV_CRC_SIZE)

#define TLV_BUF_SIZE          512

/*-----------------------------------------------------------------------------
 * Lowlevel TLV packet API
 *-----------------------------------------------------------------------------*/
extern int ushort_to_bytes(unsigned char *bytes, unsigned short val);
extern int uint_to_bytes(unsigned char *bytes, unsigned int val);
extern unsigned short bytes_to_ushort(unsigned char *bytes, int len);
extern unsigned int bytes_to_uint(unsigned char *bytes, int len);

extern int tlv_encode_tl(unsigned char *obuf, unsigned short tag, unsigned short len);
extern int tlv_decode_tl(unsigned char *ibuf, unsigned short *tag, unsigned short *len);

/*-----------------------------------------------------------------------------
 * Highlevel user packet API, used to generate the TLV packet
 *-----------------------------------------------------------------------------*/

/*  Description: Print the buffer content, just for debug used
 *   Input args: prfx : The prefix used to show what's the buffer mean;
 *               buf  : the buffer pointer
 *               len  : the buffer data length
 *  Output args: NONE
 * Return value: NONE
 */
extern void print_buf(char *prfx, unsigned char *buf, int len);

/*  Description: Packet the TLV packet
 *   Input args:
 *               $obuf_size: TLV packet output buf($tlv_obuf) size
 *               $tag: TLV tag
 *               $len: TLV length
 *               $val: TLV value  
 *  Output args: $tlv_obuf: TLV packet output buffer
 * Return value: 0: Failure >0: Output TLV packet length
 */
extern int tlv_pack(unsigned char *tlv_obuf, int obuf_size, unsigned short tag, unsigned short len, unsigned char *val);


/*  Description: Unpacket the TLV packet pointer by $tlv_data
 *   Input args: 
 *               $tlv_msg: TLV packet message buffer
 *               $msg_len: TLV packet message length
 *
 *  Output args:  
 *               $tag: TLV tag
 *               $len: TLV length
 *               $val: TLV value  
 * Return value: 0-> OK     -1-> Failure 
 */
extern int tlv_unpack(unsigned char *tlv_msg, int msg_len, unsigned short *tag, unsigned short *len, unsigned char *val);


#endif   /* ----- #ifndef _SMS_TLV_H_  ----- */



