#ifndef __K9F2G08_S3C_H__
#define __K9F2G08_S3C_H__

#include "s3c2440.h"

static inline void bsp_init_nand(void)
{
	unsigned int  reg;

	struct s3c2440_clock_power *clkpwr = s3c2440_get_base_clock_power();
 	clkpwr->CLKCON |= (1<<4);

	struct s3c2440_gpio *gpio = s3c2440_get_base_gpio();
	struct s3c2440_nand *nand = s3c2440_get_base_nand();

	reg = gpio->GPACON;
	reg |= (1<<22) | (1<<20) | (1<<19) | (1<<18) | (1<<17);
	gpio->GPACON = reg;

	nand->NFCONF = (1<<12) | (1<<8) | (1<<4) | (1<<3) |(1<<2) | (1<<5) | (0<<0);
	nand->NFCONT = 3;
}

static inline void bsp_nand_set_cs(int level)
{
	struct s3c2440_nand *nand = s3c2440_get_base_nand();
	if (level == 0) {
		nand->NFCONT &= ~(1<<1);
//		while (nand->NFSTAT & (1<<1));
	} else {
		nand->NFCONT |= 1<<1;
//		while ((nand->NFSTAT & (1<<1)) == 0);
	}
}

static inline void bsp_nand_start_main_ecc(void)
{
	struct s3c2440_nand *nand = s3c2440_get_base_nand();
	nand->NFCONT |= (1<<6) | (1<<5);  // unlock all
	nand->NFCONT |= (1<<4);  // init ecc
	nand->NFCONT &= ~(1<<6);  // lock spare
}

static inline void bsp_nand_stop_main_ecc(void)
{
	struct s3c2440_nand *nand = s3c2440_get_base_nand();
	nand->NFCONT |= (1<<5);  // lock main
}

static inline void bsp_nand_start_spare_ecc(void)
{
	struct s3c2440_nand *nand = s3c2440_get_base_nand();
	nand->NFCONT &= ~(1<<6); // unlock spare
}

static inline void bsp_nand_stop_spare_ecc(void)
{
	struct s3c2440_nand *nand = s3c2440_get_base_nand();
	nand->NFCONT |= (1<<6); // lock spare
}

static inline int bsp_nand_spare_check_ecc(unsigned short ecc, int *err_byte, int *err_bit)
{
	struct s3c2440_nand *nand = s3c2440_get_base_nand();
	unsigned int ecc_wr = ((ecc & 0xFF00) << 8) | (ecc & 0x00FF);

	nand->NFSECCD = ecc_wr;
	int err = nand->NFESTAT0;

	if ((err & (3<<2)) == (0<<2))  // no err
		return 0;


	if ((err & (3<<2)) == (1<<2)) {  // 1 bit err
		*err_byte = (err >> 21) & 0x0F;
		*err_bit = (err >> 18) & 0x07;
		return 1;
	}

	return -1;

}

static inline int bsp_nand_main_check_ecc(unsigned int ecc, int *err_byte, int *err_bit)
{
	struct s3c2440_nand *nand = s3c2440_get_base_nand();

	unsigned int ecc_wr = ((ecc & 0xFF00) << 8) | (ecc & 0x00FF);
	nand->NFMECCD0 = ecc_wr;

	ecc_wr = ((ecc & 0xFF000000) >> 8) | ((ecc & 0x00FF0000) >> 16);
	nand->NFMECCD1 = ecc_wr;

	int err = nand->NFESTAT0;

	if ((err & (3<<0)) == (0<<0))  // no err
		return 0;


	if ((err & (3<<0)) == (1<<0)) {  // no err
		*err_byte = (err >> 7) & 0x7FF;
		*err_bit = (err >> 4) & 0x07;
		return 1;
	}

	return -1;
}


static inline void bsp_nand_write_cmd(unsigned char cmd)
{
	struct s3c2440_nand *nand = s3c2440_get_base_nand();
	nand->NFCMD = cmd;
}


static inline void bsp_nand_write_addr(unsigned char addr)
{
	struct s3c2440_nand *nand = s3c2440_get_base_nand();
	nand->NFADDR = addr;
}

static inline void bsp_nand_write_data32(unsigned int dat)
{
	*(volatile unsigned int *)(&(s3c2440_get_base_nand()->NFDATA)) = dat;
}


static inline void bsp_nand_write_data16(unsigned short dat)
{
	*(volatile unsigned short *)(&(s3c2440_get_base_nand()->NFDATA)) = dat;
}

static inline void bsp_nand_write_data8(unsigned char dat)
{
	*(volatile unsigned char *)(&(s3c2440_get_base_nand()->NFDATA)) = dat;
}


static inline unsigned int bsp_nand_read_data32(void)
{
	return *(volatile unsigned int *)(&(s3c2440_get_base_nand()->NFDATA));
}


static inline unsigned short bsp_nand_read_data16(void)
{
	return *(volatile unsigned short *)(&(s3c2440_get_base_nand()->NFDATA));
}

static inline unsigned char bsp_nand_read_data8(void)
{
	return *(volatile unsigned char *)(&(s3c2440_get_base_nand()->NFDATA));
}

static inline unsigned short bsp_nand_read_spare_ecc0(void)
{
	struct s3c2440_nand *nand = s3c2440_get_base_nand();
	return nand->NFSECC & 0xFFFF;
}

static inline unsigned int bsp_nand_read_main_ecc0(void)
{
	struct s3c2440_nand *nand = s3c2440_get_base_nand();
	return nand->NFMECC0;
}


static inline void bsp_nand_wait_busy(void)
{
	int i;
	for (i=10; i>0; i--);
	struct s3c2440_nand *nand = s3c2440_get_base_nand();
	while ((nand->NFSTAT & (1<<0)) == 0);
}

#endif //__K9F2G08_S3C_H__
