#include<iostream>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<vector>
#include"Thread.hpp"
int tickets = 1000;//共享资源，尝试对一个共享资源进行更新                                  
Mutex mutex;
void* route(void* args)
{
    char* id =(char*) args;
    while(1){
        grouplock lock(mutex);//要放到循环里面，不然只会加一次锁
        if(tickets>0)
        {
            usleep(1000);//模拟具体抢票花的时间
            printf("%s get tickets: %d\n",id,tickets);
            tickets--;
        }
        else
        {
            break;
        }
    }
    return nullptr;
}

int main(){
    pthread_t t1,t2,t3,t4;
    pthread_create(&t1,NULL,route,(void*)"thread 1");
    pthread_create(&t2,NULL,route,(void*)"thread 2");
    pthread_create(&t3,NULL,route,(void*)"thread 3");
    pthread_create(&t4,NULL,route,(void*)"thread 4");

    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    pthread_join(t3,NULL);
    pthread_join(t4,NULL);

    return 0;
}