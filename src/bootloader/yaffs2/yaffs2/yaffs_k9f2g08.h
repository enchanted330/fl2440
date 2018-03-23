#ifndef __YAFFS_K9F2G08_H__
#define __YAFFS_K9F2G08_H__

#include "yaffs_guts.h"

int ynf_init(struct yaffs_dev *dev);
int ynf_mark_block_bad(struct yaffs_dev *dev,int blockNumber);
int ynf_query_block(struct yaffs_dev *dev, int block_no, enum yaffs_block_state *state, u32 *seq_number);
int ynf_write_chunk_tags(struct yaffs_dev *dev,int nand_chunk,const u8 *data, const struct yaffs_ext_tags *tags);
int ynf_read_chunk_tags(struct yaffs_dev *dev,int nand_chunk, u8 *data, struct yaffs_ext_tags *tags);
int ynf_erase_block(struct yaffs_dev *dev, int block_num);

#endif /* __YAFFS_K9F2G08_H__  */
