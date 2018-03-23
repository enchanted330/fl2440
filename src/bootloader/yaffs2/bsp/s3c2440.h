#ifndef __S3C2440_H__
#define __S3C2440_H__

#define S3C2440_UART_CHANNELS	3
#define S3C2440_SPI_CHANNELS	2

/* Memory controller (see manual chapter 5) */
struct s3c2440_memctl {
	volatile unsigned int	BWSCON;
	volatile unsigned int	BANKCON[8];
	volatile unsigned int	REFRESH;
	volatile unsigned int	BANKSIZE;
	volatile unsigned int	MRSRB6;
	volatile unsigned int	MRSRB7;
};


/* USB HOST (see manual chapter 12) */
struct s3c2440_usb_host {
	volatile unsigned int	HcRevision;
	volatile unsigned int	HcControl;
	volatile unsigned int	HcCommonStatus;
	volatile unsigned int	HcInterruptStatus;
	volatile unsigned int	HcInterruptEnable;
	volatile unsigned int	HcInterruptDisable;
	volatile unsigned int	HcHCCA;
	volatile unsigned int	HcPeriodCuttendED;
	volatile unsigned int	HcControlHeadED;
	volatile unsigned int	HcControlCurrentED;
	volatile unsigned int	HcBulkHeadED;
	volatile unsigned int	HcBuldCurrentED;
	volatile unsigned int	HcDoneHead;
	volatile unsigned int	HcRmInterval;
	volatile unsigned int	HcFmRemaining;
	volatile unsigned int	HcFmNumber;
	volatile unsigned int	HcPeriodicStart;
	volatile unsigned int	HcLSThreshold;
	volatile unsigned int	HcRhDescriptorA;
	volatile unsigned int	HcRhDescriptorB;
	volatile unsigned int	HcRhStatus;
	volatile unsigned int	HcRhPortStatus1;
	volatile unsigned int	HcRhPortStatus2;
};


/* INTERRUPT (see manual chapter 14) */
struct s3c2440_interrupt {
	volatile unsigned int	SRCPND;
	volatile unsigned int	INTMOD;
	volatile unsigned int	INTMSK;
	volatile unsigned int	PRIORITY;
	volatile unsigned int	INTPND;
	volatile unsigned int	INTOFFSET;
	volatile unsigned int	SUBSRCPND;
	volatile unsigned int	INTSUBMSK;
};


/* DMAS (see manual chapter 8) */
struct s3c2440_dma {
	volatile unsigned int	DISRC;
	volatile unsigned int	DISRCC;
	volatile unsigned int	DIDST;
	volatile unsigned int	DIDSTC;
	volatile unsigned int	DCON;
	volatile unsigned int	DSTAT;
	volatile unsigned int	DCSRC;
	volatile unsigned int	DCDST;
	volatile unsigned int	DMASKTRIG;
	volatile unsigned int	res[7];
};

struct s3c2440_dmas {
	struct s3c2440_dma	dma[4];
};


struct s3c2440_clock_power {
	volatile unsigned int	LOCKTIME;
	volatile unsigned int	MPLLCON;
	volatile unsigned int	UPLLCON;
	volatile unsigned int	CLKCON;
	volatile unsigned int	CLKSLOW;
	volatile unsigned int	CLKDIVN;
	volatile unsigned int CAMDIVN;
};


/* LCD CONTROLLER (see manual chapter 15) */
struct s3c2440_lcd {
	volatile unsigned int	LCDCON1;
	volatile unsigned int	LCDCON2;
	volatile unsigned int	LCDCON3;
	volatile unsigned int	LCDCON4;
	volatile unsigned int	LCDCON5;
	volatile unsigned int	LCDSADDR1;
	volatile unsigned int	LCDSADDR2;
	volatile unsigned int	LCDSADDR3;
	volatile unsigned int	REDLUT;
	volatile unsigned int	GREENLUT;
	volatile unsigned int	BLUELUT;
	volatile unsigned int	res[8];
	volatile unsigned int	DITHMODE;
	volatile unsigned int	TPAL;
	volatile unsigned int	LCDINTPND;
	volatile unsigned int	LCDSRCPND;
	volatile unsigned int	LCDINTMSK;
	volatile unsigned int	TCONSEL;
};


