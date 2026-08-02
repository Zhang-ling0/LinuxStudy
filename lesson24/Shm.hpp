#pragma once

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

const int gsize = 128;
//目的是让不同的进程看到同一份资源
#define PATHNAME "/tmp"
#define PROJ_ID 0x66
class Shm {
public:
  // 构造函数
  Shm(int size = gsize) : _shmid(-1), _size(size), _start_addr(nullptr) {}

  void Delete() 
  {
    int n = shmctl(_shmid, IPC_RMID, nullptr);
    (void)n;
  }

  void Attach() {
    _start_addr = shmat(_shmid, nullptr, 0);
    // if((long long int)_start_addr == -1)
    if (_start_addr == (void *)-1) {
      std::cerr << "Attach error";
      exit(3);
    }
  }

  void Detach() {
    int n = shmdt(_start_addr);
    (void)n;
  }
  void Get() { GetHelper(IPC_CREAT | 0666); }
  void Create() { GetHelper(IPC_CREAT | IPC_EXCL | 0666); }

  // 析构函数
  ~Shm() {}
  //测试共享内存的自由度
  void* Add(){
    return _start_addr;
  }
  int size(){
    return _size;
  }
private:
  // 获取key
  key_t GetKey() {

    key_t k = ftok(PATHNAME, PROJ_ID);
    if (k < 0) {
      perror("ftok");
      exit(1);
    }
    return k;
  }

  // 创建/获取共享内存核心函数
  void GetHelper(int shmflag) {
    // 1. 获取key
    key_t k = GetKey();
    if (k < 0) {
      std::cerr << "GetKey error";
      exit(1);
    }
    // 2. 创建新的共享内存
    _shmid = shmget(k, _size, shmflag);
    if (_shmid < 0) {
      perror("shmget");
      exit(2);
    }
    printf("key = 0x%x,_shmid = %d\n", k, _shmid);
  }

private:
  int _shmid;        // 共享内存ID
  int _size;         // 共享内存大小
  void *_start_addr; // 共享内存起始地址
};