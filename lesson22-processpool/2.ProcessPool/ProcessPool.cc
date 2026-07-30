#include <iostream>
#include <string>
#include<vector>
#include<cstdio>
#include <unistd.h>
enum//定义错误标准
{
    OK = 0,
    PIPE_ERROR,
    FORK_ERROR 
};
void DoTask(int fd)
{
    while(true)
    {
        sleep(1);
    }
}
const int gprocessnum =5;
//描述通道
//父进程管理通道                                   
class Channel {
public:
    Channel(int wfd,pid_t pid)
        :_wfd(wfd)
        ,_sub_pid(pid)
    {
        _sub_name ="sub-channel-"+std::to_string(_sub_pid);
    }
    void PrintInfo()
    {
        printf("wfd=%d,who= %d,channel name=%s\n",_wfd,_sub_pid,_sub_name.c_str());
    }
    ~Channel()
    {} 
private:
    int _wfd;//1.写端wfd
    pid_t _sub_pid;//2.子进程是谁？pid
    std::string _sub_name;//3.子进程名称

};
int main() {
    //0.未来组织所有Channel对象的容器
    std::vector<Channel> channels;
    //1.创建多个管道和创建多个进程
    for(int i = 0;i<gprocessnum;i++)//这个循环只有父进程执行，子进程不会执行
    {
        //1.创建管道
    int pipefd[2]={0};
    int n = pipe(pipefd);
    if(n < 0) 
    {
        std::cerr<<"pipe create error"<<std::endl;
        exit(PIPE_ERROR);
    }
    pid_t id = fork();
    if(id < 0)
    {
        std::cerr<<"fork error"<<std::endl;
        exit(PIPE_ERROR);
    }
    else if(id==0)    
    {
        //child
        close(pipefd[1]);//关闭写端
        DoTask(pipefd[0]);
        exit(OK);//根本不会执行后续代码，执行完自己的DoTask()函数后，直接退出
    }
    else
    {
        //parent
        close(pipefd[0]);//关闭读端
        Channel ch(pipefd[1],id);
        channels.push_back(ch);
        std::cout<<"创建子进程："<<id<<"成功..."<<std::endl;
        sleep(1);
    }
}
//2.父进程控制子进程
    for(auto &c:channels)
    {
        c.PrintInfo();
    }
//3.释放和回收所有资源（释放管道，回收子进程）

    return 0;
}