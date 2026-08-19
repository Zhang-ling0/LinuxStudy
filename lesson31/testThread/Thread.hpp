#include <iostream>
#include <pthread.h>

class Mutex {
public:
  Mutex() { pthread_mutex_init(&_lock, nullptr); }
  void lock() { pthread_mutex_lock(&_lock); }
  void unlock() { pthread_mutex_unlock(&_lock); }
  ~Mutex() { pthread_mutex_destroy(&_lock); }

private:
  pthread_mutex_t _lock;
};

class grouplock {
public:
  grouplock(Mutex& lock) : _lockref(lock) { _lockref.lock(); }
  ~grouplock() { _lockref.unlock(); }

private:
  Mutex &_lockref;
};
