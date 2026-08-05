#include<iostream>
#include<signal.h>
#include<unistd.h>
#include<stdlib.h>
//signo：是收到了几号信号，才让我执行的         
void sig_handler(int signo)
{
    std::cout <<"收到了一个信号: "<<signo<<"who: "<<getpid()<<std::endl;
    //自定义信号默认是二号信号，但是默认就不退出了
    exit(10);
}
int main()
{
    //调用一次就可以了，不用放到循环里        
    // signal(SIGINT,sig_handler);
    // signal(3,sig_handler);

    //signal(SIGFPE,sig_handler);
    // signal(SIGSEGV,sig_handler);


    while(true)
    {
        std::cout <<"test sig...,pig "<<getpid()<<std::endl;
        sleep(1);
        //模拟除0
        int a= 10;
        a /=0;
        //模拟野指针
        // int *p = nullptr;
        // *p = 100;
    }
    return 0;
}