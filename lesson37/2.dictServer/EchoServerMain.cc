#include"EchoServer.hpp"
#include <memory>
#include"ThreadPool.hpp"
#include"Dict.hpp"


//./server_udp ip port传递端口参数
static void Usage(const std::string &process)//定义程序名
{
    std::cerr<<"Usage::\n\t";//换行缩进
    std::cerr<<process<<"local_ip local_port"<<std::endl;
}
//命令行参数传入程序名、端口号
int main(int argc,char* argv[])
{
    if(argc != 2)   // 只需要端口号
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    ENABLE_CONSOLE_LOG_STRATEGY();
    //1.定义字典
    Dict dict;
    //2. 构建网络服务，处理IO问题
    uint16_t server_port = std::stoi(argv[1]);
    //3. 绑定上下两层
    std::unique_ptr<UdpServer> usvr = std::make_unique<UdpServer>([&dict](std::string word){
        return dict.Translate(word);    

    },server_port);//回调匿名函数，匿名函数里面有我们的翻译
    usvr->Init();
    usvr->Start();
    return 0;

}
