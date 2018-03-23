#ifndef __S3C_BOARD_H__
#define __S3C_BOARD_H__

#include "s3c2440.h"

#define SYS_CLK_FREQ             12000000 
#define CONSOLE_SERIAL           S3C2440_UART0
#define CONSOLE_BAUDRATE         115200

unsigned int s3c2440_get_fclk(void);
unsigned int s3c2440_get_hclk(void);
unsigned int s3c2440_get_pclk(void);
unsigned int s3c2440_get_uclk(void);

void s3c2440_set_baudrate(unsigned int baudrate, int index);
int s3c2440_serial_init(unsigned int baudrate, int index);
int s3c2440_serail_set_afc(int enable, int index);
int s3c2440_serial_getc (int index);
void s3c2440_serial_putc (char ch, int index);
int s3c2440_serial_tstc(int index);
void s3c2440_serial_puts(const char *s, int index);

#define console_serial_init()      s3c2440_serial_init(CONSOLE_BAUDRATE, CONSOLE_SERIAL)
#define console_serial_getc()      s3c2440_serial_getc(CONSOLE_SERIAL)
#define console_serial_tstc()      s3c2440_serial_tstc(CONSOLE_SERIAL)
#define console_serial_putc(c)     s3c2440_serial_putc(c, CONSOLE_SERIAL)
#define console_serial_puts(s)     s3c2440_serial_puts(s, CONSOLE_SERIAL)

void s3c2440_interrupt_enable(unsigned int mask);
void s3c2440_interrupt_disable(unsigned int mask);
void s3c2440_sub_interrupt_enable(unsigned int mask);
void s3c2440_sub_interrupt_disable(unsigned int mask);

#endif
