#pragma once
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <cstring>
#include <unistd.h>
#include "Logger.hpp"
#include "InetAddr.hpp"
#include <cstring>
#include <errno.h>

using namespace NS_LOG_MODULE;

enum {
    SUCCESS = 0,
    SOCKET_ERR,
    BIND_ERR,
    LISTEN_ERR,
    USAGE_ERR
};

static const int gbacklog = 16;
static const uint16_t gport = 8080; 

class TcpServer
{
public:
    TcpServer(uint16_t port = gport)
        : _port(port)
        , _listensockfd(-1)
    {}
    
    ~TcpServer()
    {
        if (_listensockfd >= 0)
            close(_listensockfd);
    }

    void InitServer()
    {
        // 1. 创建套接字
        _listensockfd = socket(AF_INET, SOCK_STREAM, 0);
        if (_listensockfd < 0)
        {
            LOG(LogLevel::FATAL) << "create socket error";
            exit(SOCKET_ERR);
        }
        LOG(LogLevel::DEBUG) << "create socket success: " << _listensockfd;
        
        // 2. 填充本地地址信息
        struct sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        local.sin_addr.s_addr = INADDR_ANY;

        // 3. bind 
        int n = bind(_listensockfd, (struct sockaddr*)&local, sizeof(local));
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "bind error: " << strerror(errno);
            exit(BIND_ERR);
        }
        LOG(LogLevel::DEBUG) << "bind success";

        // 4. listen（TCP需要监听）
        n = listen(_listensockfd, gbacklog);
        if (n < 0)
        {
            LOG(LogLevel::FATAL) << "listen error: " << strerror(errno);
            exit(LISTEN_ERR);
        }
        LOG(LogLevel::DEBUG) << "listen success";
    }

    void serviceIO(int sockfd, InetAddr address)
    {
        LOG(LogLevel::DEBUG) << "client info is: " << address.Tostring();
        
        while (true)
        {
            char inbuffer[1024] = {0};
            
            // 读
            ssize_t n = read(sockfd, inbuffer, sizeof(inbuffer) - 1);
            
            if (n > 0)
            {
                inbuffer[n] = '\0';
                LOG(LogLevel::INFO) << address.Tostring() << " say# " << inbuffer;
                
                // 回显
                std::string echo_string = "server echo# ";
                echo_string += inbuffer;
                write(sockfd, echo_string.c_str(), echo_string.size());
            }
            else if (n == 0)
            {
                LOG(LogLevel::INFO) << "client quit, address: " << address.Tostring();
                break;
            }
            else
            {
                LOG(LogLevel::ERROR) << "client read error, address: " << address.Tostring();
                break;
            }
        }
        
        close(sockfd);  
    }

    void Start()
    {
        while (true)
        {
            // 5. 获取连接
            struct sockaddr_in clientaddr;
            socklen_t len = sizeof(clientaddr);
            int sockfd = accept(_listensockfd, (struct sockaddr*)&clientaddr, &len);
            
            if (sockfd < 0)
            {
                LOG(LogLevel::WARNING) << "accept error!";
                continue;
            }
            
            LOG(LogLevel::DEBUG) << "accept success, sockfd: " << sockfd;
            
            // 6. 处理新连接
            InetAddr clientaddress(clientaddr);
            serviceIO(sockfd, clientaddress);
            // serviceIO 内部会 close(sockfd)
        }
    }

private:
    int _listensockfd;
    uint16_t _port;
};