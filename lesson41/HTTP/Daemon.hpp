#pragma once
#include <iostream>
#include<signal.h>
#include<unistd.h>
#include <fcntl.h>
//带两个选项1 1
void Daemon(int isdup,int isrdir)
{
    //1.忽略信号
    signal(SIGPIPE,SIG_IGN);//13
    signal(SIGCHLD,SIG_IGN);//多进程不等
    //2.不能是组长
    if(fork()>0)
        exit(0);//是父进程就直接退出
    //3. 设置新会话--替父从军
    pid_t id =setsid();
    (void)id;
    //上面就依旧可以了，下面是注意事项
    //更改守护进程的工作路径，根目录
    if(isrdir)
    chdir("/home/ZL/code/lesson41/HTTPS"); 
    if(isdup)
    {
        close(0);
        close(1);
        close(2);

    } 
    //5.重定向0，1，2
    int fd = open("/dev/null",O_RDWR);//读写方式打开
    if(fd>=0)
    {
        dup2(fd,0);
        dup2(fd,1);
        dup2(fd,2);//三个标准都重定向到fd中
        close(fd);
    }
}