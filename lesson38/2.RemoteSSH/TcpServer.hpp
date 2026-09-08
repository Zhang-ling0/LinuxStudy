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
#include <sys/wait.h>
#include <pthread.h>
#include "ThreadPool.hpp"
#include <functional>
// #include <signal.h>

using namespace NS_LOG_MODULE;
using namespace NS_THREAD_POOL;
using task_t = std::function<void()>;
using handler_t = std::function<std::string(const std::string &)>;
using _HandlerTips_t = std::function<std::string()>;
// 参数是一串字符串命令，返回值是处理结果，回调函数

enum
{
    SUCCESS = 0,
    SOCKET_ERR,
    BIND_ERR,
    LISTEN_ERR,
    USAGE_ERR,
    FORK_ERR
};

static const int gbacklog = 16;
static const uint16_t gport = 8080;

class TcpServer
{
public:
    TcpServer(uint16_t port = gport)
        : _port(port), _listensockfd(-1)
    {
    }

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
        int n = bind(_listensockfd, (struct sockaddr *)&local, sizeof(local));
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

    void Register(handler_t handler, _HandlerTips_t HandlerTips)
    {
        _handler = handler;
        _HandlerTips = HandlerTips;
    }
    int HandlderIO(int sockfd, InetAddr address)
    {
        char inbuffer[1024] = {0};
        // 读
        //你怎么知道，对方给你发的是数据还是命令，归根结底还是字符串
        //这是我们约定出来的--TCP在读取的时候是有问题的
        ssize_t n = read(sockfd, inbuffer, sizeof(inbuffer) - 1);

        if (n > 0)
        {
            inbuffer[n] = '\0';
            // 回调处理
            std::string result;
            if (_handler != nullptr)
            {
                result = _handler(inbuffer);
            }
            else
            {
                LOG(LogLevel::WARNING) << "handler not registered!";
                result = "Error: handler not ready!";
            }
            // 回来:读命令->回来
            write(sockfd, result.c_str(), result.size());
        }
        else if (n == 0)
        {
            LOG(LogLevel::INFO) << "client quit, address: " << address.Tostring();
        }
        else
        {
            LOG(LogLevel::ERROR) << "client read error, address: " << address.Tostring();
        }
        return n;
    }
    void ReturnTips(int sockfd, InetAddr address)
    {
        std::string result = _HandlerTips(); // 获取提示信息
        write(sockfd, result.c_str(), result.size());
    }

    void service(int sockfd, InetAddr address)
    {

        LOG(LogLevel::DEBUG) << "client info is: " << address.Tostring();

        while (true)
        {
            // 给客户端写回命令行的的过程
            ReturnTips(sockfd, address);
            if(HandlderIO(sockfd, address)<=0);
                break;
        }
        close(sockfd);
    }

    void Start()
    {
        while (true)
        {
            // signal(SIGCHLD,SIG_IGN);//最佳实践
            //  5. 获取连接
            struct sockaddr_in clientaddr;
            socklen_t len = sizeof(clientaddr);
            int sockfd = accept(_listensockfd, (struct sockaddr *)&clientaddr, &len);

            if (sockfd < 0)
            {
                LOG(LogLevel::WARNING) << "accept error!";
                continue;
            }

            LOG(LogLevel::DEBUG) << "accept success, sockfd: " << sockfd;

            // 6. 处理新连接
            // version4:创建线程的成本也是比较高的，我们可以直接接入线程池，直接使用
            InetAddr clientaddress(clientaddr);
            ThreadPool<task_t>::Instance()->Enqueue([this, sockfd, clientaddress]()
                                                    { this->service(sockfd, clientaddress); });
        }
    }

private:
    int _listensockfd;
    uint16_t _port;
    handler_t _handler;
    _HandlerTips_t _HandlerTips;
};