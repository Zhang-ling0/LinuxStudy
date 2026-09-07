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
// #include <signal.h>

using namespace NS_LOG_MODULE;

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

    void serviceIO(int sockfd, InetAddr address)
    {
        LOG(LogLevel::DEBUG) << "client info is: " << address.Tostring();

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
        }
        else
        {
            LOG(LogLevel::ERROR) << "client read error, address: " << address.Tostring();
        }
        // LOG(LogLevel::DEBUG) << "client info is: " << address.Tostring();

        // while (true)
        // {
        //     char inbuffer[1024] = {0};

        //     // 读
        //     ssize_t n = read(sockfd, inbuffer, sizeof(inbuffer) - 1);

        //     if (n > 0)
        //     {
        //         inbuffer[n] = '\0';
        //         LOG(LogLevel::INFO) << address.Tostring() << " say# " << inbuffer;

        //         // 回显
        //         std::string echo_string = "server echo# ";
        //         echo_string += inbuffer;
        //         write(sockfd, echo_string.c_str(), echo_string.size());
        //     }
        //     else if (n == 0)
        //     {
        //         LOG(LogLevel::INFO) << "client quit, address: " << address.Tostring();
        //         break;
        //     }
        //     else
        //     {
        //         LOG(LogLevel::ERROR) << "client read error, address: " << address.Tostring();
        //         break;
        //     }
        // }

        close(sockfd);
    }
    class ThreadData
    {
    public:
        ThreadData(TcpServer *ts, int sockfd, InetAddr &addr)
            : _this(ts), sockfd(sockfd), _addr(addr) {};
        ~ThreadData()
        {
            close(sockfd);
        };

    public:
        TcpServer *_this;
        int sockfd;
        InetAddr _addr;
    };
    static void *thread_routine(void *args) // static避免this指针的出现而导致的参数传递的错误
    {
        ThreadData *td = static_cast<ThreadData *>(args);
        pthread_detach(pthread_self());
        td->_this->serviceIO(td->sockfd, td->_addr);
        delete td;
        return nullptr;
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

            // version3 多线程:多线程属于长服务，意味着只要开始服务就要一直服务知道不想要
            //  长服务对应的一定是长链接--只能处理中小形应用（大量的建立链接，创建线程，你的服务器容易挂，怎加CPU的负载）
            //  --多路转接技术
            pthread_t tid;
            InetAddr clientaddress(clientaddr);
            ThreadData *td = new ThreadData(this, sockfd, clientaddress); // 主动的将this指针和文件描述符传递进去
            pthread_create(&tid, nullptr, thread_routine, (void *)td);    // 创建线程
            // pthread_join();//你这里等待不久又阻塞了吗？
            // 让线程自己把自己设计为分离状态

            // 创建线程不会给线程单独拷贝文件描述符表，所有的线程都会共享文件描述符表

            // version2 多进程版本处理新的套接字
            //  pid_t id=fork();
            //  if(id<0)
            //  {
            //      LOG(LogLevel::FATAL)<<"fork error!";
            //      exit(FORK_ERR);
            //  }
            //  else if(id==0)//父进程专门获取新连接，子进程专门获取IO,多进程的服务器操作
            //  {
            //      close(_listensockfd);//防止子进程误操作
            //      //子进程
            //      if(fork()>0) {exit(0);}//子进程直接退出
            //      //孙子进程在执行--孙子变成孤儿被系统领养，死活和爷爷没有关系
            //      //服务器就被给OS了
            //      InetAddr clientaddress(clientaddr);
            //      serviceIO(sockfd, clientaddress);
            //      close(sockfd);//子进程退出，父进程不久僵尸了吗？需要等一下,那么逻辑不久又变成单一的了吗？
            //      exit(0);
            //  }
            //  else
            //  {
            //      //父进程
            //      //wait(id);
            //      //waitpid:WNOHANG非阻塞等
            //      //一个资源你不用来了，一定要尽早释放，文件描述符本身也是资源--关闭父子不需要使用的sorkfd
            //      close(sockfd);
            //      pid_t rid=waitpid(id,nullptr,0);
            //      (void)rid;

            // }
            // 多进程服务器是没有问题的

            // version1
            //  InetAddr clientaddress(clientaddr);
            //  serviceIO(sockfd, clientaddress);

            // serviceIO 内部会 close(sockfd)
        }
    }

private:
    int _listensockfd;
    uint16_t _port;
};