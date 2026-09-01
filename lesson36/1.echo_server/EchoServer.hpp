#ifndef _ECHOSERVER_HPP
#define _ECHOSERVER_HPP

#include <iostream>
#include <stdlib.h>
#include<string>
#include <sys/socket.h>
#include "Logger.hpp"
#include "Mutex.hpp"
#include<netinet/in.h>//关键函数类型
#include <strings.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<cstring>

using namespace NS_LOG_MODULE;

const static int default_fd = -1;
const static int default_port = 8888;

enum {
    SUCCESS = 0,
    SOCKET_ERR,
    USAGE_ERR,
    BIND_ERR
};

class UdpServer
{
public:
    //UdpServer(const std::string &ip,uint16_t port=default_port)//为端口号设计一个缺省值
    UdpServer(uint16_t port=default_port)
    : _sockfd(default_fd)//网卡文件描述符
    //,_ip(ip)
    ,_port(port)
    {}
    ~UdpServer() 
    {
        if(_sockfd>0)
            close(_sockfd);//关闭文件描述符
    }

    void Init()
    {
        //第一步创建套接字，本质：也就是在打开网络文件（打开网卡这个设备）--具备系统特性
        _sockfd = socket(AF_INET, SOCK_DGRAM, 0);//打开网络文件
        //1. AF_INET:ipv4本地通信还是网络通信，限定作用域2.SOCK_DGRAM:UDP（用户数据报）使用TCP还是UDP也就是字符流式还是数据报方式
        //3. 0:默认协议 协议名称
        if (_sockfd < 0)//服务器不可能运行
        {
            LOG(LogLevel::FATAL) << "create socket error";
            exit(SOCKET_ERR);//终止进程
        }
        LOG(LogLevel::INFO) << "create socket success sockfd: " << _sockfd;
        //服务器的IP是多少，服务器的端口号是多少
        //第二步：填充网络信息,有没有把IP和端口信息设置到内核中？设置到你打开的网络文件内部
        //设置到你刚刚打开的网络socket对应的文件内部
        struct sockaddr_in local;//创建网络套接字类型
        //还没有struct——sockeadd_in实际上是一个数据类型，local这个变量是在用户栈上的，并没有设置到内核
        bzero(&local,sizeof(local));//将指定空间全部清零，填入我们的数据
        local.sin_family = AF_INET;//结构体子类的类型是一个网络套接字的类型，两层检查属于不同体系的检查
        //     local.sin_addr =_ip;//1.字符串风格的ip->4字节IP 2.hton主机序列转化为网络序列--函数接口
        //将字符串风格的IP地址及转化为整数，又转化为网络序列，返回给我
        //local.sin_addr.s_addr=inet_addr(_ip.c_str());//走网络
        //最佳实践
        local.sin_addr.s_addr=INADDR_ANY;//绑定本机所有网卡的IP地址，客户端可以通过任意网卡访问服务器
        //1. IP是字符串风格的IP,转整数（怎么做？实际上就是对比特位做读取，定义结构体指针直接指向，到时候直接用to_string拼接）
        //2. 主机序列转为网络序列
        local.sin_port = htons(_port);

        //第三步：将前两步信息合并，将网络和端口号信息合并，并设置到内核中绑定IP和端口号，上述的数据是在用户层，将数据设置到内核中
        //设置网络和端口也就是绑定，为套接字命名
        //发送报文要携带发送方的IP和端口号，接收报文要携带接收方的IP和端口号
        int n =bind(_sockfd,(struct sockaddr*)&local,sizeof(local));//哪个文件里的哪个结构（子类给父类要进行强制转化，父类指针指向子类对象）
        //这个结构有多长
        if(n<0)//绑定失败
        {
            LOG(LogLevel::FATAL)<<"bind socket error";
            exit(BIND_ERR);
        }
        LOG(LogLevel::INFO)<<",port:"<<_port;
    }

    void Start() {
        char inbuffer[1024];
        while(true)
        {
            struct sockaddr_in peer;//远端的套接字信息
            socklen_t len =sizeof(peer);//输入输出形式的数据，必须和你输入进来的大小相同，便于以同类型的形式输出
            //读和写，认为双方传递的是字符串
            //1. 用户信息本身
            //2. 用户信息的来源--对方的套接字信息，如何获取？

            ssize_t n = recvfrom(_sockfd,inbuffer,sizeof(inbuffer)-1,0,(struct sockaddr*)&peer,&len);//0阻塞读取
            if(n>0)//读取成功
            {
                //不建议将\0写到网络文件中
                inbuffer[n] = 0;//字符串结束符
                //你recvfrom读取到的数据是网络中获取的数据，大端的网络序列，你要将其转化为主机序列，才能够让人看懂
                // peer.sin_addr;//客户的IP
                // peer.sin_port;//客户的port
                uint16_t client_port = ntohs(peer.sin_port);//网络序列转化为主机序列
                std::string client_ip = inet_ntoa(peer.sin_addr);//网络序列转化为字符串风格的IP地址
                std::string client_address = "[" + client_ip + ":" + std::to_string(client_port) + "]";
                
                LOG(LogLevel::DEBUG) << "client address: " << client_address << ", say: " << inbuffer;
                std::string echo_string = "server echo# ";
                echo_string += inbuffer;
                //如何返回给客户端sendto，向指定的网络文件写入信息，成功返回写入字节数
                //1. 你要发什么数据
                //2. 你要发给谁--对方的套接字信息
                //3. 你想怎么发送--全双工，你给我发，我给你发，阻塞发送--通过文件描述符进行发送
                sendto(_sockfd,echo_string.c_str(),echo_string.size(),0,(struct sockaddr*)&peer,len);//直接发送，直接就是网络序列
            }
            else
            if(n<0)
            {
                LOG(LogLevel::FATAL)<<"error";//多个客户端，让它重新读取
            }
            
        }
    }

private:
    int _sockfd;
    //std::string _ip;//"192.168.2.2"(字符串风格的点分十进制IP地址，让人看的内核当中不会传递这个）&& 4字节IP（网络带宽比较少）
    uint16_t _port;//用户设置好的，Server端口号是唯一固定的
    //typedef了无符号短整型
};

#endif