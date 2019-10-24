#ifndef _BUF_H_
#define _BUF_H_

struct buf_header
{
    int blkno;
    struct buf_header *hash_fp;
    struct buf_header *hash_bp;
    unsigned int stat;
    struct buf_header *free_fp;
    struct buf_header *free_bp;
    char *cache_data;
};

#define NHASH 4
/* init buffer*/
struct buf_header hash_head[NHASH];
struct buf_header freelist;

#define STAT_LOCKED 0x00000001
#define STAT_VALID 0x00000002
#define STAT_DWR 0x00000004
#define STAT_KRDWR 0x00000008
#define STAT_WAITED 0x00000010
#define STAT_OLD 0x00000020

struct buf_header *hash_search(int blkno);
void insert_head(struct buf_header *h, struct buf_header *p);
void insert_tail(struct buf_header *h, struct buf_header *p);
void insert_hash(struct buf_header *h, struct buf_header *p, int where);
void remove_hash(struct buf_header *p);
void add_hash(struct buf_header *h, int blkno, unsigned int stat, char *cache_data);
void print_hash(struct buf_header *h);
void buf_free(struct buf_header *p);

#endif
