/*
|  ECC MAIN  | BAD  | RES  |       YAFFS_TAG       |
|     4      |  4   |  8   |          48           |
*/

#include "k9f2g08_s3c.h"

/* From yaffs_guts.h */
#define YAFFS_OK    1
#define YAFFS_FAIL  0


int k9f2g08_init(void)
{
	return YAFFS_OK;
}

int k9f2g08_deinit(void)
{
	return YAFFS_OK;
}

static unsigned char wait_busy_status(void)
{
	unsigned char rc;

	for (rc=0; rc <40; rc++);
	bsp_nand_set_cs(0);
	bsp_nand_write_cmd(0x70);  // read status
	do {
		rc = bsp_nand_read_data8();
	} while ((rc & (1<<6)) == 0);
	bsp_nand_set_cs(1);

	return rc;
}

int k9f2g08_mark_block_bad(unsigned int blk_num)
{
	int row_addr = blk_num * 64;
	int col_addr = 2048;

	unsigned char status;

	bsp_nand_set_cs(0);
	bsp_nand_write_cmd(0x80);  // write command
	bsp_nand_write_addr((unsigned char)(col_addr >> 0));
	bsp_nand_write_addr((unsigned char)(col_addr >> 8));
	bsp_nand_write_addr((unsigned char)(row_addr >> 0));
	bsp_nand_write_addr((unsigned char)(row_addr >> 8));
	bsp_nand_write_addr((unsigned char)(row_addr >> 16));

	bsp_nand_write_data32(0xFFFFFFFF);
	bsp_nand_write_data32(0x00000000);
	bsp_nand_write_data32(0xFFFFFFFF);
	bsp_nand_write_data32(0xFFFFFFFF);
	bsp_nand_write_cmd(0x10);
	bsp_nand_set_cs(1);

	status = wait_busy_status();

	return (status & (1<<0)) ? YAFFS_FAIL : YAFFS_OK;
}

int k9f2g08_write(unsigned int chunk, const unsigned char *dat, const unsigned char *spare, unsigned int spare_size)
{
	int i;
	int col_addr;
	unsigned char status;

	if (dat) {
		col_addr = 0;
	} else {
		col_addr = 2048;
	}


	bsp_nand_set_cs(0);
	bsp_nand_write_cmd(0x80);  // write command
	bsp_nand_write_addr((unsigned char)(col_addr >> 0));
	bsp_nand_write_addr((unsigned char)(col_addr >> 8));
	bsp_nand_write_addr((unsigned char)(chunk >> 0));
	bsp_nand_write_addr((unsigned char)(chunk >> 8));
	bsp_nand_write_addr((unsigned char)(chunk >> 16));

	bsp_nand_start_main_ecc();
	if (dat) {
		for (i=0; i<2048; i++)
			bsp_nand_write_data8(*dat++);
	}

	bsp_nand_stop_main_ecc();

	bsp_nand_write_data32(bsp_nand_read_spare_ecc0());
	bsp_nand_write_data32(0xFFFFFFFF);
	bsp_nand_write_data32(0xFFFFFFFF);
	bsp_nand_write_data32(0xFFFFFFFF);

	if (spare) {
		for (i=0; i<spare_size; i++)
			bsp_nand_write_data8(*spare++);
	}

	bsp_nand_write_cmd(0x10);
	bsp_nand_set_cs(1);

	status = wait_busy_status();

	return (status & (1<<0)) ? YAFFS_FAIL : YAFFS_OK;
}

