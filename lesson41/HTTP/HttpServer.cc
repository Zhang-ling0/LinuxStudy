#include "TcpServer.hpp"
#include "Daemon.hpp"
#include"Http.hpp"//协议处理层


#include <memory>

static void Usage(const std::string &proc)
{
    std::cout << "Usage::\n\t" << proc << " port" << std::endl;
}
//使用HTTP 实现restful风格的服务接口
//GET/Login HTTP/1.1
//GET/exec HTTP/1.1 可以让浏览器为你实现Xshell 
//可不可以是另一个服务的客户端--可以的，可以实现一个简单的代理服务的功能    
void Login(HttpRequest& req,HttpResponse& resp)
{
    std::cout<<"\nLogin function been called!"<<std::endl;
    req.DebugPrint();//将服务器中的内容注册到这个服务里
    std::string data=req.Text();
    std::cout<<"data is: "<<data<<std::endl;
    //根据分割符将数据提取出来
    //访问数据库，查找数据库,确认数据是存在的，设置应答
    resp.SetCode(200);
    resp.AddHeader("Content-Type",".txt");
    //resp.SetBody("{\"statuscode\":\"0\"}");
    resp.SetBody("Login success!");
    resp.AddHeader("Content-Length",std::to_string(resp.BodySize()));
}
void Register(HttpRequest& req,HttpResponse& resp)
{
    //插入数据就变成了register
    
    
}
void GetProductList(HttpRequest& req,HttpResponse& resp)//服务会根据功能路由路由过来
{

}
void Search(HttpRequest& req,HttpResponse& resp)
{

}


int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        Usage(argv[0]);
        exit(1);
    }
    //Daemon(1,1);//进去的是父进程,不关闭目录重定向
    daemon(0,0);//使用系统接口，命名有点绕，和我上面的不一样，控制是否重定向，工作路径是否需要改变
    //ENABLE_FILE_LOG_STRATEGY();//使用显示器打印日志的策略
    ENABLE_CONSOLE_LOG_STRATEGY();
    uint16_t port = std::stoi(argv[1]);
    //放一个守护进程
    //LOG->让日志往文件里写--上面使用文件版本的日志策略
    //printf&&cout1.改代码 2. 保留->不写道显示器，写道dev/null
    // 1. 定义Http协议
    auto protocol = std::make_shared<HttpProtocol>();

    // 2. 定义网络对象
    auto tsvr = std::make_unique<TcpServer>(
        [&protocol](std::string &inbuffer) -> std::string
        {
            return protocol->HandlerHttpRequest(inbuffer);
        },port);
        //向Http中注册服务,
    protocol->RegisterService("/Login",Login);//注册服务
    protocol->RegisterService("/Register",Register);//注册服务
    protocol->RegisterService("/Search",Search);//注册服务
    protocol->RegisterService("/api/getproductlist",GetProductList);//注册服务

    // 启动
    tsvr->Loop();
    return 0;
}