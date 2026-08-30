#include"EchoServer.hpp"
#include <memory>
//./server_udp ip port传递端口参数
static void Usage(const std::string &process)
{
    std::cerr<<"Usage::\n\t";//换行回车
    std::cerr<<process<<"local_ip local_port";
}
int main(int argc,char* argv[])
{
    if(argc !=3)
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    ENABLE_CONSOLE_LOG_STRATEGY();//开启日志的显示器策略
    std::string server_ip = argv[1];
    uint16_t server_port =std::stoi(argv[2]);//将端口号转化为整数
    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>(server_ip,server_port);
    usvr->Init();
    usvr->Start();
    return 0;
}