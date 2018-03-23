/*
 * =====================================================================================
 *
 *       Filename:  bootstrap.h
 *        Version:  1.0.0
 *         Author:  Guo Wenxue<Email: guowenxue@ghlsystems.com QQ:281143292>
 *      CopyRight:  2011 (C) Guo Wenxue
 *    Description:  Some Reigster address definition for bootstrap.S
 * =====================================================================================
 */

#define S3C_WATCHDOG_BASE       0x53000000

#define S3C_INTERRUPT_BASE      0x4a000000
#define SRCPND_OFFSET           0x00
#define INTMOD_OFFSET           0x04
#define INTMSK_OFFSET           0x08
#define PRIORITY_OFFSET         0x0c
#define INTPND_OFFSET           0x10
#define INTOFFSET_OFFSET        0x14
#define SUBSRCPND_OFFSET        0x18
#define INTSUBMSK_OFFSET        0x1c

#define S3C_CLOCK_POWER_BASE    0x4c000000
#define LOCKTIME_OFFSET         0x00
#define MPLLCON_OFFSET          0x04
#define UPLLCON_OFFSET          0x08
#define CLKCON_OFFSET           0x0c
#define CLKSLOW_OFFSET          0x10
#define CLKDIVN_OFFSET          0x14
#define CAMDIVN_OFFSET          0x18

#define BWSCON  		        0x48000000

#define MDIV_405                0x7f << 12
#define PSDIV_405               0x21


#define GPBCON   0x56000010
#define GPBDAT   0x56000014
#define GPBUP    0x56000018

#define OUTPUT   0x01   /* Set GPIO port as output mode*/
#define INPUT    0x00   /* Set GPIO port as input mode*/

#define BEEP     0      /* On FL2440 board, LED0 use GPB0*/
#define LED0     5      /* On FL2440 board, LED0 use GPB5*/
#define LED1     6      /* On FL2440 board, LED0 use GPB6*/
#define LED2     8      /* On FL2440 board, LED0 use GPB8*/
#define LED3     10     /* On FL2440 board, LED0 use GPB10*/

/*  BWSCON */
#define DW8             (0x0)
#define DW16            (0x1)
#define DW32            (0x2)
#define WAIT            (0x1<<2)
#define UBLB            (0x1<<3)

#define B1_BWSCON       (DW16)
#define B2_BWSCON       (DW16)
#define B3_BWSCON       (DW16 + WAIT + UBLB)
#define B4_BWSCON       (DW16)
#define B5_BWSCON       (DW16)
#define B6_BWSCON       (DW32)
#define B7_BWSCON       (DW32)

#define B0_Tacs         0x0
#define B0_Tcos         0x0
#define B0_Tacc         0x7
#define B0_Tcoh         0x0
#define B0_Tah          0x0
#define B0_Tacp         0x0
#define B0_PMC          0x0

#define B1_Tacs         0x0
#define B1_Tcos         0x0
#define B1_Tacc         0x7
#define B1_Tcoh         0x0
#define B1_Tah          0x0
#define B1_Tacp         0x0
#define B1_PMC          0x0 

#define B2_Tacs         0x0
#define B2_Tcos         0x0
#define B2_Tacc         0x7
#define B2_Tcoh         0x0
#define B2_Tah          0x0
#define B2_Tacp         0x0
#define B2_PMC          0x0

#define B3_Tacs         0xc
#define B3_Tcos         0x7
#define B3_Tacc         0xf
#define B3_Tcoh         0x1
#define B3_Tah          0x0
#define B3_Tacp         0x0
#define B3_PMC          0x0

#define B4_Tacs         0x0
#define B4_Tcos         0x0
#define B4_Tacc         0x7
#define B4_Tcoh         0x0
#define B4_Tah          0x0
#define B4_Tacp         0x0
#define B4_PMC          0x0

#define B5_Tacs         0xc
#define B5_Tcos         0x7
#define B5_Tacc         0xf
#define B5_Tcoh         0x1
#define B5_Tah          0x0
#define B5_Tacp         0x0
#define B5_PMC          0x0

/* SDRAM is on HSB bus, so its clock is from HCLK, FCLK=400, HCLK=100; so SDRAM 1clk=10ns */ 

#define B6_MT           0x3 /*  SDRAM */
// K4S561632 datasheet: RAS to CAS delay(Trcd) Min value should be 18/20ns, HCLK is 100MHz, so 1clk=10ns
// EM63A165  datasheet: RAS# to CAS# delay(Trcd) Min value should be 15/20ns, HCLK is 100MHz, so 1clk=10ns
#define B6_Trcd         0x2 /* 4clk */
#define B6_SCAN         0x1 /* 9bit */

#define B7_MT           0x3 /* SDRAM */
#define B7_Trcd         0x1 /* 3clk */
#define B7_SCAN         0x1 /* 9bit */

/* REFRESH register<0x48000024> parameter */
#define REFEN           0x1 /*  Refresh enable */
#define TREFMD          0x0 /*  CBR(CAS before RAS)/Auto refresh */

//                 Trp: Row precharge time
// K4S561632 datasheet: Min(Trp) value should be 18/20ns;
// EM63A165 datasheet:  Min value should be 15/20ns;
#define Trp             0x2 /*  4clk */

//                Trc:  Row cycle time
// K4S561632 datasheet: Min value should be 60/65ns;
// EM63A165 datasheet:  Min value should be 60/63ns;
// S3C2440  datasheet:  REFRESH register describe: SDRAM Row cycle time: Trc=Tsrc+Trp
#define Tsrc            0x2 /* 6clk, so Trc=Tsrc+Trp=6+3=9clk */

// K4S561632 datasheet: 64ms refresh period (8K Cycle):  64000/8192=7.81us
// EM63A165  datasheet: 8192 refresh cycles/64ms:        64000/8192=7.81us
// S3C2440   datasheet: REFRESH Register Refresh period = (2^11-refresh_count+1)/HCLK
//                      So Refresh count = 2^11 + 1 - 100x7.81 = 1268
#define REFCNT          1268
//#define REFCNT          489 /* HCLK=100Mhz, (2048+1-15.6*100) */

