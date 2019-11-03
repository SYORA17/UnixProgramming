#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "buf.h"

//char to long
void char2long(char *s) {
    long tmp = 0;
    tmp = strtol(s, NULL, 10);
    printf("%ld", tmp);
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
    printf(" - quit : quit this program.\n");
}

//init
void init_command(int argc, char *argv[])
{
    if (argc != 1) {
        fprintf(stderr, "usage : init.\n");
        return;
    }
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
        add_hash(&hash_head[head_idx], &bufs[i], i, init_nums[i], STAT_VALID, "hi");
    }
    add_free(&free_head, &bufs[9]);
    add_free(&free_head, &bufs[4]);
    add_free(&free_head, &bufs[1]);
    add_free(&free_head, &bufs[0]);
    add_free(&free_head, &bufs[5]);
    add_free(&free_head, &bufs[8]);
    return;
}

//buf [n ...]
void buf_command(int argc, char *argv[]) {
    if (argc == 1) {
        // error occurs?
        for (int i = 0; i < FIRST_BUF_NUM; i++) {
            print_buf(&hash_head[i/3],i);
        }
    } else {
        if (argc > FIRST_BUF_NUM) {
            fprintf(stderr, "usage: buf [0, ... 11].\n");
        } else {
            for (int i = 1; i < argc - 1; i++) {
                long tmp;
                tmp = cahr2long(argv[i]);
                if (tmp > NHASH -1 || tmp < 0) {
                    fprintf(stderr, "usage: buf [0, ... 11].\n");
                } else {
                    print_buf(&hash_head[tmp/3], tmp);
                }
            }
        }
    }
}

//hash [n ...]
void hash_command(int argc, char *argv[]) {
    if (argc == 1) {
        for (int i = 0; i < NHASH; i++) {
            print_hash(&hash_head[i], i);
        }
    } else {
        if (argc > NHASH) {
            fprintf(stderr, "usage: hash [0, 1, 2, 3].\n");
        } else {
            for (int i = 1; i < argc - 1; i++) {
                long tmp;
                tmp = cahr2long(argv[i]);
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
    if (argc != 1) {
        fprintf(stderr, "usage : free\n");
    } else {
        print_free(&free_head);
    }
}

//getblk n
struct buf_header *getblk(int blkno)
{
    // while(/* buffer not found*/1) {
    // search_hash_que;
    // if ()
    // }
}

// brelse n

// set n stat

// reset n stat

// quit
