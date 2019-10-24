#include <stdio.h>
#include <stdlib.h>
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

void add_hash(struct buf_header *h, int blkno, unsigned int stat, char *cache_data)
{
    // param h: header buffer
    // param blkno : block number
    // param stat : status
    // param cache_data : cache data
    struct buf_header *tmp;
    tmp->blkno = blkno;
    tmp->hash_fp = NULL;
    tmp->hash_bp = NULL;
    tmp->stat = stat;
    tmp->free_fp = NULL;
    tmp->free_bp = NULL;
    tmp->cache_data = cache_data;
    insert_hash(h, tmp, 0);
}

void print_hash(struct buf_header *h)
{
    for (; h->hash_fp != h; h = h->hash_fp)
    {
        printf(" [ %d %s %d]", h->blkno, h->cache_data, h->stat);
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