/* NAND FLASH (see S3C2440 manual chapter 6) */
struct s3c2440_nand {
	volatile unsigned int	NFCONF;
	volatile unsigned int	NFCONT;
	volatile unsigned int	NFCMD;
	volatile unsigned int	NFADDR;
	volatile unsigned int	NFDATA;
	volatile unsigned int	NFMECCD0;
	volatile unsigned int	NFMECCD1;
	volatile unsigned int	NFSECCD;
	volatile unsigned int	NFSTAT;
	volatile unsigned int	NFESTAT0;
	volatile unsigned int	NFESTAT1;
	volatile unsigned int	NFMECC0;
	volatile unsigned int	NFMECC1;
	volatile unsigned int	NFSECC;
	volatile unsigned int	NFSBLK;
	volatile unsigned int	NFEBLK;
};


/* UART (see manual chapter 11) */
struct s3c2440_uart {
	volatile unsigned int	ULCON;
	volatile unsigned int	UCON;
	volatile unsigned int	UFCON;
	volatile unsigned int	UMCON;
	volatile unsigned int	UTRSTAT;
	volatile unsigned int	UERSTAT;
	volatile unsigned int	UFSTAT;
	volatile unsigned int	UMSTAT;
#ifdef __BIG_ENDIAN
	volatile unsigned char	res1[3];
	volatile unsigned char	UTXH;
	volatile unsigned char	res2[3];
	volatile unsigned char	URXH;
#else /* Little Endian */
	volatile unsigned char	UTXH;
	volatile unsigned char	res1[3];
	volatile unsigned char	URXH;
	volatile unsigned char	res2[3];
#endif
	volatile unsigned int	UBRDIV;
};


/* PWM TIMER (see manual chapter 10) */
struct s3c2440_timer {
	volatile unsigned int	TCNTB;
	volatile unsigned int	TCMPB;
	volatile unsigned int	TCNTO;
};

struct s3c2440_timers {
	volatile unsigned int	TCFG0;
	volatile unsigned int	TCFG1;
	volatile unsigned int	TCON;
	struct s3c2440_timer	ch[4];
	volatile unsigned int	TCNTB4;
	volatile unsigned int	TCNTO4;
};


/* USB DEVICE (see manual chapter 13) */
struct s3c2440_usb_dev_fifos {
#ifdef __BIG_ENDIAN
	volatile unsigned char	res[3];
	volatile unsigned char	EP_FIFO_REG;
#else /*  little endian */
	volatile unsigned char	EP_FIFO_REG;
	volatile unsigned char	res[3];
#endif
};

struct s3c2440_usb_dev_dmas {
#ifdef __BIG_ENDIAN
	volatile unsigned char	res1[3];
	volatile unsigned char	EP_DMA_CON;
	volatile unsigned char	res2[3];
	volatile unsigned char	EP_DMA_UNIT;
	volatile unsigned char	res3[3];
	volatile unsigned char	EP_DMA_FIFO;
	volatile unsigned char	res4[3];
	volatile unsigned char	EP_DMA_TTC_L;
	volatile unsigned char	res5[3];
	volatile unsigned char	EP_DMA_TTC_M;
	volatile unsigned char	res6[3];
	volatile unsigned char	EP_DMA_TTC_H;
#else /*  little endian */
	volatile unsigned char	EP_DMA_CON;
	volatile unsigned char	res1[3];
	volatile unsigned char	EP_DMA_UNIT;
	volatile unsigned char	res2[3];
	volatile unsigned char	EP_DMA_FIFO;
	volatile unsigned char	res3[3];
	volatile unsigned char	EP_DMA_TTC_L;
	volatile unsigned char	res4[3];
	volatile unsigned char	EP_DMA_TTC_M;
	volatile unsigned char	res5[3];
	volatile unsigned char	EP_DMA_TTC_H;
	volatile unsigned char	res6[3];
#endif
};

