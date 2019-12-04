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
