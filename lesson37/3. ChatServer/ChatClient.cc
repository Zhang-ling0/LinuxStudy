#include <iostream>
#include<string>
#include<cstdlib>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include<cstring>
#include"Thread.hpp"
#include "InetAddr.hpp"
using namespace NS_ThREAD_MODULE;
int sockfd=0;
std::string server_ip;
uint16_t server_port = 0;
std::string nikename;
//./client_udp server_ip server_port
static void Usage(std::string proc)
{
    std::cout<<"Usage:\n\t";
    std::cout<<"Usage: "<<proc<<" server_ip server_port"<<std::endl;
}
//我怎么知道server的IP和port呢？类似IP+Port是被内置到client的。通信之前就知道四元组了
void RecvMessage(){
    while (true){
            //recvfrom
            char inbuffer[1024]={0};
            struct sockaddr_in temp;
            socklen_t len = sizeof(temp);
            ssize_t m = recvfrom(sockfd,inbuffer,sizeof(inbuffer),0,(struct sockaddr*)&temp,&len);
            if(m>0)
            {
                inbuffer[m] = '\0';
                std::cout<<"recv data: "<<inbuffer<<std::endl;
            }
            std::cerr<<"send data success!"<<std::endl;//2.
        }
    }
    

void SendMessage(){
    InetAddr serveraddr(server_port,server_ip);
    std::cout<<"Pleace Set Your Nick Name# ";
    std::getline(std::cin,nikename);
    while(true)
    {
        //1.获取用户输入
        std::string massage;
        std::cout<<"Please Enter# ";//1.
        std::getline(std::cin,massage);
        //2.客户端发送数据给服务器，首次发送自动绑定，不绑定发不出去
        ssize_t n= sendto(sockfd,massage.c_str(),massage.size(),0,(struct sockaddr*)&serveraddr,serveraddr.len());     
    }
}
int main(int argc,char* argv[])
{
    if(argc!=3)
    {
        Usage(argv[0]);
        exit(1);
    }
    server_ip = argv[1];
    server_port= atoi(argv[2]);
    //1.创建套接字
    sockfd = socket(AF_INET,SOCK_DGRAM,0);//创建套接字
    if(sockfd<0)
    {
        std::cerr<<"create socket error"<<std::endl;
        exit(2);
    }
    Thread recver(RecvMessage);
    Thread sender(SendMessage);
    recver.Start();
    sender.Start();
    recver.Join();//初始化并启动线程
    sender.Join();
    return 0;
}