struct s3c2440_usb_device {
#ifdef __BIG_ENDIAN
	volatile unsigned char	res1[3];
	volatile unsigned char	FUNC_ADDR_REG;
	volatile unsigned char	res2[3];
	volatile unsigned char	PWR_REG;
	volatile unsigned char	res3[3];
	volatile unsigned char	EP_INT_REG;
	volatile unsigned char	res4[15];
	volatile unsigned char	USB_INT_REG;
	volatile unsigned char	res5[3];
	volatile unsigned char	EP_INT_EN_REG;
	volatile unsigned char	res6[15];
	volatile unsigned char	USB_INT_EN_REG;
	volatile unsigned char	res7[3];
	volatile unsigned char	FRAME_NUM1_REG;
	volatile unsigned char	res8[3];
	volatile unsigned char	FRAME_NUM2_REG;
	volatile unsigned char	res9[3];
	volatile unsigned char	INDEX_REG;
	volatile unsigned char	res10[7];
	volatile unsigned char	MAXP_REG;
	volatile unsigned char	res11[3];
	volatile unsigned char	EP0_CSR_IN_CSR1_REG;
	volatile unsigned char	res12[3];
	volatile unsigned char	IN_CSR2_REG;
	volatile unsigned char	res13[7];
	volatile unsigned char	OUT_CSR1_REG;
	volatile unsigned char	res14[3];
	volatile unsigned char	OUT_CSR2_REG;
	volatile unsigned char	res15[3];
	volatile unsigned char	OUT_FIFO_CNT1_REG;
	volatile unsigned char	res16[3];
	volatile unsigned char	OUT_FIFO_CNT2_REG;
#else /*  little endian */
	volatile unsigned char	FUNC_ADDR_REG;
	volatile unsigned char	res1[3];
	volatile unsigned char	PWR_REG;
	volatile unsigned char	res2[3];
	volatile unsigned char	EP_INT_REG;
	volatile unsigned char	res3[15];
	volatile unsigned char	USB_INT_REG;
	volatile unsigned char	res4[3];
	volatile unsigned char	EP_INT_EN_REG;
	volatile unsigned char	res5[15];
	volatile unsigned char	USB_INT_EN_REG;
	volatile unsigned char	res6[3];
	volatile unsigned char	FRAME_NUM1_REG;
	volatile unsigned char	res7[3];
	volatile unsigned char	FRAME_NUM2_REG;
	volatile unsigned char	res8[3];
	volatile unsigned char	INDEX_REG;
	volatile unsigned char	res9[7];
	volatile unsigned char	MAXP_REG;
	volatile unsigned char	res10[7];
	volatile unsigned char	EP0_CSR_IN_CSR1_REG;
	volatile unsigned char	res11[3];
	volatile unsigned char	IN_CSR2_REG;
	volatile unsigned char	res12[3];
	volatile unsigned char	OUT_CSR1_REG;
	volatile unsigned char	res13[7];
	volatile unsigned char	OUT_CSR2_REG;
	volatile unsigned char	res14[3];
	volatile unsigned char	OUT_FIFO_CNT1_REG;
	volatile unsigned char	res15[3];
	volatile unsigned char	OUT_FIFO_CNT2_REG;
	volatile unsigned char	res16[3];
#endif /*  __BIG_ENDIAN */
	struct s3c2440_usb_dev_fifos	fifo[5];
	struct s3c2440_usb_dev_dmas	dma[5];
};


/* WATCH DOG TIMER (see manual chapter 18) */
struct s3c2440_watchdog {
	volatile unsigned int	WTCON;
	volatile unsigned int	WTDAT;
	volatile unsigned int	WTCNT;
};


/* IIC (see manual chapter 20) */
struct s3c2440_i2c {
	volatile unsigned int	IICCON;
	volatile unsigned int	IICSTAT;
	volatile unsigned int	IICADD;
	volatile unsigned int	IICDS;
};


