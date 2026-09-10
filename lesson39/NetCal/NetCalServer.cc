#include "TcpServer.hpp"
#include "Protocol.hpp"
#include "Calculator.hpp"
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
    auto cal = std::make_shared<Calculator>();

    // 1. 定义协议对象
    auto protocol = std::make_shared<Protocol>(
        [cal](Request &req) -> Response
        {
            return cal->Execute(req);
        });

    // 2. 定义网络对象
    auto tsvr = std::make_unique<TcpServer>(
        [protocol](std::string &inbuffer) -> std::string
        {
            return protocol->ParseRequest(inbuffer);
        },
        port);

    // 启动
    tsvr->Loop();
    return 0;
}