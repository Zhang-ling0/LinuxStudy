#include<iostream>
#include"Socket.hpp"
#include<memory>
#include"Protocol.hpp"
#include"InetAddr.hpp"
using namespace NS_LOG_MODULE;
using namespace NS_SOCKET_MODULE;
// ./netcal_client 目标IP 目标端口
static void Usage(const std::string &proc)
{
    std::cout<<"Usage::\n\t"<<proc<<"server_ip server_port"<<std::endl;
}
static void HandlderResponse(Response &resp)
{
    std::cout<<resp._result<<"["<<resp._code<<"]"<<std::endl;
}
int main(int argc,char* argv[])
{
    if(argc !=3)
    {
        Usage(argv[0]);
        exit(1);
    }
    std::string server_ip = argv[1];
    uint16_t server_port=std::stoi(argv[2]);

    std::unique_ptr<Socket> socket = std::make_unique<TcpSocket>();
    socket->BuildTcpClientSockMethod();

    InetAddr serveraddress(server_port,server_ip);
    bool n = socket->Connect(serveraddress);//将地址链接
    if(!n)
    {
        std::cerr<<"connect error"<<serveraddress.Tostring()<<std::endl;
        exit(2);
    }
    Protocol protocol(HandlderResponse);
    std::string inbuffer;
    //向服务器发起请求
    
    while(true)
    {
        int cnt = 3;
        std::string outbuffer;
        //0. 获取数据
        while(cnt--)
        {
        int x,y;
        char oper;
        std::cout<<"Enter Your x: ";
        std::cin>>x;
        std::cout<<"Enter Your y: ";
        std::cin>>y;
        std::cout<<"Enter Your oper: ";
        std::cin>>oper;

        //1. 定义结构体变量请求变量
        std::string inbuffer;
        Request req(x,y,oper);
        //2. 序列化
        std::string req_json;
        req.Serialize(&req_json);
        //3.封装报头
        std::string send_req_string = protocol.Packet(req_json);
        outbuffer += send_req_string;
        }
        std::cout<<"\n"<<outbuffer<<std::endl;
        //4.发送
        socket->Send(outbuffer);
        //5.接收数据
        socket ->Recv(&inbuffer);
        //6. 解析
        protocol.ParseResponse(inbuffer);//解析成功开始回调
    }
    // Protocol protocol(HandlderResponse);
    // std::string inbuffer;
    // //向服务器发起请求
    // while(true)
    // {
    //     //0. 获取数据
    //     int x,y;
    //     char oper;
    //     std::cout<<"Enter Your x: ";
    //     std::cin>>x;
    //     std::cout<<"Enter Your y: ";
    //     std::cin>>y;
    //     std::cout<<"Enter Your oper: ";
    //     std::cin>>oper;

    //     //1. 定义结构体变量请求变量
    //     std::string inbuffer;
    //     Request req(x,y,oper);
    //     //2. 序列化
    //     std::string req_json;
    //     req.Serialize(&req_json);
    //     //3.封装报头
    //     std::string send_req_string = protocol.Packet(req_json);
    //     //4.发送
    //     socket->Send(send_req_string);
    //     //5.接收数据
    //     socket ->Recv(&inbuffer);
    //     //6. 解析
    //     protocol.ParseResponse(inbuffer);//解析成功开始回调
    // }
    socket->Close();

    
    return 0;
}