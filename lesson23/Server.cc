#include "Pipe.hpp"
int main()
{
    // 创建文件，并将文件打开
    Fifo pipefile;
    pipefile.Build();
    pipefile.Open(ForRead);
    std::string msg;

    while (true) // 接收文件
    {
        int n = pipefile.Recv(&msg);
        if (n > 0)
            std::cout << "Client Say#" << msg << std::endl;
        else
            break;
    }
    // 读取文件之后需要将文件删除
    pipefile.Delete();
    return 0;
}