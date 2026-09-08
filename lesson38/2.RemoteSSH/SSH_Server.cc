#include "TcpServer.hpp"
#include <memory>
#include <iostream>
#include "Command.hpp"
#include <string>

static void Usage(const std::string &process)
{
    std::cerr << "Usage:\n\t";
    std::cerr << process << " local_port" << std::endl;
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }

    ENABLE_CONSOLE_LOG_STRATEGY();
    uint16_t server_port = std::stoi(argv[1]);

    // 命令处理的类对象
    std::unique_ptr<Command> command = std::make_unique<Command>();
    // 网络通信的类
    std::unique_ptr<TcpServer> tsvr = std::make_unique<TcpServer>(server_port);

    // 1. 先注册好处理命令的回调函数（绑定业务逻辑）
    tsvr->Register([&command](std::string cmd) -> std::string
                    { return command->Execute(cmd); },
                    [&command]() ->std::string {
                    return command->GetCommandString();});
    //为服务器注册两个服务1.执行命令行2.返回你获得的命令
    // 2. 初始化服务器（此时才开始 listen，对外接受连接，保证连接进来时 handler 已经存在）
    tsvr->InitServer();

    // 3. 开始进入阻塞循环，接受并处理连接
    tsvr->Start();

    return 0;
}