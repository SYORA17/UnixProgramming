#include <stdio.h>
#include <string.h>
#include <ctype.h>

// triming space
void trim( char *s ) {
    int i, j;
    for( i = strlen(s)-1; i >= 0 && isspace( s[i] ); i-- );
    s[i+1] = '\0';
    for( i = 0; isspace( s[i] ); i++ );
    if( i > 0 ) {
        j = 0;
        while( s[i] ) s[j++] = s[i++];
        s[j] = '\0';
    }
}

void count_length(char *buf) {
    size_t length;
    length = strlen(buf);
    printf("  %zu\n", length);
}

// read 1-row strings
// if EOF: retrun -1
int getargs(int *argc, char *argv[]) {
    char buffer[256];
    size_t length;

    printf("Input: ");
    if (fgets(buffer, 256, stdin) == NULL || buffer[0] == '\n') {
        return -1;
    }
    trim(buffer);
    length = strlen(buffer);
    if (buffer[length - 1] == '\n') {
        buffer[--length] = '\0';
    }
    printf("%s", buffer);
    count_length(buffer);
    // argv[*argc] = buffer;
    // printf("%zu\n", sizeof buffer);
    printf("%zu\n", sizeof argv[*argc]);
    // memset(argv[*argc], 0, sizeof buffer);
    // strncpy(argv[*argc], buffer, sizeof buffer - 1);
    printf("%s", argv[*argc]);
    *argc += 1;
    return 0;
}

int main(int argc, char *argv[]) {
    while(getargs(&argc, argv) == 0) {
        printf("argc : %d, argv : %s\n", argc, argv[argc]);
        int i = 0;
    }
    // char buffer[256];
    // size_t length;
    // while(fgets(buffer, 256, stdin) != NULL) {
    //     trim(buffer);
    //     printf("%s ", buffer);
    //     count_length(buffer);
    // }

    for (int i = 0; i < argc; i++) {
        printf("%s", argv[i]);
    }

    return 0;
}
