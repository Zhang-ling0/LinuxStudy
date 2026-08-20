#include "BlockQueue.hpp"
#include "Thread.hpp"
#include "memory"
#include <unistd.h>
#include "Task.hpp"
#include <ctime>
#include<stdlib.h>
int num = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;


int GetNumber()
{
    pthread_mutex_lock(&lock);
    int number = num++;
    pthread_mutex_unlock(&lock);
    return num;
}
using namespace ThreadModule;
int main()
{
    //std::bind
    srand(time(nullptr)^getpid());
    std::unique_ptr<BlockQueue<task_t>> bq = std::make_unique<BlockQueue<task_t>>(); // 智能指针
    Thread consumer([&bq]()
    {
        while (true)
        {
            sleep(1);
            task_t t;
            //1. 取数据--消费
            bq->Pop(&t);
            //2. 处理数据
            //t.Execute();
            t();//圆括号被重载了
           // std::cout  << " 消费" << t.getResult() << std::endl;
        } 
    });
    Thread productor([&bq]()
    {

        while (true)
        {
            //1. 获取数据或者数据
            int datax = rand()%10+1;
            usleep(rand()%1223);
            int datay = rand()%10+1;
            //2. 生产
            //Task t(datax,datay);
            bq->Enqueue(Print);
            //std::cout  << " 生产" << t.Question() << std::endl;
        }
    });
    consumer.Start();
    productor.Start();

    consumer.Join();
    productor.Join();

    return 0;
}