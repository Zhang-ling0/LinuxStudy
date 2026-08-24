#include "ThraedPool.hpp"
#include "Logger.hpp"
#include <iostream>
#include <functional>
#include <memory>

using namespace NS_THREAND_POOL;
using namespace NS_LOG_MODULE;
using task_t = std::function<void()>;

int main()
{
    ENABLE_CONSOLE_LOG_STRATEGY();


    std::unique_ptr<ThreadPool<std::function<void()>>> tp = 
        std::make_unique<ThreadPool<std::function<void()>>>(5); 

    tp->Start();

    int cnt = 10;
    while (cnt--)
    {
        tp->Enqueue([]()
        {
            LOG(LogLevel::DEBUG) << "我是一个任务正在被处理";
        });
    }

    sleep(5);

    tp->Stop();
    tp->Wait();

    return 0;
}