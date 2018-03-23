/********************************************************************************************
 *        File:  common.h
 *     Version:  1.0.0
 *   Copyright:  2011 (c) Guo Wenxue <guowenxue@gmail.com>
 * Description:  A busy head file
 *   ChangeLog:  1, Release initial version on "Tue Jul 12 16:43:18 CST 2011"
 *
 *******************************************************************************************/

#ifndef __COMMON_H
#define __COMMON_H

#include <stdarg.h>
#include <types.h>

#ifdef DEBUG
#define dbg_print(format,args...) printf(format, ##args)
#else
#define dbg_print(format,args...) do{} while(0);
#endif

void serial_init(void);
void serial_send_byte(char c);
int serial_is_recv_enable(void);
int serial_recv_byte(void);
void serial_puts(const char *s);

/*Define in printf.c*/
void printf(const char *fmt, ...);
int sprintf(char *buf, const char *fmt, ...);

#endif
