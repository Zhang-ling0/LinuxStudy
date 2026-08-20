#include "BlockQueue.hpp"
#include <unistd.h>
int num = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int GetNumber()
{
    pthread_mutex_lock(&lock);
    int number = num++;
    pthread_mutex_unlock(&lock);
    return num;
}
void *ConsumerRutine(void *args)
{
    // 线程局部存储？
    sleep(5);
    int number = GetNumber();
    std::string name = "Consumer-" + std::to_string(number);
    pthread_setname_np(pthread_self(), name.c_str());           // 将线程的名字设置进去
    BlockQueue<int> *bq = static_cast<BlockQueue<int> *>(args); // 让两个线程看到同一个阻塞队列
    while (true)
    {
        int data;
        bq->Pop(&data);
        std::cout << name << " 消费" << data << std::endl;
    }
}
void *ProductorRoutine(void *args)
{
    int number = GetNumber();
    std::string name = "Productor-" + std::to_string(number);
    pthread_setname_np(pthread_self(), name.c_str()); // 将线程的名字设置进去
    BlockQueue<int> *bq = static_cast<BlockQueue<int> *>(args);
    int data = 10;
    while (true)
    {
        bq->Enqueue(data);
        std::cout << name << " 生产" << data++ << std::endl;
    }
    return nullptr;
}
int main()
{
    BlockQueue<int> *bq = new BlockQueue<int>(); // 创建阻塞队列
    pthread_t c,p;
    pthread_create(&c, nullptr, ConsumerRutine, bq);
    pthread_create(&p, nullptr, ProductorRoutine, bq);

    pthread_join(c, nullptr);
    pthread_join(p, nullptr);


    // pthread_t c[3], p[2];
    // pthread_create(c, nullptr, ConsumerRutine, bq);
    // pthread_create(c + 1, nullptr, ConsumerRutine, bq);
    // pthread_create(c + 2, nullptr, ConsumerRutine, bq);
    // pthread_create(p, nullptr, ProductorRoutine, bq);
    // pthread_create(p + 1, nullptr, ProductorRoutine, bq);

    // pthread_join(c[0], nullptr);
    // pthread_join(c[1], nullptr);
    // pthread_join(c[2], nullptr);
    // pthread_join(p[0], nullptr);
    // pthread_join(p[1], nullptr);
    return 0;
}