#pragma once
#include<iostream>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include <cstring>
#include"Logger.hpp"
#include"InetAddr.hpp"
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
        _listensockfd=socket(AF_INET,SOCK_STREAM,0);//TCP
        if(_listensockfd<0)
        {
            LOG(LogLevel::FATAL)<<"create socket error";
            exit(SOCKET_ERR);
        }
        LOG(LogLevel::DEBUG)<<"create socket success: "<<_listensockfd;
        struct sockaddr_in local;
        memset(&local,0,sizeof(local));
        local.sin_family=AF_INET;
        local.sin_port=htons(_port);
        local.sin_addr.s_addr=INADDR_ANY;

        //3.bind
        int n = (_listensockfd,(struct socketaddr_in*)&local,sizeof(local));
        if(n<0)
        {
            LOG(LogLevel::FATAL)<<"bind error";
            exit(BIND_ERR);
        }
        LOG(LogLevel::DEBUG)<<"bind success";
        //4.tcp面向链接，所以TCP服务器要一直处于监听状态
        n = listen(_listensockfd,gbacklog);
        if(n<0)
        {
            LOG(LogLevel::FATAL)<<"listen error";
            exit(LISTEN_ERR);
        }
        LOG(LogLevel::DEBUG)<<"listen success";
        
    }
    void serviceIO(int sockfd,InetAddr address){
        //tcp全双工
        LOG(LogLevel::DEBUG)<<"client info is: "<<address.Tostring();
        while(true)
        {
            char inbuffer[1024]={0};
            //读
            size_t n = read(sockfd,inbuffer,sizeof(inbuffer)-1);
            //写
            if(n>0)
            {
                inbuffer[n]=0;
                LOG(LogLevel::INFO)<<address.Tostring()<<" say# "<<inbuffer;
                std::string echo_string ="server echo# ";
                echo_string += inbuffer;
                //写
                write(sockfd,echo_string.c_str(),echo_string.size());

            }
            else if(n==0)
            {
                //类似于管道
                LOG(LogLevel::INFO)<<"client quit,address: "<<address.Tostring();
                break;
            }
            else if(n<0)
            {
                //类似于管道
                LOG(LogLevel::ERROR)<<"client read error,address: "<<address.Tostring();
                break;
            }
        }

    }
    void Start(){
        while(true)   
        {
            //5.获取链接
            struct sockaddr_in clientaddr;
            socklen_t len =sizeof(clientaddr);
            int sockfd =accept(_listensockfd,(struct sockaddr*)&clientaddr,&len);
            if(sockfd<0)
            {
                //获取失败
                LOG(LogLevel::WARNING)<<"accept error!";
                continue;
            }
                LOG(LogLevel::DEBUG)<<"accept sucess,sockfd: "<<sockfd; 
                //6.处理新sockfd
                //version 0
                InetAddr clientaddress(clientaddr);
                serviceIO(sockfd,clientaddress);
                close(sockfd);//断开连接

        }
    }
    ~TcpServer(){}
private:
    int _listensockfd;
    uint16_t _port;
};