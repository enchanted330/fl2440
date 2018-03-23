#ifndef __K9F2G08_API_H__
#define __K9F2G08_API_H__

int k9f2g08_init(void);
int k9f2g08_deinit(void);
int k9f2g08_mark_block_bad(unsigned int blk_num);
int k9f2g08_write(unsigned int chunk, const unsigned char *dat, const unsigned char *spare, unsigned int spare_size);
int k9f2g08_read(unsigned int chunk, unsigned char *dat, unsigned char *spare, unsigned int spare_size, int *eccStatus);
int k9f2g08_read_ll(unsigned int chunk, unsigned char *dat, unsigned char *spare);
int k9f2g08_erase_block(unsigned int blk_num);
int k9f2g08_is_block_ok(unsigned int blk_num);
int k9f2g08_read_id(unsigned int *id);

#endif  // __K9F2G08_API_H__
