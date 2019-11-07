#ifndef _BUF_H_
#define _BUF_H_

struct buf_header
{
    int bufno;
    int blkno;
    struct buf_header *hash_fp;
    struct buf_header *hash_bp;
    unsigned int stat;
    struct buf_header *free_fp;
    struct buf_header *free_bp;
    char *cache_data;
};

#define NHASH 4
#define FIRST_BUF_NUM 12
#define MAX_ARGV 256
#define MAX_ARGC 16

/* init buffer*/
struct buf_header hash_head[NHASH];
struct buf_header free_head;
struct buf_header bufs[FIRST_BUF_NUM];

#define STAT_LOCKED 0x00000001
#define STAT_VALID 0x00000002
#define STAT_DWR 0x00000004
#define STAT_KRDWR 0x00000008
#define STAT_WAITED 0x00000010
#define STAT_OLD 0x00000020

int hash(int num);
struct buf_header *hash_search(int blkno);
struct buf_header *hash_search_bufno(int bufno);
void insert_head(struct buf_header *h, struct buf_header *p);
void insert_tail(struct buf_header *h, struct buf_header *p);
void insert_hash(struct buf_header *h, struct buf_header *p, int where);
void init_head_hash(struct buf_header *h, int blkno, unsigned int stat);
void init_free_hash(struct buf_header *h, int blkno, unsigned int stat);
void remove_hash(struct buf_header *p);
void remove_free(struct buf_header *p);
struct buf_header *remove_free_top(struct buf_header *h);
int isBuffer_hashlist(struct buf_header *h, struct buf_header *p);
int isBuffer_freelist(struct buf_header *h);
void add_hash(struct buf_header *h, struct buf_header *tmp, int bufno,int blkno, unsigned int stat, char *cache_data);
void add_free(struct buf_header *h, struct buf_header *p);
void add_free_top(struct buf_header *h, struct buf_header *p);
void print_hash(struct buf_header *h, int idx);
int print_buf(int bufno);
void print_free(struct buf_header *h);
void buf_free(struct buf_header *p);

long char2long(char *s);
void help_command();
void init();
void init_command(int argc, char *argv[]);
void buf_command(int argc, char *argv[]);
void hash_command(int argc, char *argv[]);
void free_command(int argc, char *argv[]);
struct buf_header *getblk(int blkno);
void getblk_command(int argc, char *argv[]);
void brelse(struct buf_header *buf);
void brelse_command(int argc, char *argv[]);
void set_command(int argc, char *argv[]);
void reset_command(int argc, char *argv[]);
void quit_command();

/*
void buf_proc(int, char *[]);
void hash_proc(int, char *[]);
void free_proc(int, char *[]);
void getblk_proc(int, char *[]);
void brelse_proc(int, char *[]);
void set_proc(int, char *[]);
void reset_proc(int, char *[]);
void quit_proc(int, char *[]);
*/
// struct commnad_table {
//     char *cmd;
//     void (*func)(int, char *[]);
// } cmd_tbl[] = {
//     {"help", help_proc},
//     {"init", init_proc},
//     {"buf", buf_proc},
//     {"hash", hash_proc},
//     {"free", free_proc},
//     {"getblk", getblk_proc},
//     {"brelse", brelse_proc},
//     {"set", set_proc},
//     {"reset", reset_proc},
//     {"quit", quit_proc},
//     {NULL, NULL},
// };

#endif
