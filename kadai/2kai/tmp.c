#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

int main(int argc, char *argv[]) {
    long tmp;
    tmp = strtol(argv[1], NULL, 10);
    printf("%ld", tmp);
}
