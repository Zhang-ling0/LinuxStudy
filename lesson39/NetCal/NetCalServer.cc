#include"TcpServer.hpp"
#include"Protocol.hpp"
int main()
{
    TcpServer tsvr;
    tsvr.Loop();
    return 0;
}