#pragma once
#include <iostream>
#include "Logger.hpp"
#include <vector>
#include <queue>
#include "Thread.hpp"
#include "Mutex.hpp"
#include "Cond.hpp"

namespace NS_THREAND_POOL
{
    using namespace NS_LOG_MODULE;
    using namespace NS_ThREAD_MODULE;

    const int defaultnum = 5;

    void Test()
    {
        char name[128];
        pthread_getname_np(pthread_self(), name, sizeof(name));
        while (true)
        {
            LOG(LogLevel::DEBUG) << "我是一个线程，我要运行:" << name;
            sleep(1);
        }
    }
    // 线程池要不要对多个线程进行管理？需要
    // 先描述，再组织

    template <typename T>
    class ThreadPool
    {
    private:
        // static void HanlderTask()一旦static就无法访问类内的成员属性了
        void HanlderTask()
        {
            char name[128];
            pthread_getname_np(pthread_self(), name, sizeof(name));

            while (true)
            {
                T task;
                {

                    // 1. 临界区：加锁
                    grouplock lock(_mutex); // RAII 加锁
                    // 2. 等待任务
                    //不休眠：1.队列不为空2.
                    {
                    while (_tasks.empty()&&_isrunnig)//既不退出，也不为空，就退出
                    {
                        _slaver_sleeper_count++;
                        _cond.Wait(_mutex); // 伪唤醒
                        _slaver_sleeper_count--;

                        // 如果线程池停止且队列为空，退出
                        if (!_isrunnig && _tasks.empty())
                        {
                            LOG(LogLevel::INFO) << name << " 优雅退出";
                            return;
                        }
                    }
                    //走到这里：有任务/线程池退出
                    //线程池退出，break?不能
                    //1.线程池退出&&任务队列为空
                    if(!_isrunnig&&_tasks.empty())
                    {
                        _mutex.unlock();//要先解锁再完成
                        break;//直接退出每解锁
                    }
                }

                    // 3. 取出任务（临界区内）
                    // 有任务怎么办，1.取任务：把任务从任务队列写到临时变量里，将任务由公共变为私有
                    // 拷贝指针T-> task*可以将其压入队列
                    task = _tasks.front();
                    _tasks.pop();

                } // 4. 离开作用域，自动解锁（grouplock 析构）

                // 处理任务需要在临界区内进行处理吗？不需要
                // 5. 处理任务（临界区外，安全且高效）
                LOG(LogLevel::INFO) << name << "处理任务";
                task(); // 线程中的线程关心
                LOG(LogLevel::DEBUG) <<task.Result() << "正在运行";
                // sleep(5);
            }
            //线程脱出
            LOG(LogLevel::INFO) <<name << "quit";
        }

    public:
        ThreadPool(int slaver_num = defaultnum)
            : _isrunnig(false), _slaver_sleeper_count(0), _slaver_num(slaver_num)
        {
            // ThreadPool对象已经存在了
            for (int idx = 0; idx < _slaver_num; idx++)
            {
                // auto f = std::bind(&ThreadPool::HanlderTask,std::__placeholders::_1);//绑定多个参数
                // auto f = std::bind(&ThreadPool::HanlderTask, nullptr); // 绑定多个参数
                // auto f = [this]{
                //     this->HanlderTask();
                // }//等价于
                auto f = std::bind(&ThreadPool::HanlderTask, this);
                _slavers.emplace_back(f);
                // _slavers.emplace_back([this](){
                //     this->HanlderTask();
                // });//构建线程对象
                // //无参传参解耦做法，直接调用类内的方法
            }
        }

        void Start()
        {
            if (_isrunnig)
            {
                LOG(LogLevel::WARNING) << "Thread Pool Is Already Running";
                return;
            }
            _isrunnig = true;
            for (auto &slave : _slavers)
            {
                slave.Start(); // 逐步启动线程
            }
        }

        void Stop()
        {
            // if (!_isrunnig)
            // {
            //     LOG(LogLevel::WARNING) << "Thread Pool Is Not Running";
            //     return;
            // }

            // _isrunnig = false;  // 先标记停止
            // _cond.Boradcast();  // 唤醒所有等待的线程，让它们优雅退出

            // for (auto &slave : _slavers)
            // {
            //     slave.Die(); // 逐步启动线程
            // }

            //修改这个简单粗暴的退出逻辑
            //1.退出条件：_isrunning = false
            //2.需要处理完成所有的任务
            //3.线程状态：休眠、正在处理任务->让所有的线程全部唤醒
            //HandlerTask自动break
            _mutex.lock();
            _isrunnig = false;//修改Hander
            if(_slaver_sleeper_count>0)
                _cond.Boradcast();
            _mutex.unlock();
        }

        void Wait()
        {
            for (auto &slave : _slavers)
            {
                slave.Join(); // 逐步启动线程
            }
        }

        void Enqueue(const T &task)
        {
            grouplock lock(_mutex);
            _tasks.push(task);
            if (_slaver_sleeper_count > 0)
                // 唤醒一个等待的线程
                _cond.Signal();
        }

        T Pop()
        {
            grouplock lock(_mutex);
            while (_tasks.empty())
            {
                _slaver_sleeper_count++;
                _cond.Wait(_mutex);
                _slaver_sleeper_count--;
            }
            T task = _tasks.front();
            _tasks.pop();
            return task;
        }

        ~ThreadPool()
        {
            if (_isrunnig)
            {
                Stop();
                Wait();
            }
        }

    private:
        // 线程怎么来？我们用之前封装的
        // 选择数据结构对线程进行管理
        bool _isrunnig;
        int _slaver_num;              // 多少个线程
        std::vector<Thread> _slavers; // 线程数组
        std::queue<T> _tasks;         // 任务队列（临界资源）
        Mutex _mutex;                 // 保护任务队列的锁
        Cond _cond;                   // 条件变量
        int _slaver_sleeper_count;    // 有多少slaver正在休眠
    };
}