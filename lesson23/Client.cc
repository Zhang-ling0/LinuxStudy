#include "Pipe.hpp"
// 客户端发送消息
int main()
{
    Fifo fileclient;
    fileclient.Open(ForWrite); // 将这个文件以写的方式打开
    while (true)
    {
        std::cout << "Please Enter@";
        std::string msg;
        std::getline(std::cin, msg);
        fileclient.Send(msg);
    }
    return 0;
}