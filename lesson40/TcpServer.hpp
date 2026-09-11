#include "Socket.hpp"
#include "Logger.hpp"
#include "InetAddr.hpp"
#include <vector>
#include <memory>
#include <unistd.h>
#include <signal.h>
#include <functional>

static uint16_t gport = 8080;
using namespace NS_SOCKET_MODULE;
using Handler_t = std::function<std::string(std::string &)>;
class TcpServer
{
public:
    TcpServer(Handler_t handler, int port = gport)
        : _port(port), _listensock(std::make_unique<TcpSocket>()) // 基类指针指向子类对象
          ,
          _handler(handler)
    {
        _listensock->BuildTcpSocketMethod(_port); // 使用模板方法
        LOG(LogLevel::INFO) << "create listen socket success: " << _listensock->Sockfd();
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
//短服务
    void service(std::shared_ptr<Socket> sockfd, InetAddr &clientaddr)
    {
        std::string inbuffer; // 持久缓冲区，累积数据
                              // 短服务:你怎么知道怎么保证你读到的是一个完整报文--自定义协议

        int n = sockfd->Recv(&inbuffer);
        if (n <= 0)
        {
            LOG(LogLevel::WARNING) << "recv:client quit, " << clientaddr.Tostring();
            return;
        }
        LOG(LogLevel::DEBUG) << "inbuffer:\n " << inbuffer;

        // 处理数据（Protocol 会自动 erase 已处理的报文）
        std::string outbuffer;
        if (_handler) // 读取数据给上层
            outbuffer = _handler(inbuffer);

        // 没有完整报文，继续接收
        if (outbuffer.empty())
        {
            LOG(LogLevel::DEBUG) << "no complete packet, continue recv";
            return;
        }

        LOG(LogLevel::DEBUG) << "outbuffer:\n " << outbuffer;

        // ✅ 发送响应（outbuffer）
        ssize_t s = sockfd->Send(outbuffer);
        if (s < 0)
        {
            LOG(LogLevel::WARNING) << "snd:client quit, " << clientaddr.Tostring(); // IO部分

            LOG(LogLevel::DEBUG) << "sent " << s << " bytes";
        }
    }

private:
    uint16_t _port;
    std::unique_ptr<Socket> _listensock;
    std::vector<std::shared_ptr<Socket>> _clients;
    Handler_t _handler;
};