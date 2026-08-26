#include "ThraedPool.hpp"
#include "Logger.hpp"
#include <iostream>
#include <functional>
#include<ctime>
#include<stdlib.h>
#include <memory>

using namespace NS_THREAND_POOL;
using namespace NS_LOG_MODULE;
using task_t = std::function<void()>;
class Task
{
    public:
    Task(){}
    Task(int x,int y)
        :_x(x)
        ,_y(y)
    {}
    std::string Result()
    {
        return std::to_string(_x)+"+"+std::to_string(_y)+"="+std::to_string(_result);
    }
    void operator()()
    {
        _result=_x+_y;
    }
    ~Task(){}
    private:
    int _x;
    int _y;
    int _result;
};
int main()
{
    //创建对象的时机
    //单例模式，只允许在加载或者运行期间，整体最多创建一个该类对象
    //1. 加载到内存的时候，创建对象，int gval = 100;
    //2. 进程在运行期间，创建对象，int *val = (int*)malloc(sizeof(int));--最佳实践
    ENABLE_CONSOLE_LOG_STRATEGY();
    srand((long)time(nullptr)^getpid());

    std::unique_ptr<ThreadPool<Task>> tp = std::make_unique<ThreadPool<Task>>(); 

    tp->Start();

    int cnt = 10;
    while (cnt--)
    {
        int x = rand()%10+1;
        usleep(137);
        int y = rand()%20;
        Task t(x,y);
        tp->Enqueue(t);
        sleep(1);
        // tp->Enqueue([]()
        // {
        //     LOG(LogLevel::DEBUG) << "我是一个任务正在被处理";
        // });
    }

    //sleep(5);

    tp->Stop();
    tp->Wait();

    return 0;
}