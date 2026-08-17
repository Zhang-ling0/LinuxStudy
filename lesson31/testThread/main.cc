#include<iostream>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<vector>
int tickets = 100;//共享资源，尝试对一个共享资源进行更新                                  

void* route(void* args)
{
    //类型转化用以接收函数
    char* id =(char*) args;
    while(1){
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