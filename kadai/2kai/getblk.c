#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <ctype.h>
#include "buf.h"

//char to long
long char2long(char *s) {
    long tmp = 0;
    if (isdigit(s[0])) {
        tmp = strtol(s, NULL, 10);
        return tmp;
    }
    else
        return -1;
}


//help
void help_command()
{
    printf(" - help : print help.\n");
    printf(" - init : initailize hash list and free list.\n");
    printf(" - buf : print all buffer status.\n");
    printf(" - buf [n...] : print buffer status you selected.\n");
    printf(" - hash : print all hash list.\n");
    printf(" - hash [n...] : print hash list you selected.\n");
    printf(" - free : print free list.\n");
    printf(" - getblk n : execute getblk by using blkno->n buffer\n");
    printf(" - brelse n : execute brelse by using blkno->n buffer's back pointer.\n");
    printf(" - set n stat : set status to blkno->n buffer.\n");
    printf(" - reset n stat : reset status to blkno->n buffer.\n");
    printf("        (stat) -> [L, V, D, K, W, O]\n");
    printf(" - quit : quit this program.\n");
}

//init
void init(/*int argc, char *argv[]*/)
{
    /*
    if (argc != 1) {
        fprintf(stderr, "usage : init\n");
        return;
    }
    */
    for(int i = 0; i < NHASH; i++) {
        hash_head[i] = *(struct buf_header *)malloc(sizeof(struct buf_header));
    }
    free_head = *(struct buf_header *)malloc(sizeof(struct buf_header));
    init_head_hash(&hash_head[0], 0, STAT_VALID);
    init_head_hash(&hash_head[1], 0, STAT_VALID);
    init_head_hash(&hash_head[2], 0, STAT_VALID);
    init_head_hash(&hash_head[3], 0, STAT_VALID);
    init_free_hash(&free_head, 0, STAT_VALID);
    for (int i = 0; i < FIRST_BUF_NUM; i++) {
        bufs[i] = *(struct buf_header *)malloc(sizeof(struct buf_header));
    }
    int init_nums[FIRST_BUF_NUM] = {28, 4,64, 17, 5, 97, 98, 50, 10, 3, 35, 99};
    for (int i = 0; i < FIRST_BUF_NUM; i++) {
        int head_idx = i / 3;
        add_hash(&hash_head[head_idx], &bufs[i], i, init_nums[i], (STAT_VALID | STAT_LOCKED), "hi");
    }
    add_free(&free_head, &bufs[9]);
    add_free(&free_head, &bufs[4]);
    add_free(&free_head, &bufs[1]);
    add_free(&free_head, &bufs[0]);
    add_free(&free_head, &bufs[5]);
    add_free(&free_head, &bufs[8]);
    return;
}

void init_command(int argc, char *argv[])
{
    if (argc != 0) {
        fprintf(stderr, "usage : init\n");
        return;
    }
    /*
    // free all bufs;
    struct buf_header *p;
    for (int i = 0; i < NHASH; i++) {
        printf("hi");
        p = &hash_head[i];
        do {
            buf_free(p->hash_fp);
        } while(p->hash_fp != p);
        // buf_free(p);
    }
    printf("done.");
    buf_free(&free_head);
    */
    init_head_hash(&hash_head[0], 0, STAT_VALID);
    init_head_hash(&hash_head[1], 0, STAT_VALID);
    init_head_hash(&hash_head[2], 0, STAT_VALID);
    init_head_hash(&hash_head[3], 0, STAT_VALID);
    init_free_hash(&free_head, 0, STAT_VALID);
    int init_nums[FIRST_BUF_NUM] = {28, 4,64, 17, 5, 97, 98, 50, 10, 3, 35, 99};
    for (int i = 0; i < FIRST_BUF_NUM; i++) {
        int head_idx = i / 3;
        add_hash(&hash_head[head_idx], &bufs[i], i, init_nums[i], (STAT_VALID | STAT_LOCKED), "hi");
    }
    add_free(&free_head, &bufs[9]);
    add_free(&free_head, &bufs[4]);
    add_free(&free_head, &bufs[1]);
    add_free(&free_head, &bufs[0]);
    add_free(&free_head, &bufs[5]);
    add_free(&free_head, &bufs[8]);
    printf("just initialized!\n");
    return;
}

