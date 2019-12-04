#ifndef MYSH_H
#define MYSH_H

#define TOKENLEN    80

#define TKN_NUMS    9

#define TKN_NONE         0000000
#define TKN_NORMAL       0000001
#define TKN_REDIR_IN     0000010
#define TKN_REDIR_OUT    0000020
#define TKN_REDIR_APPEND 0000030
#define TKN_PIPE         0000100
#define TKN_BG           0001000
#define TKN_EOL          0010000
#define TKN_EOF          0020000

#define MAX_ARGC         16
#define MAX_ARGV         256
#define MAX_BUF          256

#define CDWORD           "cd"
#define ENDWORD          "exit"

#endif
