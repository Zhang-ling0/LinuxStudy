#ifndef __BLOCK_QUEUE_H
#define __BLOCK_QUEUE_H

#include <iostream>
#include <queue>
#include"Mutex.hpp"
#include"Cond.hpp"
const int defaltcap = 5;
template <typename T>
class BlockQueue
{
public:
    BlockQueue(int cap = defaltcap)
        : _cap(cap)
    {
        sleep_productor_num = 0;
        sleep_consumer_num = 0;
    }
    void Enqueue(T in) // 生产者
    {
        {
        grouplock lock(_mutex);
        while (_bq.size() == _cap)
        {
            sleep_productor_num++;
            _Productor_cond.Wait(_mutex);
            sleep_productor_num--;
        }
        _bq.push(in);
        if (sleep_consumer_num > 0) // 还可以加水位线
            _Consumer_cond.Signal();
        }
    }
    void Pop(T *out) // 消费者
    {
        {
        grouplock lock(_mutex);
        while (_bq.empty()) // 醒来不能直接往后走，判断条件避免伪唤醒问题
        {
            sleep_consumer_num++;
            _Consumer_cond.Wait(_mutex);
            sleep_consumer_num--;
        }
        *out = _bq.front();
        _bq.pop();
        if (sleep_productor_num > 0) // 还可以加水位线
            _Productor_cond.Signal();
        }
    }
    ~BlockQueue()
    {
    
    }

private:
    std::queue<T> _bq;
    int _cap;
    Mutex _mutex;

    Cond _Consumer_cond;
    Cond _Productor_cond;

    int sleep_productor_num;
    int sleep_consumer_num;
    
};
#endif
