#include<iostream>
#include<cstdlib>
#include<string>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include"InetAddr.hpp"

static void Usage(const std::string &name)
{
    std::cerr<<"Usage::\n\t";
    std::cerr<<name<<"server_ip server_port"<<std::endl;
}
//./client_tcp server_ip server_port
int main(int argc,char *argv[]){
    if(argc !=3)
    {
        Usage(argv[0]);
        exit(1);
    }
    std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[2]);
    //int sockfd=-1;
    //int ret=0;
    // while (true)
    // {
    //if(ret==0) break;
    //1. 创建tcpsocket套接字
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0)
    {
        std::cerr << "socket error"<<std::endl;
        exit(2);
        //goto WAIT;
    }

    //2.要不要进行bind?需要
    //要不要显示的bind本地IP和端口？不需要自己手动显示的进行bind,OS会自动bind的
    //OS帮助我们进行随机端口。防止端口号冲突
    //3. 应该干什么？bind端口号ip,发起建立连接的工作connect
    //{client ip,client port}<->{server ip,server port}
    //面向连接的通信方式，建立conect连接
    //如何通过网络发送出去？connect:1.bind本地socket地址向2.server发起请求
    InetAddr serveraddress(server_port,server_ip);
    int n = connect(sockfd,(struct sockaddr*)serveraddress.GetNetAddress(),serveraddress.len());
    if(n<0)
    {
        std::cerr<<"connect to"<<serveraddress.Tostring()<<"failed!";
        exit(3);//?
        //goto WAIT;  
    }
    //连接成功
    std::cerr<<"connect to"<<serveraddress.Tostring()<<"success!\n";
// WAIT:
//     sleep(1);
//        cnt++;
// }
    //3.通信
    while(true)
    {
        std::string line;
        std::cout<<"please Enter# ";
        std::getline(std::cin,line);
        write(sockfd,line.c_str(),line.size());
        char inbuffer[1024];
        ssize_t n=read(sockfd,inbuffer,sizeof(inbuffer));
        if(n>0)
        {
            inbuffer[n]=0;
            std::cout<<inbuffer<<std::endl;
        }
        else if(n==0)
        {
            std::cout<<"read end of file"<<std::endl;
            break;
        }
        else
        {
            std::cerr<<"read error!"<<std::endl;
        }

    }

    return 0;
}