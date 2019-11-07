#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buf.h"

int hash(int num)
{
    return num % NHASH;
}


struct buf_header *hash_search(int blkno) {
    // struct buf_header *tmp;
    int hash_no = hash(blkno);
    struct buf_header *h = &hash_head[hash_no];
    struct buf_header *tmp = h;
    for (h = h->hash_fp; h != tmp; h = h->hash_fp)
    {
        if (h->blkno == blkno) {
            return h;
        }
    }
    return NULL;
}

struct buf_header *hash_search_bufno(int bufno) {
    struct buf_header *h;
    struct buf_header *p;
    for (int i = 0; i < NHASH; i++) {
        h = &hash_head[i];
        for (p = h->hash_fp; p != h; p = p->hash_fp)
        {
            if (p->bufno == bufno) {
                return p;
            }
        }
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
    // buf_free(p);
}

void remove_free(struct buf_header *p)
{
    p->free_bp->free_fp = p->free_fp;
    p->free_fp->free_bp = p->free_bp;
}

struct buf_header *remove_free_top(struct buf_header *h)
{
    struct buf_header *p;
    p = h->free_fp;
    remove_free(p);
    return p;
}

int isBuffer_hashlist(struct buf_header *h, struct buf_header *p)
{
    // -1 :no , 1 :yes
    struct buf_header *tmp = h;
    for (tmp = tmp->hash_fp; tmp != h; tmp = tmp->hash_fp) {
        if (tmp == p)
            return 1;
    }
    return -1;
}

int isBuffer_freelist(struct buf_header *h)
{
    // -1 represents no buffer
    // 0 represents top buffer is STAT_DWR
    // 1 represetns top buffer is not STAT_DWR
    if (h->free_fp == h)
        return -1;
    if (h->free_fp->stat & STAT_DWR)
        return 0;
    return 1;
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
    p->stat = STAT_VALID;
    p->free_bp = h->free_bp;
    p->free_fp = h;
    h->free_bp->free_fp = p;
    h->free_bp = p;
}

void add_free_top(struct buf_header *h, struct buf_header *p) {
    // insert p to free list's top
    p->stat = STAT_VALID;
    p->free_bp = h;
    p->free_fp = h->free_fp;
    h->free_fp->free_bp = p;
    h->free_fp = p;
}

void print_hash(struct buf_header *h, int idx)
{
    printf("%d: ", idx);
    struct buf_header *tmp = h;
    tmp = h;
    for (tmp = tmp->hash_fp; tmp != h; tmp = tmp->hash_fp)
    {
        char stat[7] = "------\0";
        if (tmp->stat & STAT_LOCKED)
            stat[5] = 'L';
        if (tmp->stat & STAT_VALID)
            stat[4] = 'V';
        if (tmp->stat & STAT_DWR)
            stat[3] = 'D';
        if (tmp->stat & STAT_KRDWR)
            stat[2] = 'K';
        if (tmp->stat & STAT_WAITED)
            stat[1] = 'W';
        if (tmp->stat & STAT_OLD)
            stat[0] = 'O';
        printf("[%2d: %2d %s] ", tmp->bufno, tmp->blkno, stat);
    }
    printf("\n");
}

//print by blkno
// -1 : error, 1 : no error
int print_buf(int bufno)
{
    // int fp_cnt = idx % 3 + 1;
    // for (int i = 0; i < fp_cnt; i++)
    //     h = h->hash_fp;
    struct buf_header *p = hash_search_bufno(bufno);
    if (p == NULL) {
        fprintf(stderr, "coludn't find such a buffer.\n");
        return -1;
    }
    char stat[7] = "------\0";
    if (p->stat & STAT_LOCKED)
        stat[5] = 'L';
    if (p->stat & STAT_VALID)
        stat[4] = 'V';
    if (p->stat & STAT_DWR)
        stat[3] = 'D';
    if (p->stat & STAT_KRDWR)
        stat[2] = 'K';
    if (p->stat & STAT_WAITED)
        stat[1] = 'W';
    if (p->stat & STAT_OLD)
        stat[0] = 'O';

    printf("[%2d: %2d %s]\n", p->bufno, p->blkno, stat);
    return 1;
}

void print_free(struct buf_header *h) {
    struct buf_header *tmp = h;
    int cnt = 0;
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
        if (cnt == 3 || cnt == 7)
            printf("\n");
        cnt++;
    }
    printf("\n");
}

void buf_free(struct buf_header *p)
{
    remove_free(p);
    remove_hash(p);
    free(&(p->blkno));
    free(p->cache_data);
    free(&(p->stat));
    free(p);
}
