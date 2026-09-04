#pragma once
#include<iostream>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include <cstring>
#include"Logger.hpp"
#include<unistd.h>
using namespace NS_LOG_MODULE;
enum{
    SUCCESS=0,
    SOCKET_ERR,
    BIND_ERR,
    LISTEN_ERR,
    USAGE_ERR
};
static const int gbacklog=16;
static const uint16_t gport=888;
class TcpServer
{
public:
    TcpServer(uint16_t port=gport)
        :_port(port)
    {}
    void InitServer()
    {
        //创建套接字
        _sockfd=socket(AF_INET,SOCK_STREAM,0);//TCP
        if(_sockfd<0)
        {
            LOG(LogLevel::FATAL)<<"create socket error";
            exit(SOCKET_ERR);
        }
        LOG(LogLevel::DEBUG)<<"create socket success: "<<_sockfd;
        struct sockaddr_in local;
        memset(&local,0,sizeof(local));
        local.sin_family=AF_INET;
        local.sin_port=htons(_port);
        local.sin_addr.s_addr=INADDR_ANY;

        //3.bind
        int n = (_sockfd,(struct socketaddr_in*)&local,sizeof(local));
        if(n<0)
        {
            LOG(LogLevel::FATAL)<<"bind error";
            exit(BIND_ERR);
        }
        LOG(LogLevel::DEBUG)<<"bind success";
        //4.tcp面向链接，所以TCP服务器要一直处于监听状态
        n = listen(_sockfd,gbacklog);
        if(n<0)
        {
            LOG(LogLevel::FATAL)<<"listen error";
            exit(LISTEN_ERR);
        }
        LOG(LogLevel::DEBUG)<<"listen success";
        
    }
    void Start(){
        while(true)   
        {
            //5.获取链接
                   
        }
    }
    ~TcpServer(){}
private:
    int _sockfd;
    uint16_t _port;
};