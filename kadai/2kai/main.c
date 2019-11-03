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

int main()
{
    // 61709736 Takanori Shirasaka
    init_command();
    // print_hash(&hash_head[0], 0);
    // for (int i = 0; i < FIRST_BUF_NUM; i++) {
    //     print_buf(&hash_head[i/3],i);
    // }
    print_free(&free_head);

    int argc = 0;
    char **argv;
    for (int i = 0; i < MAX_ARGC; i++) {
        argv[i] = malloc(sizeof(char) * MAX_ARGV);
    }

    printf("$ ");
    while(getargs(&argc, argv) == 0) {
        char *arg_num;
        for (int i = 0; i <= argc; i++) {
            printf("argc: %d, argv: %s\n", i, argv[i]);
        }
        // command_arg(&argc, argv);
        // printf("%s\n", arg_num);
        printf("$ ");
    }

   return 0;

}