//buf [n ...]
void buf_command(int argc, char *argv[]) {
    if (argc == 0) {
        // error occurs?
        for (int i = 0; i < FIRST_BUF_NUM; i++) {
            print_buf(i);
        }
    } else {
        if (argc > FIRST_BUF_NUM) {
            fprintf(stderr, "usage: buf [0, ... 11].\n");
        } else {
            for (int i = 1; i <= argc; i++) {
                long tmp;
                tmp = char2long(argv[i]);
                if (tmp >= FIRST_BUF_NUM || tmp < 0) {
                    fprintf(stderr, "usage: buf [0, ... 11].\n");
                } else {
                    print_buf(tmp);
                }
            }
        }
    }
}

//hash [n ...]
void hash_command(int argc, char *argv[]) {
    if (argc == 0) {
        for (int i = 0; i < NHASH; i++) {
            print_hash(&hash_head[i], i);
        }
    } else {
        if (argc > NHASH) {
            fprintf(stderr, "usage: hash [0, 1, 2, 3].\n");
        } else {
            for (int i = 1; i <= argc; i++) {
                long tmp;
                tmp = char2long(argv[i]);
                // printf("tmp : %ld", tmp);
                if (tmp > NHASH -1 || tmp < 0) {
                    fprintf(stderr, "usage: hash [0, 1, 2, 3].\n");
                } else {
                    print_hash(&hash_head[tmp], tmp);
                }
            }
        }
    }
}


//free [n ...]
void free_command(int argc, char *argv[]) {
    if (argc != 0) {
        fprintf(stderr, "usage : free\n");
    } else {
        print_free(&free_head);
    }
}

//getblk n
struct buf_header *getblk(int blkno)
{
    int head_idx = hash(blkno);
    struct buf_header *p;
    do {//buffer not found
        p = hash_search(blkno);
        if (p != NULL) {
            if (p->stat & STAT_LOCKED) {
                // scenario 5
                // sleep(event buffer becomes free);
                printf("Process goes to sleep\n");
                p->stat = STAT_LOCKED | STAT_WAITED | STAT_VALID;
                // brelse(p);
                // p->stat = STAT_VALID;
                // continue;
                return NULL;
            }
            //scenario 1
            // REQ:search buf is in hash list and free.
            // AFF:delete it from free list, and make it locked.
            // return pointer to buf.
            remove_free(p);
            p->stat |= STAT_LOCKED;
            return p;
        } else {
            if (isBuffer_freelist(&free_head) == -1) {
                // scenario 4
                // sleep(event any buffer becomes free);
                printf("Process goes to sleep\n");
                brelse(p);
                // continue;
                return NULL;
            }
            // remove buffer from free list;
            if (isBuffer_freelist(&free_head) == 0) {
                // scenario 3
                // asynchronous write buffer to disk;
                p = remove_free_top(&free_head);
                p->stat = STAT_LOCKED | STAT_VALID | STAT_KRDWR | STAT_OLD;
                // p->stat = STAT_LOCKED | STAT_OLD;
                continue;
            }
            // scenario 2
            // remove buffer from old hash queue;
            // put buffer onto new hash queue;
            // return pointer to buffer;
            p = remove_free_top(&free_head);
            remove_hash(p); // p was top of free list
            add_hash(&hash_head[head_idx], p, p->bufno, blkno, (STAT_VALID | STAT_LOCKED), "hi");
            return p;
        }
    } while(p->blkno != blkno);
    return NULL;
}

void getblk_command(int argc, char *argv[]) {
    if (argc != 1) {
        fprintf(stderr, "usage : getblk n\n");
        return;
    }
    long blkno;
    blkno = char2long(argv[1]);
    getblk(blkno);
    return;
}


