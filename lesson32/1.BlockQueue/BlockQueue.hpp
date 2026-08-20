#ifndef __BLOCK_QUEUE_H
#define __BLOCK_QUEUE_H

#include <iostream>
#include "pthread.h"
#include <queue>
const int defaltcap = 5;
template <typename T>
class BlockQueue
{
public:
    BlockQueue(int cap = defaltcap)
        : _cap(cap)
    {
        pthread_mutex_init(&_mutex, nullptr);
        pthread_cond_init(&_consumer_cond, nullptr);
        pthread_cond_init(&_productor_cond, nullptr);

        // _blockqueue_low_water=_bq.size()/3;
        // _blockqueue_low_water=_bq.size()*2/3;
        sleep_productor_num = 0;
        sleep_consumer_num = 0;
    }
    void Enqueue(T &in) // 生产者
    {
        pthread_mutex_lock(&_mutex);
        // 生产条件一定满足吗？
        while (_bq.size() == _cap)
        {
            sleep_productor_num++;
            pthread_cond_wait(&_productor_cond, &_mutex);
            sleep_productor_num--;
        }
        _bq.push(in);
        // if (bq.size() > _consumer_high_water)
        //     pthread_cond_signal(&_consumer_cond); // 唤醒，可以放在临界区以内，也可以放到临界区以外
        //  唤醒解锁，竞争锁，每竞争到锁，就在锁边等待
        if (sleep_consumer_num > 0) // 还可以加水位线
            pthread_cond_signal(&_consumer_cond);
        pthread_mutex_unlock(&_mutex);
    }
    void Pop(T *out) // 消费者
    {
        // 从阻塞队列中获取元素
        pthread_mutex_lock(&_mutex);
        // 增强代码的健壮性，需要进行一些防御性编程将if->while
        // 如果队列中没有元素，就不能pop,判断等待
        while (_bq.empty()) // 醒来不能直接往后走，判断条件避免伪唤醒问题
        {
            // 0.为什么要判断？
            // 只有有数据了才能取数据
            // 1.为什么非得在临界区里进行等待？
            // 因为访问临界资源，必定在临界区内部访问，判断资源不久徐，本质也是访问临界资源
            // 判断必须在临界区内部，(买票)整个过程必须是安全的
            // 2.为什么要把锁传递进去？
            // 原因是等待的时候，是在临界区内部等待的，需要把锁
            // 传递进入，让pthread_cond_wait自动释放锁
            // 当线程唤醒的时候，我可是在临界区内部醒来的
            // 把锁传递进入，让pthread_cond_wait自动竞争并获取_mutex锁
            sleep_consumer_num++;
            pthread_cond_wait(&_consumer_cond, &_mutex);
            sleep_consumer_num--;
            // 1. 过量的唤醒信息
            // 2. 函数调用失败
            // 3. 线程被伪唤醒:伪唤醒后接着向下执行，如果队列为空不久报错了吗
        }
        // 100%:bq肯定有数据
        // 不能else
        *out = _bq.front();
        _bq.pop();
        // if (bq.size() < _consumer_low_water)
        //     pthread_cond_signal(&_productor_cond); // 唤醒，可以放在临界区以内，也可以放到临界区以外
        //  ？？？
        if (sleep_productor_num > 0) // 还可以加水位线
            pthread_cond_signal(&_productor_cond);
        pthread_mutex_unlock(&_mutex);
    }
    ~BlockQueue()
    {
        pthread_mutex_destroy(&_mutex);
        pthread_cond_destroy(&_consumer_cond);
        pthread_cond_destroy(&_productor_cond);
    }

private:
    std::queue<T> _bq;
    int _cap;
    pthread_mutex_t _mutex;
    // 为生产者和消费者各自提供一个条件变量
    pthread_cond_t _consumer_cond;
    pthread_cond_t _productor_cond;
    // int _consumer_low_water;//消费低水位线
    // int _productor_low_water;//消费低水位线
    // 水位线
    // int _blockqueue_low_water;  // bq低水位线
    // int _blockqueue_high_water; // bq高水位线

    // 线程休眠个数:计数器衡量
    int sleep_productor_num;
    int sleep_consumer_num;

    
};
#endif
