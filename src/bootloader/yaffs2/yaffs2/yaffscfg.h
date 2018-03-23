/*
 * YAFFS: Yet another Flash File System . A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2011 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 2.1 as
 * published by the Free Software Foundation.
 *
 * Note: Only YAFFS headers are LGPL, YAFFS C code is covered by GPL.
 */

/*
 * Header file for using yaffs in an application via
 * a direct interface.
 */


#ifndef __YAFFSCFG_H__
#define __YAFFSCFG_H__


#include "yportenv.h"

#define YAFFSFS_N_HANDLES	           100
#define YAFFSFS_N_DSC	               20
#define YAFFSFS_MNT_POINT              "/nand"

#define SIZE_1M                        0x100000
#define YAFFSFS_OFFSET                 (2*SIZE_1M)  /* YAFFS2 file system start offset address */
#define YAFFSFS_SIZE                   (10*SIZE_1M) /* YAFFS2 file system start offset address */

#define K9F2G08_PAGE_SIZE              2048         /* Nandflash pagesize:  2K */
#define K9F2G08_BLOCK_SIZE             0x20000      /* Nandflash block size: 128K  */
#define K9F2G08_SPARE_SIZE             64           /* Nandflash spare area size:  64 */

#define NF_PAGE_SIZE                   K9F2G08_PAGE_SIZE 
#define NF_BLOCK_SIZE                  K9F2G08_BLOCK_SIZE
#define NF_SPARE_SIZE                  K9F2G08_SPARE_SIZE

#define YAFFSFS_START_BLOCK            ( YAFFSFS_OFFSET/NF_BLOCK_SIZE )
#define YAFFSFS_END_BLOCK              ( YAFFSFS_START_BLOCK+(YAFFSFS_SIZE/NF_BLOCK_SIZE) )


struct yaffsfs_DeviceConfiguration {
	const YCHAR *prefix;
	struct yaffs_dev *dev;
};


#endif

