#include "Socket.hpp"
#include "Logger.hpp"
#include "InetAddr.hpp"
#include <vector>
#include <memory>
#include <unistd.h>
#include <signal.h>

static uint16_t gport = 8080;
using namespace NS_SOCKET_MODULE;
class TcpServer
{
public:
    TcpServer(uint16_t port = gport)
        : _port(port), _listensock(std::make_unique<TcpSocket>()) // 基类指针指向子类对象
    {
        _listensock->BuildTcpSocketMethod(_port); // 使用模板方法
    }

    void Loop()
    {
        signal(SIGCHLD, SIG_IGN);
        while (true)
        {
            // 获取新链接也就是获取套接字
            InetAddr clientaddr; // 获取一个客户端地址
            auto sockfd = _listensock->Accepter(clientaddr);
            if (!sockfd)
                continue;
            _clients.push_back(sockfd);
            LOG(LogLevel::DEBUG) << "get a new link,socket address: " << clientaddr.Tostring() << " sockfd: " << sockfd->Sockfd();
            // 得到一个新地址并打印出来,并看到文件描述符
            // 修改为多进程版本
            if (fork() == 0)
            {
                // 证明其就是子进程
                // child,子进程完成任务，父进程读都不读
                // 让Tcp服务器读取数
                service(sockfd, clientaddr);
                sockfd->Close();
                exit(0);
            }
        }
    }
    ~TcpServer()
    {
    }

private:
    void service(std::shared_ptr<Socket> sockfd, InetAddr &clientaddr)
    {
        std::string inbuffer;
        // 短服务:你怎么知道怎么保证你读到的是一个完整报文--自定义协议
        while (true)
        {
            sockfd->Recv(&inbuffer);
            std::cout<<inbuffer<<std::endl;
            sockfd->Send(inbuffer);
        }
    }
private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensock;
    std::vector<std::shared_ptr<Socket>> _clients;
};