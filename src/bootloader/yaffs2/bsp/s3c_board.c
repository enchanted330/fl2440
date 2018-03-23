#include "s3c_board.h"
#include "s3c2440.h"

unsigned int s3c2440_get_fclk(void)
{ 
    struct s3c2440_clock_power *clk_power = s3c2440_get_base_clock_power();
	unsigned int r, m, p, s;

	r = clk_power->MPLLCON;

	m = ((r & 0xFF000) >> 12) + 8;
    p = ((r & 0x003F0) >> 4) + 2;
    s = r & 0x3;

	return (SYS_CLK_FREQ * m * 2) / (p << s);
}
 
unsigned int s3c2440_get_hclk(void)
{
    struct s3c2440_clock_power * const clk_power = s3c2440_get_base_clock_power();
	unsigned int fclk = s3c2440_get_fclk();

	if (clk_power->CLKDIVN & 0x6)
	{
		if ((clk_power->CLKDIVN & 0x6)==2)
			return fclk/2;
		if ((clk_power->CLKDIVN & 0x6)==6)
			return (clk_power->CAMDIVN & 0x100) ? fclk/6 : fclk/3;
		if ((clk_power->CLKDIVN & 0x6)==4)
			return (clk_power->CAMDIVN & 0x200) ? fclk/8 : fclk/4;
		return fclk;
	}

	return fclk;
}
 
unsigned int s3c2440_get_pclk(void)
{
    struct s3c2440_clock_power * const clk_power = s3c2440_get_base_clock_power();
	unsigned int hclk = s3c2440_get_hclk();

    return (clk_power->CLKDIVN & 0x1) ? hclk/2 : hclk;
}
 
unsigned int s3c2440_get_uclk(void)
{
    struct s3c2440_clock_power *clk_power = s3c2440_get_base_clock_power();
	unsigned int r, m, p, s;

	r = clk_power->UPLLCON;

	m = ((r & 0xFF000) >> 12) + 8;
    p = ((r & 0x003F0) >> 4) + 2;
    s = r & 0x3;

	return (SYS_CLK_FREQ * m) / (p << s);
}

void s3c2440_set_baudrate(unsigned int baudrate, int index)
{
	struct s3c2440_uart *uart = s3c2440_get_base_uart(index);
	unsigned int reg = 0;
	int i; 
	reg = s3c2440_get_pclk() / (16 * baudrate) - 1;

	uart->UBRDIV = reg;
	for (i = 0; i < 100; i++);
} 
 
int s3c2440_serial_init(unsigned int baudrate, int index)
{
	struct s3c2440_uart *uart = s3c2440_get_base_uart(index);

	/* FIFO enable, Tx/Rx FIFO clear */
	uart->UFCON = 0x07;
	uart->UMCON = 0x0;

	/* Normal,No parity,1 stop,8 bit */
	uart->ULCON = 0x3;
	/*
	 * tx=level,rx=edge,disable timeout int.,enable rx error int.,
	 * normal,interrupt or polling
	 */
	uart->UCON = (1<<8) | (1<<2) | (1<<0);

//	uart->UMCON = 0x1; /* RTS up */
 
	s3c2440_set_baudrate(baudrate, index);

	return (0);
}

int s3c2440_serail_set_afc(int enable, int index)
{
	if (index != S3C2440_UART0 || index != S3C2440_UART1)
		return -1;

	struct s3c2440_uart *uart = s3c2440_get_base_uart(index);
	if (enable) {
		uart->UMCON = (1<<4);
	} else {
		uart->UMCON = 0;
		uart->UMCON = 0;
		uart->UMCON = 0;
		uart->UMCON = 1;
	}

	return 0;
}

/*
 * Read a single byte from the serial port. Returns 1 on success, 0
 * otherwise. When the function is succesfull, the character read is
 * written into its argument c.
 */
int s3c2440_serial_getc (int index)
{
	struct s3c2440_uart *uart = s3c2440_get_base_uart(index);

	/* wait for character to arrive */
	//while (!(uart->UTRSTAT & 0x1));
	while(!(uart->UFSTAT & 0x1F));

	return uart->URXH & 0xff;
} 

#if 0
static int hwflow = 0; /* turned off by default */
int hwflow_onoff(int on)
{
	switch(on) {
	case 0:
	default:
		break; /* return current */
	case 1:
		hwflow = 1; /* turn on */
		break;
	case -1:
		hwflow = 0; /* turn off */
		break;
	}
	return hwflow;
}
#endif

#if 0 
static int be_quiet = 0;
void disable_putc(void)
{
	be_quiet = 1;
}

void enable_putc(void)
{
	be_quiet = 0;
}
#endif


/*
 * Output a single byte to the serial port.
 */
void s3c2440_serial_putc (char c, int index)
{
	struct s3c2440_uart *uart = s3c2440_get_base_uart(index);

	/* wait for room in the tx FIFO */
	//while ((!(uart->UTRSTAT & 0x2)));
	while (uart->UFSTAT & (1<<14));
	uart->UTXH = c;
}


void s3c2440_serial_puts(const char *s, int index)
{
	while (*s) 
    {
        if (*s == '\n')              /*  If \n, also do \r */
            s3c2440_serial_putc('\r', index);
		s3c2440_serial_putc (*s++, index);
	}
}


void s3c2440_interrupt_enable(unsigned int mask)
{
	mask = ~mask;
	struct s3c2440_interrupt *interrupt = s3c2440_get_base_interrupt();
	interrupt->INTMSK &= mask;
}

void s3c2440_interrupt_disable(unsigned int mask)
{
	struct s3c2440_interrupt *interrupt = s3c2440_get_base_interrupt();
	interrupt->INTMSK |= mask;
}

void s3c2440_sub_interrupt_enable(unsigned int mask)
{
	mask = ~mask;
	struct s3c2440_interrupt *interrupt = s3c2440_get_base_interrupt();
	interrupt->INTSUBMSK &= mask;
}

void s3c2440_sub_interrupt_disable(unsigned int mask)
{
	struct s3c2440_interrupt *interrupt = s3c2440_get_base_interrupt();
	interrupt->INTSUBMSK |= mask;
}



 
