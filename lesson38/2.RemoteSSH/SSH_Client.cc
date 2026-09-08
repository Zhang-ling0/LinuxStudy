#include <iostream>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "InetAddr.hpp"

static void Usage(const std::string &name)
{
    std::cerr << "Usage::\n\t";
    std::cerr << name << "server_ip server_port" << std::endl;
}
//./client_tcp server_ip server_port
int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        Usage(argv[0]);
        exit(1);
    }
    std::string server_ip = argv[1];
    uint16_t server_port = std::stoi(argv[2]);
    // int sockfd=-1;
    // int ret=0;
    //  while (true)
    //  {
    // if(ret==0) break;
    // 1. 创建tcpsocket套接字
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        std::cerr << "socket error" << std::endl;
        exit(2);
        // goto WAIT;
    }

    // 2.要不要进行bind?需要
    // 要不要显示的bind本地IP和端口？不需要自己手动显示的进行bind,OS会自动bind的
    // OS帮助我们进行随机端口。防止端口号冲突
    // 3. 应该干什么？bind端口号ip,发起建立连接的工作connect
    //{client ip,client port}<->{server ip,server port}
    // 面向连接的通信方式，建立conect连接
    // 如何通过网络发送出去？connect:1.bind本地socket地址向2.server发起请求
    InetAddr serveraddress(server_port, server_ip);
    int n = connect(sockfd, (struct sockaddr *)serveraddress.GetNetAddress(), serveraddress.len());
    if (n < 0)
    {
        std::cerr << "connect to" << serveraddress.Tostring() << "failed!";
        exit(3); //?
        // goto WAIT;
    }
    // 连接成功
    std::cerr << "connect to" << serveraddress.Tostring() << "success!\n";
    // WAIT:
    //     sleep(1);
    //        cnt++;
    // }
    // 3. 通信
    while (true)
    {
        char prompt_buffer[1024] = {0};
        // 1. 接收服务端发来的提示符
        ssize_t n_read = read(sockfd, prompt_buffer, sizeof(prompt_buffer) - 1);

        if (n_read > 0)
        {
            // 正常收到提示符，直接打印（注意：提示符本身通常自带换行或不需要换行，根据服务端格式来）
            std::cout << prompt_buffer;
        }
        else if (n_read == 0)
        {
            // 服务端断开连接
            std::cout << "\nConnection closed by server." << std::endl;
            break;
        }
        else
        {
            // 读取错误
            std::cerr << "read prompt error!" << std::endl;
            break;
        }

        // 2. 用户在本地输入命令
        std::string command;
        std::getline(std::cin, command);

        // 发送命令给服务端（注意：加上 \n 以便服务端识别）
        std::string cmd_to_send = command + "\n";
        write(sockfd, cmd_to_send.c_str(), cmd_to_send.size());

        // 3. 接收服务端执行命令后的返回结果
        char result_buffer[4096] = {0}; // 增大缓冲区，防止命令输出过长被截断
        ssize_t n_result = read(sockfd, result_buffer, sizeof(result_buffer) - 1);

        if (n_result > 0)
        {
            std::cout << result_buffer; // 直接输出结果（popen的结果通常自带换行）
        }
        else if (n_result == 0)
        {
            std::cout << "\nread end of file" << std::endl;
            break;
        }
        else
        {
            std::cerr << "read result error!" << std::endl;
            break;
        }
    }

    return 0;
}