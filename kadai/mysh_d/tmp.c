#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_ARGC 16
#define MAX_ARGV 256
#define MAX_BUF 256

#define ENDWORD "\n"

3

int main() {
    int i;
    char buf[256];
    int ac;
    char *av[MAX_ARGC];
    for (i = 0; i < MAX_ARGC; i++) {
        av[i] = malloc(sizeof(char) * MAX_ARGV);
    }
    int pid;
    int stat;
    for(;;) {
        fprintf(stderr, "simplesh$ ");
        if (fgets(buf, sizeof buf, stdin) == NULL) {
            if (ferror(stdin)) {
                perror("fgets");
                exit(1);
            }
            exit(0);
        }
        buf[strlen(buf) - 1] = '\0';
        memset(av, 0, sizeof av);
        getargs(&ac, av, buf);

        fprintf(stderr, "\t** argv start ** \n");
        for (i = 0; i < ac, i++)
            fprintf(stderr, "\tac[%d]: %s\n", i, av[i]);
        fprintf(stderr, "\t** argv end **\n");

        if (ac == 0)
            continue;
        if (strcmp(av[0], ENDWORD) == 0)
            return 0;
        if ((pid = fork()) < 0) {
            perror("fork");
            exit(1);
        }
        if (pid == 0) {
            // child process
            fprintf(stderr, "\t** child: execute %s **\n", av[0]);

            if (execvp(av[0], av) < 0) {
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
