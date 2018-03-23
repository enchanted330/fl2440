/*
 * YAFFS: Yet Another Flash File System. A NAND-flash specific file system.
 *
 * Copyright (C) 2002-2011 Aleph One Ltd.
 *   for Toby Churchill Ltd and Brightstar Engineering
 *
 * Created by Charles Manning <charles@aleph1.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/*
 * yaffscfg2k.c  The configuration for the "direct" use of yaffs.
 *
 * This file is intended to be modified to your requirements.
 * There is no need to redistribute this file.
 */

#include "yaffscfg.h"
#include "yaffs_guts.h"
#include "yaffsfs.h"
#include "yaffs_trace.h"
#include "yaffs_osglue.h"
#include "yaffs_k9f2g08.h"

//#include <asm/errno.h>
//#include <errno.h>

#if 1
unsigned yaffs_trace_mask =

//	YAFFS_TRACE_SCAN |
	YAFFS_TRACE_GC |
//    YAFFS_TRACE_MTD |
	YAFFS_TRACE_ERASE |
	YAFFS_TRACE_ERROR |
//	YAFFS_TRACE_TRACING |
//	YAFFS_TRACE_ALLOCATE |
	YAFFS_TRACE_BAD_BLOCKS |
//	YAFFS_TRACE_VERIFY |
//    YAFFS_TRACE_ALWAYS |
	0;
#else
unsigned yaffs_trace_mask = 0xFFFFFFFF;
#endif


int yaffs_devconfig(char *_mp, int start_block, int end_block)
{
    struct yaffs_dev *dev = NULL;
    char *mp = NULL;

    dev = malloc(sizeof(*dev));
    mp = strdup(_mp);

    if (!dev || !mp) 
    {
        /*  Alloc error */
        printf("Failed to allocate memory\n");
        return -1;
    }

    /*  Seems sane, so configure */
    memset(dev, 0, sizeof(*dev));
    dev->param.name = mp;
    dev->param.is_yaffs2 = 1;

    dev->param.total_bytes_per_chunk = NF_PAGE_SIZE;
    dev->param.spare_bytes_per_chunk = NF_SPARE_SIZE;
    dev->param.chunks_per_block = NF_BLOCK_SIZE / NF_PAGE_SIZE;
    dev->param.start_block = start_block;
    dev->param.end_block = end_block;
    dev->param.n_reserved_blocks = 8;

    dev->param.inband_tags = 0;
    dev->param.use_nand_ecc = 0;
    dev->param.no_tags_ecc = 0;
    dev->param.n_caches=0;
    dev->param.empty_lost_n_found = 1;
    dev->param.skip_checkpt_rd = 0;
    dev->param.skip_checkpt_wr = 0;
    dev->param.refresh_period = 1000;

    dev->param.initialise_flash_fn = ynf_init;
    dev->param.erase_fn = ynf_erase_block;
    dev->param.write_chunk_tags_fn = ynf_write_chunk_tags;
    dev->param.read_chunk_tags_fn = ynf_read_chunk_tags;
    dev->param.bad_block_fn = ynf_mark_block_bad;
    dev->param.query_block_fn = ynf_query_block;
    dev->driver_context = NULL;

    yaffs_add_device(dev);

    printf("Configures yaffs mount %s: start block %d, end block %d %s\n", 
            mp, start_block, end_block, dev->param.inband_tags ? "using inband tags" : "");
    return 0;
}


/* Configure the devices that will be used */

int yaffs_start_up(void)
{
	static int start_up_called = 0;

	if(start_up_called)
		return 0;
	start_up_called = 1;

    yaffs_devconfig(YAFFSFS_MNT_POINT, YAFFSFS_START_BLOCK, YAFFSFS_END_BLOCK);

	/* Call the OS initialisation (eg. set up lock semaphore */
	yaffsfs_OSInitialisation();

	return 0;
}



