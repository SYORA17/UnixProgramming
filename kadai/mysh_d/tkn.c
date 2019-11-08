#define TOKENLEN    80

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
