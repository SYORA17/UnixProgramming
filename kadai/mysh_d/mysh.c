#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "mysh.h"


struct tkntype {
    int type;
    char *name;
} tkn[] = {
    TKN_NORMAL,         "NORMAL",
    TKN_REDIR_IN,       "REDIR_IN",
    TKN_REDIR_OUT,      "REDIR_OUT",
    TKN_REDIR_APPEND,   "REDIR_APPEND",
    TKN_PIPE,           "PIPE",
    TKN_BG,             "BG",
    TKN_EOL,            "EOL",
    TKN_EOF,            "EOF",
    TKN_NONE,           "NONE",
    0,                  "NULL"
};

char *pr_ttype(int ttype) {
    int i;
    for (i = 0; i < TKN_NUMS; i++) {
        if (tkn[i].type == ttype) {
            return tkn[i].name;
        }
    }
    return tkn[i].name;
}

int gettoken(char *token, int len)
{
    // fprintf(stderr, "got token: %s\n", token);
    char tmp_c;
    int c, i = 0, type = TKN_NONE;
    char *p = token;

    // while (isblank(c = getchar()));
    // c = getchar();
    tmp_c = token[0];
    c = (int)tmp_c;

    // if special character, return the corresponding type
    switch(c) {
        case EOF:
            return TKN_EOF;
        case '\n':
            return TKN_EOL;
        case '&':
            return TKN_BG;
        case '|':
            return TKN_PIPE;
        case '<':
            return TKN_REDIR_IN;
        case '>':
            if (token[1] == '>')
                return TKN_REDIR_APPEND;
            //ungetc(c, stdin);
            return TKN_REDIR_OUT;
        default:
            break;
    }

    // normal character is input
    // ungetc(c, stdin);
    for (i = 0; i < len - 1; i++) {
        c = token[i];
        // c = getchar();
        // fprintf(stderr, "%c", c);
        if (c != EOF && c != '\n' && c != '&' 
                && c != '<' && c != '>' && c != '|' && !isblank(c))
            *p++;
        else
            break;

    }
    // ungetc(c, stdin);
    *p = '\0';
    if (i > 0)
        return TKN_NORMAL;
    else
        return TKN_NONE;
}

int getargs(int *argc, char *argv[], char *buf) {
    *argc = 0;
    char c;
    int prev_space_flg = 0;
    int c_pos = 0;
    int i = 0;
    // printf("%lu", strlen(buf));
    // while((c = getchar()) != EOF) {
    for (i = 0; i < strlen(buf); i++) {
        c = buf[i];
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

int main() {
    int i;
    char buf[256];
    int ac;
    int ttype[MAX_ARGC];
    char *av[MAX_ARGC];
    for (i = 0; i < MAX_ARGC; i++) {
        av[i] = malloc(sizeof(char) * MAX_ARGV);
        memset(av[i], '\0', sizeof av[i]);
    }
    int pid;
    int stat;
    for(;;) {
        for (i = 0; i < MAX_ARGC; i++) {
            memset(av[i], '\0', sizeof av[i]);
        }
        fprintf(stderr, "simplesh$ ");
        if (fgets(buf, sizeof buf, stdin) == NULL) {
            if (ferror(stdin)) {
                perror("fgets");
                exit(1);
            }
            exit(0);
        }
        buf[strlen(buf) - 1] = '\0';
        getargs(&ac, av, buf);
        fprintf(stderr, "\t** argv start ** \n");
        for (i = 0; i < ac + 1; i++)
            fprintf(stderr, "\tac[%d]: %s\n", i, av[i]);
        fprintf(stderr, "\t** argv end **\n");
        
        // ttype[0] = gettoken(av[0], TOKENLEN);
        // fprintf(stderr, "\tType: %s, %s\n", pr_ttype(ttype[0]), av[0]);
        
        for (i = 0; i < ac + 1; i++) {
            ttype[i] = gettoken(av[i], TOKENLEN);
            /*
            if (ttype[i] != TKN_EOL && ttype[i] != TKN_EOF) {
                fprintf(stderr, "\tType: %s, %s\n", pr_ttype(ttype[i]), av[i]);
            }
            if (ttype[i] == TKN_EOL)
                fprintf(stderr, "\tType");
            */
        }

        
        // if (ac == 0)
        //     continue;
        if (strcmp(av[0], ENDWORD) == 0)
            return 0;
        if ((pid = fork()) < 0) {
            perror("fork");
            exit(1);
        }
        if (pid == 0) {
            // child process
            fprintf(stderr, "\t** child: execute %s **\n", av[0]);

            char *bin_path = "/bin/";
            strcat(bin_path, av[0]);

            if (execvp(bin_path, av) < 0) {
                perror("execvp");
                exit(1);
            }
        }
        // parent process
        fprintf(stderr, "\t** parent: waif for child(%d) **\n", pid);
        if (wait(&stat) < 0) {
            perror("wait");
            exit(1);
        }
        fprintf(stderr, "\t** parent: wait end: %d **\n", stat);

    }
}
