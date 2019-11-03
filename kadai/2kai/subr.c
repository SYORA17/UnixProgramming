#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buf.h"

int hash(int num)
{
    return num % NHASH;
}

struct buf_header *hash_search(int blkno)
{
    int h;
    struct buf_header *p;

    h = hash(blkno);
    for (p = hash_head[h].hash_fp; p != &hash_head[h]; p = p->hash_fp)
    {
        if (p->blkno == blkno)
            return p;
    }
    return NULL;
}

void insert_head(struct buf_header *h, struct buf_header *p)
{
    // param h: head pointer
    // param p: pointer that will be inserted
    p->hash_bp = h;
    p->hash_fp = h->hash_fp;
    h->hash_fp->hash_bp = p;
    h->hash_fp = p;
}

void insert_tail(struct buf_header *h, struct buf_header *p)
{
    // param h: head pointer
    // param p: pointer that will be inserted
    p->hash_bp = h->hash_bp;
    p->hash_fp = h;
    h->hash_bp->hash_fp = p;
    h->hash_bp = p;
}

void insert_hash(struct buf_header *h, struct buf_header *p, int where)
{
    // param h: head pointer
    // param p: pointer that will be inserted
    // param where: 0 represents insert at tail, others represent head.
    if (where != 0)
        insert_head(h, p);
    else
        insert_tail(h, p);
}

void remove_hash(struct buf_header *p)
{
    // param p: pointer that will be removed.
    p->hash_bp->hash_fp = p->hash_fp;
    p->hash_fp->hash_bp = p->hash_bp;
    buf_free(p);
}

void init_head_hash(struct buf_header *h, int blkno, unsigned int stat) {
    h->bufno = -1;
    h->blkno = blkno;
    h->hash_fp = h;
    h->hash_bp = h;
    h->stat = stat;
    h->free_fp = NULL;
    h->free_bp = NULL;
    h->cache_data = NULL;
}

void init_free_hash(struct buf_header *h, int blkno, unsigned int stat) {
    h->bufno = -1;
    h->blkno = blkno;
    h->hash_fp = NULL;
    h->hash_bp = NULL;
    h->stat = stat;
    h->free_fp = h;
    h->free_bp = h;
    h->cache_data = NULL;
}

void add_hash(struct buf_header *h, struct buf_header *tmp, int bufno,int blkno, unsigned int stat, char *cache_data)
{
    // param h: header buffer
    // param tmp : buffer to insert
    // param bufnp : buffer number
    // param blkno : block number
    // param stat : status
    // param cache_data : cache data
    // struct buf_header *tmp;
    // tmp = malloc(sizeof(struct buf_header));
    tmp->bufno = bufno;
    tmp->blkno = blkno;
    tmp->hash_fp = NULL;
    tmp->hash_bp = NULL;
    tmp->stat = stat;
    tmp->free_fp = NULL;
    tmp->free_bp = NULL;
    tmp->cache_data = cache_data;
    insert_hash(h, tmp, 0);
    // printf("%d", h->hash_fp->blkno);
}

void add_free(struct buf_header *h, struct buf_header *p) {
    // insert p to free list's tail
    p->free_bp = h->free_bp;
    p->free_fp = h;
    h->free_bp->free_bp = p;
    h->free_bp = p;
}

void print_hash(struct buf_header *h, int idx)
{
    printf("%d: ", idx);
    struct buf_header *tmp = h;

    for (h = h->hash_fp; h != tmp; h = h->hash_fp)
    {
        char stat[7] = "------\0";
        if (h->stat & STAT_LOCKED)
            stat[5] = 'L';
        if (h->stat & STAT_VALID)
            stat[4] = 'V';
        if (h->stat & STAT_DWR)
            stat[3] = 'D';
        if (h->stat & STAT_KRDWR)
            stat[2] = 'K';
        if (h->stat & STAT_WAITED)
            stat[1] = 'W';
        if (h->stat & STAT_OLD)
            stat[0] = 'O';
        printf("[%2d: %2d %s] ", h->bufno, h->blkno, stat);
    }
    printf("\n");
}

void print_buf(struct buf_header *h, int idx)
{
    int fp_cnt = idx % 3 + 1;
    for (int i = 0; i < fp_cnt; i++)
        h = h->hash_fp;
    char stat[7] = "------\0";
    if (h->stat & STAT_LOCKED)
        stat[5] = 'L';
    if (h->stat & STAT_VALID)
        stat[4] = 'V';
    if (h->stat & STAT_DWR)
        stat[3] = 'D';
    if (h->stat & STAT_KRDWR)
        stat[2] = 'K';
    if (h->stat & STAT_WAITED)
        stat[1] = 'W';
    if (h->stat & STAT_OLD)
        stat[0] = 'O';

    printf("[%2d: %2d %s]\n", h->bufno, h->blkno, stat);
    return;
}

void print_free(struct buf_header *h) {
    struct buf_header *tmp = h;
    // int cnt = 0;
    for (h = h->free_fp; h != tmp; h = h->free_fp)
    {
        char stat[7] = "------\0";
        if (h->stat & STAT_LOCKED)
            stat[5] = 'L';
        if (h->stat & STAT_VALID)
            stat[4] = 'V';
        if (h->stat & STAT_DWR)
            stat[3] = 'D';
        if (h->stat & STAT_KRDWR)
            stat[2] = 'K';
        if (h->stat & STAT_WAITED)
            stat[1] = 'W';
        if (h->stat & STAT_OLD)
            stat[0] = 'O';
        printf("[%2d: %2d %s] ", h->bufno, h->blkno, stat);
        // if (cnt == 3 || cnt == 7)
        //     printf("\n");
        // cnt++;
    }
    printf("\n");
}

void buf_free(struct buf_header *p)
{
    free(&(p->blkno));
    free(p->cache_data);
    free(&(p->stat));
    free(p);
}
