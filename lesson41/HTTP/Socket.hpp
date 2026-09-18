#pragma once
#include <iostream>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "InetAddr.hpp"
#include "Logger.hpp"
namespace NS_SOCKET_MODULE
{
    // 模板方法模式，比较适合于解决实现方式上差异比较大，但是具体完成是将方法组合起来，具有非常强的套路性质的问题
    // 差异化由底层实现，我们所调用的接口是固定的
    using namespace NS_LOG_MODULE;
    static const int gbacklog = 16;
    enum
    {
        OK = 0,
        SOCKET_ERR,
        BIND_ERR,
        LISTEN_ERR
    };
    class Socket
    {
    public:
        ~Socket()
        {
        }

    protected:
        virtual void CreateSocketOrDie() = 0; // 纯虚接口
        virtual void BindSocketOrDie() = 0;
        virtual void ListenSocketOrDie() = 0;
        uint16_t GetPort() const { return _port; } 
        // virtual ssize_t Recv()=0;//读写接口
        // virtual ssize_t Send()=0;
    public:
        virtual std::shared_ptr<Socket> Accepter(InetAddr &addr) = 0;
        virtual int Sockfd() = 0;
        virtual int Recv(std::string *out)=0;
        virtual int Send(const std::string &in)=0;
        virtual void Close()=0;
        virtual bool Connect(InetAddr &addr) = 0;
        void BuildTcpSocketMethod(uint16_t port) // 套路化的固定方法放到父类中被子进程直接继承，这个叫做模板方法
        {
            _port = port;
            CreateSocketOrDie();
            BindSocketOrDie();
            ListenSocketOrDie();
        }
        void BuildTcpClientSockMethod()
        {
            CreateSocketOrDie();   
        }
        // void BuildUdpSocketMethod()
        // {
        //     CreateSocketOrDie();
        //     BindSocketOrDie();
        // }
    private:
        uint16_t _port = 0;
    };
    class TcpSocket : public Socket // 上层以统一的视角来管理网络接口
    {
    public:
        // 调用父类的方法
        TcpSocket()
            : _sockfd(0)
        {
        }
        TcpSocket(int sockfd)
            : _sockfd(sockfd)
        {
        }
        // void BuildTcpSocketMethod(uint16_t port) // 重写这个方法
        // {
        //     _port = port; // 设置 TcpSocket 的 _port
        //     CreateSocketOrDie();
        //     BindSocketOrDie();
        //     ListenSocketOrDie();
        // }
        void CreateSocketOrDie() override
        {
            _sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (_sockfd < 0)
            {
                LOG(LogLevel::FATAL) << "create socket error";
                exit(SOCKET_ERR);
            }

            //地址复用
            int opt =1;
            setsockopt(_sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
        }
        void BindSocketOrDie() override
        {
            // 告诉绑定的端口号
            InetAddr addr(GetPort());
            if (bind(_sockfd, addr.NetAddress(), addr.len()) != 0)
            {
                // 绑定失败
                LOG(LogLevel::FATAL) << "bind socket error";
                exit(BIND_ERR);
            }
        }
        void ListenSocketOrDie() override
        {

            if (listen(_sockfd, gbacklog) != 0)
            {
                LOG(LogLevel::FATAL) << "listen socket error";
                exit(LISTEN_ERR);
            }
        }
        std::shared_ptr<Socket> Accepter(InetAddr &clientaddr) override
        {
            struct sockaddr_in addr;
            socklen_t len = sizeof(addr);
            int sockfd = accept(_sockfd, CONV(&addr), &len);
            if (sockfd < 0)
            {
                LOG(LogLevel::WARNING) << "accept error";
                return nullptr;
            }
            clientaddr = addr; // 将地址带出来
            // 返回获取得套接字
            return std::make_shared<TcpSocket>(sockfd);
        }
        int Sockfd() override
        {
            return _sockfd;
        }
        int Recv(std::string *out)  override
        {
            //read和recv在使用上基本是完全一样的
            char inbuffer[1024];
            ssize_t n = recv(_sockfd,inbuffer,sizeof(inbuffer)-1,0);//认为这里读到的就是完整的HTTP请求
            if(n>0)
            {
                inbuffer[n]=0;
                *out = inbuffer;//追加写入到缓冲区里
            }
            return n;
        }
        int Send(const std::string &in)  override
        {
            return send(_sockfd,in.c_str(),in.size(),0);
        }
        void Close() override
        {
            if(_sockfd>=0)
            {
                close(_sockfd);
                _sockfd = -1;//关掉套接字
            }
        }
        bool Connect(InetAddr &addr) override
        {
            int n = connect(_sockfd,addr.NetAddress(),addr.len());
            if(n<0) return false;
            else return true;
        }
        
        ~TcpSocket()
        {
        }

    private:
        int _sockfd;
        
    };
    // class UdpSocket : public Socket
    // {
    // };

}