int k9f2g08_read(unsigned int chunk, unsigned char *dat, unsigned char *spare, unsigned int spare_size, int *eccStatus)
{
	int i;
	int col_addr;
	unsigned char *p = dat;
	unsigned int ecc = 0;
	int err_bit;
	int err_byte;

	if (dat) {
		col_addr = 0;
	} else {
		col_addr = 2048 + 16;
	}


	bsp_nand_set_cs(0);
	bsp_nand_write_cmd(0x00);  // read command
	bsp_nand_write_addr((unsigned char)(col_addr >> 0));
	bsp_nand_write_addr((unsigned char)(col_addr >> 8));
	bsp_nand_write_addr((unsigned char)(chunk >> 0));
	bsp_nand_write_addr((unsigned char)(chunk >> 8));
	bsp_nand_write_addr((unsigned char)(chunk >> 16));
	bsp_nand_write_cmd(0x30);  // read
	bsp_nand_wait_busy();

	if (dat) {
		bsp_nand_start_main_ecc();
		for (i=0; i<2048; i++)
			*p++ = bsp_nand_read_data8();

		bsp_nand_stop_main_ecc();

		ecc = bsp_nand_read_data32();

		bsp_nand_read_data32();
		bsp_nand_read_data32();
		bsp_nand_read_data32();
	}
	if (spare) {
		p = spare;
		for (i=0; i<spare_size; i++)
			*p++ = bsp_nand_read_data8();
	}

	bsp_nand_set_cs(1);

	if (dat) {
		i = bsp_nand_main_check_ecc(ecc, &err_byte, &err_bit);
		if (i == 0) {
			*eccStatus = 0;
			return 0;
		}

		if (i < 0) {
			*eccStatus = -1;
			return 0;
		}

		*eccStatus = 1;
		dat[err_byte] ^= (1<<err_bit);
	}

	return YAFFS_OK;
}

int k9f2g08_erase_block(unsigned int blk_num)
{
	unsigned char status;
	int chunk = blk_num * 64;

	bsp_nand_set_cs(0);
	bsp_nand_write_cmd(0x60);  // erase command
	bsp_nand_write_addr((unsigned char)(chunk >> 0));
	bsp_nand_write_addr((unsigned char)(chunk >> 8));
	bsp_nand_write_addr((unsigned char)(chunk >> 16));
	bsp_nand_write_cmd(0xD0);  // erase
	bsp_nand_set_cs(1);

	status = wait_busy_status();
	return (status & (1<<0)) ? YAFFS_FAIL : YAFFS_OK;
}

int k9f2g08_is_block_ok(unsigned int blk_num)
{
	int chunk = blk_num * 64;
	int col_addr = 2048 + 4;

	bsp_nand_set_cs(0);
	bsp_nand_write_cmd(0x00);  // read command
	bsp_nand_write_addr((unsigned char)(col_addr >> 0));
	bsp_nand_write_addr((unsigned char)(col_addr >> 8));
	bsp_nand_write_addr((unsigned char)(chunk >> 0));
	bsp_nand_write_addr((unsigned char)(chunk >> 8));
	bsp_nand_write_addr((unsigned char)(chunk >> 16));
	bsp_nand_write_cmd(0x30);  // read
	bsp_nand_wait_busy();

	int tmp = bsp_nand_read_data32();
	bsp_nand_set_cs(1);

	if (tmp == 0xFFFFFFFF) {
		return 1;
	}
	return 0;
}


int k9f2g08_read_id(unsigned int *id)
{
	int rc = -1;
	bsp_nand_set_cs(0);
	bsp_nand_write_cmd(0x90);  // read command
	bsp_nand_write_addr((unsigned char)0);
	do {
		if ((rc = bsp_nand_read_data8()) != 0xEC)
			break;
		*id = bsp_nand_read_data32();
		rc = 0;
	} while(0);

	bsp_nand_set_cs(1);

	return rc;
}

int k9f2g08_read_ll(unsigned int chunk, unsigned char *dat, unsigned char *spare)
{
	int i;
	int col_addr;
	unsigned char *p;

	if (dat) {
		col_addr = 0;
	} else {
		col_addr = 2048;
	}


	bsp_nand_set_cs(0);
	bsp_nand_write_cmd(0x00);  // read command
	bsp_nand_write_addr((unsigned char)(col_addr >> 0));
	bsp_nand_write_addr((unsigned char)(col_addr >> 8));
	bsp_nand_write_addr((unsigned char)(chunk >> 0));
	bsp_nand_write_addr((unsigned char)(chunk >> 8));
	bsp_nand_write_addr((unsigned char)(chunk >> 16));
	bsp_nand_write_cmd(0x30);  // read

	bsp_nand_wait_busy();

	if (dat) {
		p = dat;
		for (i=0; i<2048; i++)
			*p++ = bsp_nand_read_data8();
	}
	if (spare) {
		p = spare;
		for (i=0; i<64; i++)
			*p++ = bsp_nand_read_data8();
	}

	bsp_nand_set_cs(1);

	return 1;
}
