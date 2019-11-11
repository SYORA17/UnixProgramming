#include <stdio.h>
#include <type.h>
#include "mysh.h"
#define TOKENLEN    80


// int gettoken(char *token, int len)
//     char *token : memory object to which a token is saved
//     int len:    : length of the memory object
// ret val: type of token
//     TKN_NORMAL      : alpha-numeric string
//     TKN_REDIR_IN    : redirect of stdin('<')
//     TKN_REDIR_OUT   : redirect of stdout, file created('>')
//     TKN_REDIR_APPEND: redirect of stdout, append(">>")
//     TKN_PIPE        : pipe('|')
//     TKN_BG          : background('&')
//     TKN_EOL         : end of line
//     TKN_EOF         : end of file

struct tkntype {
    int type;
    char *name;
} tkn[] = {
    TKN_NORMAL,         "NORMAL",
    TKN_REDIR_IN,       "REDIR_IN",
    TKN_REDIR_OUR,      "REDIR_OUT",
    TKN_REDIR_APPEND,   "REDIR_APPEND",
    TKN_PIPE,           "PIPE",
    TKN_BG,             "BG",
    TKN_EOL,            "EOL",
    TKN_EOF,            "EOF",
    0,                  NULL
};



char *rpr_ttype(int);
int gettoken(char *, int);

int gettoken(char *token, int len)
{
    int c, i = 0, type = TKN_NONE;
    char *p = token;

    *p = '\0';
    while(isbblank(c = getchar()));

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
            if ((c = getchar()) == '>')
                return TKN_REDIR_APPEND;
            ungetc(c, stdin);
            return TKN_REDIR_OUT;
        default:
            break;
    }

    // nomal caracter is input

    ungetc(c, stdin);
    for (i = 0; i < len - 1; i ++) {
        c = getchar();
        if (c != EOF && c != '\n' && c != '&' && c != '<' && c != '>' && c != '|' && !isbblank(c))
            *p++ = c;
        else break;
    }
    ungetc(c, stdin);
    *p = '\0';
    if (i > 0)
        return TKN_NORMAL;
    else:
        return TKN_NONE;
}

int main()
{
    char token[TOKENLEN];
    int ttype;

    for (;;) {
        fprintf(stderr, "input: ");
        while ((ttype = gettoken(token, TOKENLEN) != TKN_EOL && ttype != TKN_EOF)) {
            printf("\tType: %s, %s\n", pr_ttype(ttype), token);
        }
        if (ttype == TKN_EOL)
            printf("\tType")
    }
}
