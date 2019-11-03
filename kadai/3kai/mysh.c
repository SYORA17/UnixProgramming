#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_ARGC 16
#define MAX_ARGV 256

extern char **environ;

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

int main(int argc, char *argv[]) {
    printf("mysh$ ");
    int pid;
    while (getargs(&argc, argv) == 0) {
        if (strcmp(argv[0], "exit")) {
            exit(0);
        }
        if ((pid = fork()) < 0) {
            // error
            fprintf(stderr, "error!");
            exit(1);
        } else if (pid == 0) {
            execvp(argv[0], **argv);
            //子プロセスの処理
        } else {
            wait(0);
            //親プロセスの処理
        }
        printf("mysh$ ");
    }
}
