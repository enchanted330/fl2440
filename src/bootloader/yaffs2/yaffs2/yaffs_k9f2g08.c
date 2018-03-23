
#include "yaffs_k9f2g08.h"
#include "k9f2g08_api.h"
#include "yaffs_packedtags2.h"
#include "yaffs_trace.h"

int ynf_init(struct yaffs_dev *dev)
{
	yaffs_trace(YAFFS_TRACE_MTD, "ynf_init on %s", dev->param.name);
	return YAFFS_OK;
}

int ynf_write_chunk_tags(struct yaffs_dev *dev, int nand_chunk, const u8 *data, const struct yaffs_ext_tags *tags)
{
	struct yaffs_packed_tags2 pt;
	void *spare;
	unsigned spareSize = 0;

	yaffs_trace(YAFFS_TRACE_MTD, "ynf_write_chunk_tags chunk %d data %p tags %p", nand_chunk, data, tags);

	/* For yaffs2 writing there must be both data and tags.
	 * If we're using inband tags, then the tags are stuffed into
	 * the end of the data buffer.
	 */
	if(dev->param.inband_tags){
		struct yaffs_packed_tags2_tags_only *pt2tp;
		pt2tp = (struct yaffs_packed_tags2_tags_only *)
			(data + dev->data_bytes_per_chunk);
		yaffs_pack_tags2_tags_only(pt2tp,tags);
		spare = NULL;
		spareSize = 0;
	}
	else{
		yaffs_pack_tags2(&pt, tags,!dev->param.no_tags_ecc);
		spare = &pt;
		spareSize = sizeof(struct yaffs_packed_tags2);
	}

	return k9f2g08_write(nand_chunk, data, spare, spareSize);
}

int ynf_read_chunk_tags(struct yaffs_dev *dev, int nand_chunk, u8 *data, struct yaffs_ext_tags *tags)
{
	struct yaffs_packed_tags2 pt;
	int localData = 0;
	void *spare = NULL;
	unsigned spareSize;
	int retval = 0;
	int eccStatus; //0 = ok, 1 = fixed, -1 = unfixed

	yaffs_trace(YAFFS_TRACE_MTD, "ynf_read_chunk_tags chunk %d data %p tags %p", nand_chunk, data, tags);

	if(!tags){
		spare = NULL;
		spareSize = 0;
	}else if(dev->param.inband_tags){

		if(!data) {
			localData = 1;
			data = yaffs_get_temp_buffer(dev);
		}
		spare = NULL;
		spareSize = 0;
	}
	else {
		spare = &pt;
		spareSize = sizeof(struct yaffs_packed_tags2);
	}

	retval = k9f2g08_read(nand_chunk, data, spare, spareSize, &eccStatus);

	if(dev->param.inband_tags){
		if(tags){
			struct yaffs_packed_tags2_tags_only * pt2tp;
			pt2tp = (struct yaffs_packed_tags2_tags_only *)&data[dev->data_bytes_per_chunk];
			yaffs_unpack_tags2_tags_only(tags,pt2tp);
		}
	}
	else {
		if (tags){
			yaffs_unpack_tags2(tags, &pt,!dev->param.no_tags_ecc);
		}
	}


	if(tags && tags->chunk_used){
		if(eccStatus < 0 ||
		   tags->ecc_result == YAFFS_ECC_RESULT_UNFIXED)
			tags->ecc_result = YAFFS_ECC_RESULT_UNFIXED;
		else if(eccStatus > 0 ||
			     tags->ecc_result == YAFFS_ECC_RESULT_FIXED)
			tags->ecc_result = YAFFS_ECC_RESULT_FIXED;
		else
			tags->ecc_result = YAFFS_ECC_RESULT_NO_ERROR;
	}

	if(localData)
		yaffs_release_temp_buffer(dev, data);

	return retval;
}

int ynf_mark_block_bad(struct yaffs_dev *dev, int blockId)
{
	yaffs_trace(YAFFS_TRACE_MTD, "ynf_mark_block_bad block@%d ", blockId);
	return k9f2g08_mark_block_bad(blockId);
}

int ynf_erase_block(struct yaffs_dev *dev, int blockId)
{
	yaffs_trace(YAFFS_TRACE_MTD, "ynf_erase_block   block@%d ", blockId);
	return k9f2g08_erase_block(blockId);
}


int ynf_query_block(struct yaffs_dev *dev, int blockId, enum yaffs_block_state *state, u32 *seq_number)
{
	unsigned chunkNo;
	struct yaffs_ext_tags tags;

	yaffs_trace(YAFFS_TRACE_MTD, "ynf_query_block   block@%d ", blockId);
	*seq_number = 0;

	chunkNo = blockId * dev->param.chunks_per_block;

	if (!k9f2g08_is_block_ok(blockId)) {
		*state = YAFFS_BLOCK_STATE_DEAD;
		return YAFFS_OK;
	}

	ynf_read_chunk_tags(dev, chunkNo, NULL, &tags);
	if(!tags.chunk_used)
	{
		*state = YAFFS_BLOCK_STATE_EMPTY;
	}
	else
	{
		*state = YAFFS_BLOCK_STATE_NEEDS_SCAN;
		*seq_number = tags.seq_number;
	}
	return YAFFS_OK;
}


