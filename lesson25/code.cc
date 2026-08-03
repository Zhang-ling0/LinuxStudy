#include<cstdio>
#include<unistd.h>

int main(){
    pid_t pid = fork();
    if(pid==0)
    {
        while(1)
        {
            printf("A");
            fflush(stdout);
            usleep(1240);
            printf("A");
            usleep(1240);

            printf("\n");
        }  
    }
    while(1)
    {
        printf("B");
        fflush(stdout);
        usleep(1240);
        printf("B");
        usleep(1240);
        printf("\n");
    }
}