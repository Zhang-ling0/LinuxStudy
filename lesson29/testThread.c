//#include<iostream>
#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<stdbool.h>
void *threadRun(void *args)
{
        while(true)
        {
            printf("new thread is running,pid:%d\n",getpid());
            // std::cout << "new thread is running" << std::endl;
            sleep(1);
        }             
}

int main()
{
    pthread_t tid;
    pthread_create(&tid,NULL,threadRun,NULL);

    while(true)
    {
        printf("main thread is running,pid:%d\n",getpid());
        // std::cout << "main thread is running" << std::endl;
        sleep(1);
    }
}