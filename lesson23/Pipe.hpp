#pragma once
#include <iostream>
#include <string>
#include <sys/stat.h> // mkfifo
#include <cstring>    // strerror
#include <cstdlib>    // exit
#include <cerrno>     // errno
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

const std::string gcommfile = "./fifo";//定义一个字符串
#define ForRead 1
#define ForWrite 2

class Fifo//定义命名管道类
{
public:
    Fifo(const std::string &commfile = gcommfile)//创建文件的两大要素1.什么文件 2.文件是什么权限
        : _commfile(commfile)
        , _mode(0666)
    {
    }
    // 1. 一个创建管道
    void Build()
    {
        if(IsExists())
            return;
        umask(0);//自定义权限掩码
        int n = mkfifo(_commfile.c_str(), _mode);//创建文件
        if (n < 0)
        {
            std::cerr << "mkfifo error: " << strerror(errno) << "errno: " << errno << std::endl;
            exit(1);
        }
        std::cerr << "mkfifo success: " << strerror(errno) << "errno: " << errno << std::endl;
    }               


    // 2. 一个打开管道:这里其实也就是对系统调用的一个封装
    void Open(int mode)
    {
        // r or w
        if(mode == ForRead)//判定以什么样的方式打开文件，如果是读
            _fd = open(_commfile.c_str(),O_RDONLY);
        else if(mode == ForWrite)//如果是写
            _fd = open(_commfile.c_str(),O_WRONLY);
        else
        {}

        if(_fd <0)
        {
            std::cerr<<"open error: "<< strerror(errno) << "errno: "<<errno << std::endl;
            exit(2);
        }
        else
        {
            std::cout << "open file success" <<std::endl;
        }
    }
    //发送消息:也就是向文件中写入
    void Send(const std::string &msgin)
    {
        ssize_t n = write(_fd,msgin.c_str(),msgin.size());
        (void)n;
    }
    //接收消息,也就是从缓冲区中读取文件
    int Recv(std::string *msgout)
    {
        char buffer[128];
        ssize_t n = read(_fd,buffer,sizeof(buffer)-1);
        //提出问题：上面为什么要-1,因为系统在读取文件的时候是不会解析符号的
        //你只想写入内容而不想带有/0就需要自行去除
        if(n>0)//n就是写入的字符数目
        {
            buffer[n] = 0;
            *msgout  = buffer;
            return n;
        }
        else if(n == 0)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    // 3. 删除管道
    void Delete(){
        if(!IsExists())
            return;//如果文件不存在则返回
        int n = unlink(_commfile.c_str());
        (void) n;
        std::cout<<"Ulink"<<_commfile <<std::endl;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
    }
    ~Fifo() {}
  
private:
    //判断文件是否存在，存在则返回真，不存在就返回假
    bool IsExists()
    {
        //不敢使用open判断，和管道的特点有关
        struct stat st;
        int n = stat(_commfile.c_str(),&st);
        if(n == 0)
        {
            std::cout << "file exists"<<std::endl;
            return true;
        }
        else
        {
            errno = 0;
            std::cout <<"file not exist exists"<<errno<<std::endl;
            return false;
        }
    }
    std::string _commfile;
    int _fd;
    mode_t _mode;
};