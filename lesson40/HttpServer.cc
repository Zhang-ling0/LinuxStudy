#include "TcpServer.hpp"
#include"Http.hpp"//协议处理层


#include <memory>

static void Usage(const std::string &proc)
{
    std::cout << "Usage::\n\t" << proc << " port" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(1);
    }
    uint16_t port = std::stoi(argv[1]);

    // 0. 定义计算器
   

    // 1. 定义Http协议
    auto protocol = std::make_shared<HttpProtocol>();

    // 2. 定义网络对象
    auto tsvr = std::make_unique<TcpServer>(
        [&protocol](std::string &inbuffer) -> std::string
        {
            return protocol->HandlerHttpRequest(inbuffer);
        },port);

    // 启动
    tsvr->Loop();
    return 0;
}