#pragma once
#include<iostream>
#include<string>
#include<vector>
#include"Mutex.hpp"
#include"Sem.hpp"
const int defaultcap =5;
template<typename T>
class RingQueue
{
public:
    RingQueue(int cap = defaultcap)
        :_cap(cap)
        ,_rq(cap)
        ,_consumer_step(0)
        ,_productor_step(0)
        ,_data_sem(0)
        ,_blank_sem(cap)
    {

    }
    void Enqueue(T &in)//生产者调用
    {
        //1. 预定资源
        _blank_sem.P();
        _pmutex.lock();//让生产者先竞争锁呢？还是让生产者先预定资源呢？ 先预定资源，再竞争锁
        //2. 找位置进行生产
        _rq[_productor_step++] = in;
        _productor_step %=_cap;//环型队列的概念
        _pmutex.unlock();
        //3. 释放数据资源
        _data_sem.V();
    }
    void Pop(T *out)//消费者调用
    {
   
        //预定资源
        _data_sem.P();
        _cmutex.lock();
        *out = _rq[_consumer_step++];
        _consumer_step %= _cap;
        _cmutex.unlock();
        _blank_sem.V(); 
    }
    ~RingQueue(){}
private:
    int _cap;//环形队列的容量
    std::vector<T> _rq;//环形队列

    int _consumer_step;
    int _productor_step;

    Sem _blank_sem; //格子资源计数器，生产者关心
    Sem _data_sem;//数据信号量，消费者关心

    Mutex _cmutex;
    Mutex _pmutex;

};

    