// brelse n
void brelse(struct buf_header *buf)
{
    if (buf == NULL) {
        // scenario 4
        // wakeup all procs: event, waiting for any buffer to become free;
        printf("Wakeup processes waiting for any buffer\n");
        return;
    }
    int hash_no = hash(buf->blkno);
    if (isBuffer_hashlist(&hash_head[hash_no], buf)) {
        // scenario 5
        // wakeup all procs: event, waiting for this buffer to become free;
        printf("Wakeup processes waiting for buffer of blkno %d\n", buf->blkno);
    }
    // raise_cpu_lebel();
    if ((buf->stat & STAT_VALID) && (buf->stat & STAT_OLD)) {
        // scenario 3
        printf("enqueue buffer at beginning of free list\n");
        add_free_top(&free_head, buf);
    } else {
        printf("enqueue buffer at end of free list\n");
        add_free(&free_head, buf);
    }
    // lower_cpu_level();
    // make buffer unlocked.
    buf->stat &= ~STAT_LOCKED;
}

void brelse_command(int argc, char *argv[]) {
    if (argc != 1) {
        fprintf(stderr, "usage : brelse n\n");
        return;
    }
    long blkno;
    blkno = char2long(argv[1]);
    struct buf_header *tmp = hash_search(blkno);
    if (tmp == NULL) {
        fprintf(stderr, "couldn't find such a buffer.\n");
        return;
    }
    brelse(tmp);
    return;
}

// set n stat
void set_command(int argc, char *argv[]) {
    // (stat) -> [L, V, D, K, W, O]
    if (argc > 7 || argc < 2) {
        fprintf(stderr, "usage : set n [L, V, D, K, W, O]\n");
    } else {
        long blkno;
        blkno = char2long(argv[1]);
        // if buffer exists
        struct buf_header *tmp = hash_search(blkno);
        if (tmp == NULL) {
            fprintf(stderr, "coludn't find such a buffer.\n");
            return;
        }
        // search by blkno
        for (int i = 2; i <= argc; i++) {
            if (strcmp(argv[i], "L") == 0)
                tmp->stat |= STAT_LOCKED;
            else if (strcmp(argv[i], "V") == 0)
                tmp->stat |= STAT_VALID;
            else if (strcmp(argv[i], "D") == 0)
                tmp->stat |= STAT_DWR;
            else if (strcmp(argv[i], "K") == 0)
                tmp->stat |= STAT_KRDWR;
            else if (strcmp(argv[i], "W") == 0)
                tmp->stat |= STAT_WAITED;
            else if (strcmp(argv[i], "O") == 0)
                tmp->stat |= STAT_OLD;
            else {
                fprintf(stderr, "usage : set n [L, V, D, K, W, O]\n");
                return;
            }
        }
    }
}

// reset n stat
void reset_command(int argc, char *argv[]) {
    // (stat) -> [L, V, D, K, W, O]
    if (argc > 7 || argc < 2) {
        fprintf(stderr, "usage : reset n [L, V, D, K, W, O]\n");
    } else {
        long blkno;
        blkno = char2long(argv[1]);
        // if buffer exists
        struct buf_header *tmp = hash_search(blkno);
        if (tmp == NULL) {
            fprintf(stderr, "coludn't find such a buffer.\n");
            return;
        }
        // search by blkno
        for (int i = 2; i <= argc; i++) {
            if (strcmp(argv[i], "L") == 0)
                tmp->stat &= ~STAT_LOCKED;
            else if (strcmp(argv[i], "V") == 0)
                tmp->stat &= ~STAT_VALID;
            else if (strcmp(argv[i], "D") == 0)
                tmp->stat &= ~STAT_DWR;
            else if (strcmp(argv[i], "K") == 0)
                tmp->stat &= ~STAT_KRDWR;
            else if (strcmp(argv[i], "W") == 0)
                tmp->stat &= ~STAT_WAITED;
            else if (strcmp(argv[i], "O") == 0)
                tmp->stat &= ~STAT_OLD;
            else {
                fprintf(stderr, "usage : reset n [L, V, D, K, W, O]\n");
                return;
            }
        }
    }
}

// quit
void quit_command()
{
    exit(0);
}
