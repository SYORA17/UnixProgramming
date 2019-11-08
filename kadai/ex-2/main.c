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
    int first_flg = 1;
    int c_pos = 0;
    while((c = getchar()) != EOF) {
        if (c == '\n') {
            if (prev_space_flg == 0) {
                argv[*argc][c_pos] = '\0';
            }
            return 0;
        } else if (c == ' ' || c == '\t') {
            if (first_flg) {
                continue;
            }
            if (prev_space_flg == 0) {
                prev_space_flg = 1;
                argv[*argc][c_pos] = '\0';
            }
        } else {
            first_flg = 0;
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

int main()
{
    int i;
    // 61709736 Takanori Shirasaka
    init();
    int argc = 0;
    char **argv;
    argv = malloc(MAX_ARGC * sizeof(char *));
    for (i = 0; i < MAX_ARGC; i++) {
        argv[i] = malloc(sizeof(char) * MAX_ARGV);
    }

    printf("$ ");
    while(getargs(&argc, argv) == 0) {
        // char *arg_num;
        char cmd[MAX_ARGC];
        memset(cmd, 0, sizeof cmd);
        strncpy(cmd, argv[0], sizeof cmd - 1);
        if (strcmp(cmd, "help") == 0) {
            help_command();
        } else if (strcmp(cmd, "init") == 0) {
            init_command(argc, argv);
        } else if (strcmp(cmd, "buf") == 0) {
            buf_command(argc, argv);
        } else if (strcmp(cmd, "hash") == 0) {
            hash_command(argc, argv);
        } else if (strcmp(cmd, "free") == 0) {
            free_command(argc, argv);
        } else if (strcmp(cmd, "getblk") == 0) {
            getblk_command(argc, argv);
        } else if (strcmp(cmd, "brelse") == 0) {
            brelse_command(argc, argv);
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
