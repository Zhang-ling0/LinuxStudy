#ifndef __COND_HPP
#define __COND_HPP
#include <pthread.h>
#include "Thread.hpp"
class Cond
{
public:
    Cond()
    {
        pthread_cond_init(&_cond, nullptr);
    }
    void Wait(Mutex &mutex)
    {
        //wait接口让线程在指定的条件变量下等
        int n = pthread_cond_wait(&_cond,mutex.Ptr());//这里必须传递原生的锁
        (void)n;
    }
    void Signal()
    {
        //唤醒
        int n = pthread_cond_signal(&_cond);
        (void)n;
    }
    void Boradcast()
    {
        int n = pthread_cond_broadcast(&_cond);//广播唤醒
        (void)n;
    }
    ~Cond()
    {
        pthread_cond_destroy(&_cond);
    }

private:
    pthread_cond_t _cond;
} cond_init
#endif