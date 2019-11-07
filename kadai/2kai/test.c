#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
// #include <ctype.h>
#include "buf.h"

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

void char2int(char *s) {
    long tmp = 0;
    tmp = strtol(s, NULL, 10);
    printf("%ld", tmp);
}

char *command_arg(char *cmd) {
    char *arg_num = "0";
    if (strcmp(cmd, "help") == 0) {
        arg_num = "0";
    } else if (strcmp(cmd, "init") == 0) {
        arg_num = "0";
    } else if (strcmp(cmd, "buf") == 0) {
        arg_num = "0 or over 0";
    } else if (strcmp(cmd, "quit") == 0) {
        exit(1);
    }
    return arg_num;
}

int main() {
    int i;
    int argc = 0;
    char **argv;
    for (i = 0; i < MAX_ARGC; i++) {
        argv[i] = malloc(sizeof(char) * MAX_ARGV);
    }

    printf("$ ");
    while(getargs(&argc, argv) == 0) {
        char *arg_num;
        for (i = 0; i <= argc; i++) {
            printf("argc: %d, argv: %s\n", i, argv[i]);
        }
        arg_num = command_arg(argv[0]);
        printf("%s\n", arg_num);
        printf("$ ");
    }
    return 0;
}