/* IIS (see manual chapter 21) */
struct s3c2440_i2s {
#ifdef __BIG_ENDIAN
	volatile unsigned short	res1;
	volatile unsigned short	IISCON;
	volatile unsigned short	res2;
	volatile unsigned short	IISMOD;
	volatile unsigned short	res3;
	volatile unsigned short	IISPSR;
	volatile unsigned short	res4;
	volatile unsigned short	IISFCON;
	volatile unsigned short	res5;
	volatile unsigned short	IISFIFO;
#else /*  little endian */
	volatile unsigned short	IISCON;
	volatile unsigned short	res1;
	volatile unsigned short	IISMOD;
	volatile unsigned short	res2;
	volatile unsigned short	IISPSR;
	volatile unsigned short	res3;
	volatile unsigned short	IISFCON;
	volatile unsigned short	res4;
	volatile unsigned short	IISFIFO;
	volatile unsigned short	res5;
#endif
};


/* I/O PORT (see manual chapter 9) */
struct s3c2440_gpio {
	volatile unsigned int	GPACON;
	volatile unsigned int	GPADAT;
	volatile unsigned int	res1[2];
	volatile unsigned int	GPBCON;
	volatile unsigned int	GPBDAT;
	volatile unsigned int	GPBUP;
	volatile unsigned int	res2;
	volatile unsigned int	GPCCON;
	volatile unsigned int	GPCDAT;
	volatile unsigned int	GPCUP;
	volatile unsigned int	res3;
	volatile unsigned int	GPDCON;
	volatile unsigned int	GPDDAT;
	volatile unsigned int	GPDUP;
	volatile unsigned int	res4;
	volatile unsigned int	GPECON;
	volatile unsigned int	GPEDAT;
	volatile unsigned int	GPEUP;
	volatile unsigned int	res5;
	volatile unsigned int	GPFCON;
	volatile unsigned int	GPFDAT;
	volatile unsigned int	GPFUP;
	volatile unsigned int	res6;
	volatile unsigned int	GPGCON;
	volatile unsigned int	GPGDAT;
	volatile unsigned int	GPGUP;
	volatile unsigned int	res7;
	volatile unsigned int	GPHCON;
	volatile unsigned int	GPHDAT;
	volatile unsigned int	GPHUP;
	volatile unsigned int	res8;

	volatile unsigned int	MISCCR;
	volatile unsigned int	DCLKCON;
	volatile unsigned int	EXTINT0;
	volatile unsigned int	EXTINT1;
	volatile unsigned int	EXTINT2;
	volatile unsigned int	EINTFLT0;
	volatile unsigned int	EINTFLT1;
	volatile unsigned int	EINTFLT2;
	volatile unsigned int	EINTFLT3;
	volatile unsigned int	EINTMASK;
	volatile unsigned int	EINTPEND;
	volatile unsigned int	GSTATUS0;
	volatile unsigned int	GSTATUS1;
	volatile unsigned int	GSTATUS2;
	volatile unsigned int	GSTATUS3;
	volatile unsigned int	GSTATUS4;
};


