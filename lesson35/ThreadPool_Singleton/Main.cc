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
    ENABLE_CONSOLE_LOG_STRATEGY();
    //万一有一个人创建了多个线程去访问该单例线池，也就是说线程池被很多线程所共同访问
    srand((long)time(nullptr)^getpid());

    // std::unique_ptr<ThreadPool<Task>> tp = std::make_unique<ThreadPool<Task>>(); 

    // tp->Start();不用构建和start

    int cnt = 10;
    while (cnt--)
    {
        int x = rand()%10+1;
        usleep(137);
        int y = rand()%20;
        //
        Task t(x,y);
        ThreadPool<Task>::Instance()->Enqueue(t);
        sleep(1);
    }

    //sleep(5);
    ThreadPool<Task>::Instance()->Stop();
    ThreadPool<Task>::Instance()->Wait();
    //只能通过入口函数才能获取，不能拷贝

    return 0;
}