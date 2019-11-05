#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "buf.h"

// struct buf_header hash_head[NHASH];
// struct buf_header free_head;

// get argmunets
int getargs(int *argc, char *argv[]) {
    *argc = 0;
    char c;
    int prev_space_flg = 0;
    int c_pos = 0;
    while((c = getchar()) != EOF) {
        if (c == '\n') {
            if (prev_space_flg == 0) {
                argv[*argc][c_pos] = '\0';
            }
            return 0;
        } else if (c == ' ' || c == '\t') {
            if (prev_space_flg == 0) {
                prev_space_flg = 1;
                argv[*argc][c_pos] = '\0';
            }
        } else {
            if (prev_space_flg == 1) {
                *argc += 1;
                c_pos = 0;
            }
            if (*argc >= MAX_ARGC) {
                fprintf(stderr, "command should be under %d.\n", MAX_ARGC);
                return -1;
            }
            prev_space_flg = 0;
            if (c_pos > MAX_ARGV - 2) {
                break;
            } else {
                argv[*argc][c_pos++] = c;
            }
        }
    }
    return -1;
}
/*
void chach_args(int *argc, char *argv[]) {
    struct command_table *p;
    for (p = cmd_tbl; p->cmd; p++) {
        if (strcmp(argv[0], p->cmd) == 0) {
            (*p->func)(&argc, argv);
            break;
        }
    }
    if (p->cmd == NULL) {
        fprintf(stderr, "unknown command\n");
    }
    printf("done\n");
}
*/

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

/*
void help_command();
void init_command();
void buf_command(int argc, char *argv[]);
void hash_command(int argc, char *argv[]);
void free_command(int argc, char *argv[]);
void set_command(int argc, char *argv[]);
void reset_command(int argc, char *argv[]);
void quit_command();
*/

int main()
{
    // 61709736 Takanori Shirasaka
    init();
    // print_hash(&hash_head[1], 0);
    for (int i = 0; i < FIRST_BUF_NUM; i++) {
        print_buf(&hash_head[i/3],i);
    }

    // print_free(&free_head);

    int argc = 0;
    char **argv;
    argv = malloc(MAX_ARGC * sizeof(char *));
    for (int i = 0; i < MAX_ARGC; i++) {
        argv[i] = malloc(sizeof(char) * MAX_ARGV);
    }

    printf("$ ");
    while(getargs(&argc, argv) == 0) {
        // char *arg_num;
        char cmd[MAX_ARGC];
        memset(cmd, 0, sizeof cmd);
        strncpy(cmd, argv[0], sizeof cmd - 1);
        // for (int i = 0; i <= argc; i++) {
        //     printf("argc: %d, argv: %s\n", i, argv[i]);
        // }
        if (strcmp(cmd, "help") == 0) {
            help_command();
        } else if (strcmp(cmd, "init") == 0) {
            init_command();
        } else if (strcmp(cmd, "buf") == 0) {
            buf_command(argc, argv);
        } else if (strcmp(cmd, "hash") == 0) {
            hash_command(argc, argv);
        } else if (strcmp(cmd, "free") == 0) {
            free_command(argc, argv);
        } else if (strcmp(cmd, "set") == 0) {
            set_command(argc, argv);
        } else if (strcmp(cmd, "reset") == 0) {
            reset_command(argc, argv);
        } else if (strcmp(cmd, "quit") == 0) {
            quit_command();
        } else {
            fprintf(stderr, "command error! use $ help\n");
        }
        printf("$ ");
    }

   return 0;

}