/* RTC (see manual chapter 17) */
struct s3c2440_rtc {
#ifdef __BIG_ENDIAN
	volatile unsigned char	res1[67];
	volatile unsigned char	RTCCON;
	volatile unsigned char	res2[3];
	volatile unsigned char	TICNT;
	volatile unsigned char	res3[11];
	volatile unsigned char	RTCALM;
	volatile unsigned char	res4[3];
	volatile unsigned char	ALMSEC;
	volatile unsigned char	res5[3];
	volatile unsigned char	ALMMIN;
	volatile unsigned char	res6[3];
	volatile unsigned char	ALMHOUR;
	volatile unsigned char	res7[3];
	volatile unsigned char	ALMDATE;
	volatile unsigned char	res8[3];
	volatile unsigned char	ALMMON;
	volatile unsigned char	res9[3];
	volatile unsigned char	ALMYEAR;
	volatile unsigned char	res10[3];
	volatile unsigned char	RTCRST;
	volatile unsigned char	res11[3];
	volatile unsigned char	BCDSEC;
	volatile unsigned char	res12[3];
	volatile unsigned char	BCDMIN;
	volatile unsigned char	res13[3];
	volatile unsigned char	BCDHOUR;
	volatile unsigned char	res14[3];
	volatile unsigned char	BCDDATE;
	volatile unsigned char	res15[3];
	volatile unsigned char	BCDDAY;
	volatile unsigned char	res16[3];
	volatile unsigned char	BCDMON;
	volatile unsigned char	res17[3];
	volatile unsigned char	BCDYEAR;
#else /*  little endian */
	volatile unsigned char	res0[64];
	volatile unsigned char	RTCCON;
	volatile unsigned char	res1[3];
	volatile unsigned char	TICNT;
	volatile unsigned char	res2[11];
	volatile unsigned char	RTCALM;
	volatile unsigned char	res3[3];
	volatile unsigned char	ALMSEC;
	volatile unsigned char	res4[3];
	volatile unsigned char	ALMMIN;
	volatile unsigned char	res5[3];
	volatile unsigned char	ALMHOUR;
	volatile unsigned char	res6[3];
	volatile unsigned char	ALMDATE;
	volatile unsigned char	res7[3];
	volatile unsigned char	ALMMON;
	volatile unsigned char	res8[3];
	volatile unsigned char	ALMYEAR;
	volatile unsigned char	res9[3];
	volatile unsigned char	RTCRST;
	volatile unsigned char	res10[3];
	volatile unsigned char	BCDSEC;
	volatile unsigned char	res11[3];
	volatile unsigned char	BCDMIN;
	volatile unsigned char	res12[3];
	volatile unsigned char	BCDHOUR;
	volatile unsigned char	res13[3];
	volatile unsigned char	BCDDATE;
	volatile unsigned char	res14[3];
	volatile unsigned char	BCDDAY;
	volatile unsigned char	res15[3];
	volatile unsigned char	BCDMON;
	volatile unsigned char	res16[3];
	volatile unsigned char	BCDYEAR;
	volatile unsigned char	res17[3];
#endif
};


/* ADC (see manual chapter 16) */
struct s3c2440_adc {
	volatile unsigned int	ADCCON;
	volatile unsigned int	ADCTSC;
	volatile unsigned int	ADCDLY;
	volatile unsigned int	ADCDAT0;
	volatile unsigned int	ADCDAT1;
	volatile unsigned int ADCUPDN;
};


/* SPI (see manual chapter 22) */
struct s3c2440_spi_channel {
	volatile unsigned char	SPCON;
	volatile unsigned char	res1[3];
	volatile unsigned char	SPSTA;
	volatile unsigned char	res2[3];
	volatile unsigned char	SPPIN;
	volatile unsigned char	res3[3];
	volatile unsigned char	SPPRE;
	volatile unsigned char	res4[3];
	volatile unsigned char	SPTDAT;
	volatile unsigned char	res5[3];
	volatile unsigned char	SPRDAT;
	volatile unsigned char	res6[3];
	volatile unsigned char	res7[16];
};

struct s3c2440_spi {
	struct s3c2440_spi_channel	ch[S3C2440_SPI_CHANNELS];
};


struct s3c2440_sdi {
	volatile unsigned int	SDICON;
	volatile unsigned int	SDIPRE;
	volatile unsigned int	SDICARG;
	volatile unsigned int	SDICCON;
	volatile unsigned int	SDICSTA;
	volatile unsigned int	SDIRSP0;
	volatile unsigned int	SDIRSP1;
	volatile unsigned int	SDIRSP2;
	volatile unsigned int	SDIRSP3;
	volatile unsigned int	SDIDTIMER;
	volatile unsigned int	SDIBSIZE;
	volatile unsigned int	SDIDCON;
	volatile unsigned int	SDIDCNT;
	volatile unsigned int	SDIDSTA;
	volatile unsigned int	SDIFSTA;
#ifdef __BIG_ENDIAN
	volatile unsigned char	res[3];
	volatile unsigned char	SDIDAT;
#else
	volatile unsigned char	SDIDAT;
	volatile unsigned char	res[3];
#endif
	volatile unsigned int	SDIIMSK;
};



/* S3C2440 only supports 512 Byte HW ECC */
#define S3C2440_ECCSIZE		512
#define S3C2440_ECCBYTES	3

enum s3c2440_uarts_nr {
	S3C2440_UART0 = 0,
	S3C2440_UART1 = 1,
	S3C2440_UART2 = 2
};

