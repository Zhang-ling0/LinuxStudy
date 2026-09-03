#include"UDPServer.hpp"//获取事件
#include"ThreadPool.hpp"//执行者，执行处理动作的人
#include"Route.hpp"//将任务路由，交给特定的人去处理
#include <memory>
//./server_udp ip port传递端口参数
using namespace NS_THREAND_POOL;
static void Usage(const std::string &process)//定义程序名
{
    std::cerr<<"Usage::\n\t";//换行缩进
    std::cerr<<process<<"local_ip local_port"<<std::endl;
}
using task_t =std::function<void()>;
//命令行参数传入程序名、端口号
int main(int argc,char* argv[])
{
    if(argc != 2)   // 只需要端口号
    {
        Usage(argv[0]);
        exit(USAGE_ERR);
    }
    ENABLE_CONSOLE_LOG_STRATEGY();
    uint16_t server_port = std::stoi(argv[1]);
    //构建一个任务类
    //线程池模块
    auto thread_pool = ThreadPool<task_t>::Instance();
    //路由模块
    Route r;
    //网络模块
    UdpServer usvr(server_port);
    usvr.Init();
    //执行注册服务
    usvr.RegisterService([&r](const InetAddr &addr){
        r.CheckUser(addr);
    },
        [&r,&thread_pool](int sokcfd,std::string msg){
        //r.Broadcast(sokcfd,msg);//将这个任务交给进程池来做
        task_t t = std::bind(&Route::Broadcast,&r,sokcfd,msg);//将参数绑定为一个任务
        thread_pool->Enqueue(t);//转发
    });//传递两个服务
    
    return 0;

}
