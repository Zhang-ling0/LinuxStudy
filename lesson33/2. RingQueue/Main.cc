#include "RingQueue.hpp"
#include "Task.hpp"
#include<unistd.h>
#include<ctime>
#include <stdlib.h> 
int data =1;//问题
Mutex screen_lock;
Mutex cnt_lock;
// int GetData()
// {
//     cnt_lock.lock();
//     int result = data++;
//     cnt_lock.unlock();
//     return result;

// }
// void  Print(const std::string &name,const std::string &info)
// {
//     screen_lock.lock();
//     std::cout<<name<<" ： "<< info << std::endl;
//     screen_lock.unlock();
// }
// class ThreadDate
// {
// public:
//     ThreadDate(RingQueue<int>* r,const std::string &n)
//         :rq(r)
//         ,name(n)
//         {}
//     std::string name;
//     RingQueue<int> *rq;
// };

void *ProductorRoutine(void *args)
{
    //ThreadDate *td = static_cast<ThreadDate*>(args);
    RingQueue<Task> *rq = static_cast<RingQueue<Task>*>(args);
    //pthread_setname_np(pthread_self(),td->name.c_str());
    //int data = 1;
    while(true)
    {
        // int data = GetData();
         sleep(3);
        // td->rq->Enqueue(data);
        // Print(td->name,"生成数据："+std::to_string(data));
        //1. 获取数据
        int x = rand()%10+1;
        usleep(123);
        int y = rand()%10+1;
        Task t(x,y);
        //2. 生产数据
        rq->Enqueue(t);
        std::cout<<"生成任务： "<<t.Question()<<std::endl;

    }
}

void *ConsumerRoutine(void *args)
{
    // ThreadDate *td = static_cast<ThreadDate*>(args);
    // pthread_setname_np(pthread_self(),td->name.c_str());
    RingQueue<Task> *rq = static_cast<RingQueue<Task>*>(args);
    int data = 0;
    while(true)
    {
        // td->rq->Pop(&data);
        // Print(td->name,"消费数据："+std::to_string(data));
        //2. 生产数据
        //1. 消费任务
        Task t;
        //2.处理任务
        rq->Pop(&t);
        std::cout<<"消费任务： "<<t.getResult()<<std::endl;

    }
    
}

int main()
{
    RingQueue<Task> *rq = new RingQueue<Task>();
    srand(time(nullptr) ^ getpid());
    //单单
    // pthread_t c[2],p[3];

    // ThreadDate *td0 = new ThreadDate(rq,"product-1");
    // pthread_create(p,nullptr,ConsumerRoutine,td0);

    // ThreadDate *td1 = new ThreadDate(rq,"product-2");
    // pthread_create(p+1,nullptr,ConsumerRoutine,td1);

    // ThreadDate *td2 = new ThreadDate(rq,"product-3");
    // pthread_create(p+2,nullptr,ConsumerRoutine,td2);

    // ThreadDate *td3 = new ThreadDate(rq,"consumer-1");
    // pthread_create(c,nullptr,ProductorRoutine,td3);

    // ThreadDate *td4 = new ThreadDate(rq,"consumer-2");
    // pthread_create(c+1,nullptr,ProductorRoutine,td4);

    pthread_t c,p;

    pthread_create(&c,nullptr,ProductorRoutine,rq);
    pthread_create(&p,nullptr,ConsumerRoutine,rq);

    pthread_join(c,nullptr);
    pthread_join(p,nullptr);

    // pthread_join(c[0],nullptr);
    // pthread_join(c[1],nullptr);
    // pthread_join(p[0],nullptr);
    // pthread_join(p[1],nullptr);
    // pthread_join(p[2],nullptr);
    return 0;
}