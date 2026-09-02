#include <iostream>
#include<string>
#include<cstdlib>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include<cstring>
//./client_udp server_ip server_port
static void Usage(std::string proc)
{
    std::cout<<"Usage:\n\t";
    std::cout<<"Usage: "<<proc<<" server_ip server_port"<<std::endl;
}
//我怎么知道server的IP和port呢？类似IP+Port是被内置到client的。通信之前就知道四元组了

int main(int argc,char* argv[])
{
    if(argc!=3)
    {
        Usage(argv[0]);
        exit(1);
    }
    std::string server_ip = argv[1];
    uint16_t port = atoi(argv[2]);
    //1.创建套接字
    int sockfd = socket(AF_INET,SOCK_DGRAM,0);//创建套接字
    if(sockfd<0)
    {
        std::cerr<<"create socket error"<<std::endl;
        exit(2);
    }
    //2. client需要有自己的IP和端口吗？需要啊，你还要给别人的
    //需要显示的bind自己的IP和端口吗？不要显示bind
    //1. 为什么不让client显示bind自己的IP和端口？你要传参，你就要填参数，站在通信的角度一个服务端会有多个客户端
    //客户端的端口号是几实际上不重要，只要是唯一的就行，一个端口只能被一个进程绑定，client bind port可能会出现冲突
    //2. 客户端一般会采用随机端口的方式，系统会自动分配一个临时的端口号给客户端使用。有OS自主选择
    //udp client首次发送数据的时候，OS底层会隐式自动bind自己的IP和端口
    //服务端如果采用随机端口就不会被客户端找到。服务端的端口号会不会冲突？公司里服务器是自己的，好商量

    //2.构建服务端socket数据
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(server_ip.c_str());
    while(true)
    {
        //1.获取用户输入
        std::string massage;
        std::cout<<"Please Enter# ";
        std::getline(std::cin,massage);
        //2.客户端发送数据给服务器，首次发送自动绑定，不绑定发不出去
        ssize_t n= sendto(sockfd,massage.c_str(),massage.size(),0,(struct sockaddr*)&server,sizeof(server));
        if(n>0)
        {
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
            std::cout<<"send data success!"<<std::endl;
        }
        else
        {
            std::cerr<<"send data error!"<<std::endl;
            continue;
        }   
    }


    return 0;
}