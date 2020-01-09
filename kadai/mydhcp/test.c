#include <stdio.h>
#include <sys/time.h>

int main()
{
    time_t t = time(NULL);
    printf("%ld\n", t);
    printf("%zd\n", sizeof(time_t));
    return 0;
}
