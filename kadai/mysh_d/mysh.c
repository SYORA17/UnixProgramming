#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "mysh.h"

extern char **environ;

struct tkntype {
    int type;
    char *name;
} tkn[] = {
    TKN_NORMAL,         "NORMAL",
    TKN_OPTION,         "OPTION",
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
        case '-':
            return TKN_OPTION;
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
    int i, j;
    char buf[256];
    int ac;
    int ttype[MAX_ARGC];
    char *av[MAX_ARGC];
    /*
    char *coms[MAX_ARGC][MAX_ARGC];
    for (i = 0; i < MAX_ARGC; i++) {
        coms[i] = (char *)malloc(sizeof(char *) * MAX_ARGC);
    }
    */
    for (i = 0; i < MAX_ARGC; i++) {
        av[i] = malloc(sizeof(char) * MAX_ARGV);
        memset(av[i], '\0', sizeof av[i]);
        /*
        for (j = 0; j < MAX_ARGV; j++) {
            coms[i][j] = malloc(sizeof(char) * MAX_ARGV);
            memset(coms[i][j], '\0', sizeof coms[i][j]);
        }
        */
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

        int pre_type = TKN_NONE;
        int bg_flg = 0;
        int pipe_num = 0;
        int red_com_flg[ac + 1];
        int com_err_flg = 0;

        for (i = 0; i < ac + 1; i++) {
            ttype[i] = gettoken(av[i], TOKENLEN);
            /* 
            // use pr_ttype
            if (ttype[i] != TKN_EOL && ttype[i] != TKN_EOF) {
                fprintf(stderr, "\tType: %s, %s\n", pr_ttype(ttype[i]), av[i]);
            }
            if (ttype[i] == TKN_EOL)
                fprintf(stderr, "\tType");
            */
            
            if (i == 0) {
                if (ttype[i] != TKN_NORMAL && ttype[i] != TKN_EOL && ttype[i] != TKN_EOF) {
                    fprintf(stderr, "command error\n");
                    com_err_flg = 1;
                    break;
                }
            } else if (i == ac) {
                if (ttype[i] == TKN_BG) {
                    bg_flg = 1;
                }
            } else {
                if (pre_type == TKN_NORMAL || pre_type == TKN_OPTION) {
                    if (ttype[i] == TKN_PIPE) {
                         pipe_num += 1;
                    } else if (ttype[i] == TKN_REDIR_IN || ttype[i] == TKN_REDIR_OUT || ttype[i] || TKN_REDIR_APPEND) {
                        red_com_flg[i] = 1;
                    }
                } else if (pre_type == TKN_REDIR_IN || pre_type == TKN_REDIR_OUT || pre_type == TKN_REDIR_APPEND || pre_type == TKN_PIPE) {
                    if (ttype[i] != TKN_NORMAL) {
                        fprintf(stderr, "command error\n");
                        com_err_flg = 1;
                        break;
                    }
                }
            }
            pre_type = ttype[i];
        }
        
        // if (ac == 0)
        //     continue;
        if (strcmp(av[0], ENDWORD) == 0)
            return 0;
        if (com_err_flg == 1)
            continue;
        if (strcmp(av[0], "cd") == 0) {
            if (chdir(av[1]) != 0) {
                fprintf(stderr, "no such directory.\n");
            }
            continue;
        }
        if ((pid = fork()) < 0) {
            perror("fork");
            exit(1);
        }
        if (pid == 0) {
            // child process
            fprintf(stderr, "\t** child: execute %s **\n", av[0]);

            char *bin_path = "/bin/";
            strcat(bin_path, av[0]);
            av[0] = "pwd";
            av[1] = NULL;
                
            
            if (execve("/bin/pwd", av, environ) < 0) {
                perror("execve");
                exit(1);
            }
            /*
            if (execvp("/bin/pwd", av) < 0) {
                perror("execvp");
                exit(1);
            }
            */
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
