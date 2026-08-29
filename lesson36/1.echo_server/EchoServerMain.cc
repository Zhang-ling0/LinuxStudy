#include"EchoServer.hpp"
#include <memory>
int main()
{
    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>();
    usvr->Init();
    usvr->work();
}