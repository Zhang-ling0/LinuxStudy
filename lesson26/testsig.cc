#include<iostream>
#include<signal.h>
#include<unistd.h>
void sig_handler(int signo)
{
    std::cout <<"catch signal "<<signo<<std::endl;
}
int main()
{
    signal(SIGINT,handler);
    while(true)
    {
        std::cout <<"test sig...,pig "<<getpid()<<std::endl;
        sleep(1);
    }
    return 0;
}