/* S3C2440 device base addresses */
#define S3C2440_MEMCTL_BASE			0x48000000
#define S3C2440_USB_HOST_BASE		0x49000000
#define S3C2440_INTERRUPT_BASE		0x4A000000
#define S3C2440_DMA_BASE			0x4B000000
#define S3C2440_CLOCK_POWER_BASE	0x4C000000
#define S3C2440_LCD_BASE			0x4D000000
#define S3C2440_NAND_BASE			0x4E000000
#define S3C2440_UART_BASE			0x50000000
#define S3C2440_TIMER_BASE			0x51000000
#define S3C2440_USB_DEVICE_BASE		0x52000140
#define S3C2440_WATCHDOG_BASE		0x53000000
#define S3C2440_I2C_BASE			0x54000000
#define S3C2440_I2S_BASE			0x55000000
#define S3C2440_GPIO_BASE			0x56000000
#define S3C2440_RTC_BASE			0x57000000
#define S3C2440_ADC_BASE			0x58000000
#define S3C2440_SPI_BASE			0x59000000
#define S3C2440_SDI_BASE			0x5A000000


static inline struct s3c2440_memctl *s3c2440_get_base_memctl(void)
{
	return (struct s3c2440_memctl *)S3C2440_MEMCTL_BASE;
}

static inline struct s3c2440_usb_host *s3c2440_get_base_usb_host(void)
{
	return (struct s3c2440_usb_host *)S3C2440_USB_HOST_BASE;
}

static inline struct s3c2440_interrupt *s3c2440_get_base_interrupt(void)
{
	return (struct s3c2440_interrupt *)S3C2440_INTERRUPT_BASE;
}

static inline struct s3c2440_dmas *s3c2440_get_base_dmas(void)
{
	return (struct s3c2440_dmas *)S3C2440_DMA_BASE;
}

static inline struct s3c2440_clock_power *s3c2440_get_base_clock_power(void)
{
	return (struct s3c2440_clock_power *)S3C2440_CLOCK_POWER_BASE;
}

static inline struct s3c2440_lcd *s3c2440_get_base_lcd(void)
{
	return (struct s3c2440_lcd *)S3C2440_LCD_BASE;
}

static inline struct s3c2440_nand *s3c2440_get_base_nand(void)
{
	return (struct s3c2440_nand *)S3C2440_NAND_BASE;
}

static inline struct s3c2440_uart *s3c2440_get_base_uart(enum s3c2440_uarts_nr n)
{
	return (struct s3c2440_uart *)(S3C2440_UART_BASE + (n * 0x4000));
}

static inline struct s3c2440_timers *s3c2440_get_base_timers(void)
{
	return (struct s3c2440_timers *)S3C2440_TIMER_BASE;
}

static inline struct s3c2440_usb_device *s3c2440_get_base_usb_device(void)
{
	return (struct s3c2440_usb_device *)S3C2440_USB_DEVICE_BASE;
}

static inline struct s3c2440_watchdog *s3c2440_get_base_watchdog(void)
{
	return (struct s3c2440_watchdog *)S3C2440_WATCHDOG_BASE;
}

static inline struct s3c2440_i2c *s3c2440_get_base_i2c(void)
{
	return (struct s3c2440_i2c *)S3C2440_I2C_BASE;
}

static inline struct s3c2440_i2s *s3c2440_get_base_i2s(void)
{
	return (struct s3c2440_i2s *)S3C2440_I2S_BASE;
}

static inline struct s3c2440_gpio *s3c2440_get_base_gpio(void)
{
	return (struct s3c2440_gpio *)S3C2440_GPIO_BASE;
}

static inline struct s3c2440_rtc *s3c2440_get_base_rtc(void)
{
	return (struct s3c2440_rtc *)S3C2440_RTC_BASE;
}

static inline struct s3c2440_adc *s3c2440_get_base_adc(void)
{
	return (struct s3c2440_adc *)S3C2440_ADC_BASE;
}

static inline struct s3c2440_spi *s3c2440_get_base_spi(void)
{
	return (struct s3c2440_spi *)S3C2440_SPI_BASE;
}

static inline struct s3c2440_sdi *s3c2440_get_base_sdi(void)
{
	return (struct s3c2440_sdi *)S3C2440_SDI_BASE;
}

#endif /*__S3C2440_H__*/
