#include"EchoServer.hpp"
#include <memory>
//./server_udp ip port传递端口参数
static void Usage(const std::string &process)//定义程序名
{
    std::cerr<<"Usage::\n\t";//换行缩进
    std::cerr<<process<<"local_ip local_port"<<std::endl;
}
//命令行参数传入程序名、端口号
int main(int argc,char* argv[])
{
    if(argc != 2)   // 只需要端口号
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    ENABLE_CONSOLE_LOG_STRATEGY();
    uint16_t server_port = std::stoi(argv[1]);
    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>(server_port);
    usvr->Init();
    usvr->Start();
    return 0;

}
