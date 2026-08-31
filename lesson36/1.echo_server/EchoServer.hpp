#ifndef _ECHOSERVER_HPP
#define _ECHOSERVER_HPP

#include <iostream>
#include <stdlib.h>
#include<string>
#include <sys/socket.h>
#include "Logger.hpp"
#include "Mutex.hpp"
#include<netinet/in.h>
#include <strings.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<cstring>

using namespace NS_LOG_MODULE;

const static int default_fd = -1;
//const static int default_fd = 8888;

enum {
    SUCCESS = 0,
    SOCKET_ERR,
    USAGE_ERR,
    BIND_ERR
};

class UdpServer
{
public:
    UdpServer(const std::string &ip,uint16_t port)//为端口号设计一个缺省值
    : _sockfd(default_fd)
    ,_ip(ip)
    ,_port(port)
    {}
    ~UdpServer() 
    {
        if(_sockfd>0)
            close(_sockfd);//关闭文件描述符
    }

    void Init()
    {
        //第一步创建套接字，本质：也就是在打开网络文件（打开网卡这个设备）--具备系统特性
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (_sockfd < 0)
        {
            LOG(LogLevel::FATAL) << "create socket error";
            exit(SOCKET_ERR);
        }
        LOG(LogLevel::INFO) << "create socket success sockfd: " << _sockfd;
        //服务器的IP是多少，服务器的端口号是多少
        //第二步：填充网络信息,有没有把IP和端口信息设置到内核中
        //设置到你刚刚打开的网络socket对应的文件内部
        struct sockaddr_in local;//创建网络套接字类型
        //还没有struct——sockeadd_in实际上是一个数据类型，local这个变量是在用户栈上的，并没有设置到内核
        bzero(&local,sizeof(local));
        local.sin_family = AF_INET;//结构体子类的类型是一个网络套接字的类型
        //     local.sin_addr =_ip;//1.字符串风格的ip->4字节IP 2.hton主机序列转化为网络序列--函数接口
        //将字符串风格的IP地址及转化为整数，又转化为网络序列，返回给我
        local.sin_addr.s_addr=inet_addr(_ip.c_str());
        local.sin_port = htons(_port);//主机序列转化为网络序列才能接着发送

        //第三步：将前两步信息合并，将网络和端口号信息合并，并设置到内核中绑定IP和端口号
        int n =bind(_sockfd,(struct sockaddr*)&local,sizeof(local));
        if(n<0)//绑定失败
        {
            LOG(LogLevel::FATAL)<<"bind socket error";
            exit(BIND_ERR);
        }
        LOG(LogLevel::INFO)<<"bind socket success,ip: "<<_ip<<",port:"<<_port;
    }

    void Start() {
        char inbuffer[1024];
        while(true)
        {
            struct sockaddr_in peer;
            socklen_t len =sizeof(peer);
            //读和写
            ssize_t n = recvfrom(_sockfd,inbuffer,sizeof(inbuffer)-1,0,(struct sockaddr*)&peer,&len);
            if(n<0)
            {
                LOG(LogLevel::FATAL)<<"error";
            }
        }
    }

private:
    int _sockfd;
    std::string _ip;//"192.168.2.2"(字符串风格的点分十进制IP地址，让人看的）4字节IP
    uint16_t _port;//用户设置好的，Server端口号是唯一固定的
    //typedef了无符号短整型
};

#endif