#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
int main()
{
    printf("hello word");
    sleep(3);
    _exit(0);
    return 0;
}
