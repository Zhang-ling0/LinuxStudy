#include "Task.hpp"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <pthread.h>
#include <signal.h>
#include <string>
#include <unistd.h>
#include <vector>
//线程局部存储
__thread pid_t id = 0;
void *routine(void *args) {
  std::string name = static_cast<const char *>(args);
  while (true) {
    std::cout << "new thread id: " << id << std::endl;
    printf("new thread id: %p\n",&id);
    id++;
    sleep(1);
  }
  return nullptr;
}

int main() {
  pthread_t tid;
  pthread_create(&tid, nullptr, routine, "thread-1");
  while (true) {
    std::cout << "main thread id: " << id << std::endl;
    sleep(1);
  }
  return 0;
}
// classRes {
// public:
//   int code;
//   std::string name;
//   std::string info;
// }
// void *Routine(void *args) {
//     int cnt = 5;
//     while(cnt)
//     {
//         std::cout<<"新进程正在运行： "<<cnt--<<endl;
//         sleep(1);
//     }
//     return nullptr;
// //   Task *t = static_cast<Task *>(args);
// //   t->Excute();
// //   while(true){
// //     sleep(1);
//   }
//   retuen(void *) t;
//   std::string name = static_cast<const char *>(args);
//   while (true) {
//     std::cout << "new thread" << name << std::endl;
//   }
//   Res res = new Res();
//   res->code = 10;
//   res->name = name;
//   res->info = "我这个线程已经完蛋了你们继续努力！" retuen(void *) res;
// }
// int main() {
//   Task_t(10, 20);
//   pthread_t tid;
//   // pthread_create(&tid, nullptr, Routine, (void *)"thread-1");
//   pthread_create(&tid, nullptr, Routine, (void *)t);
//   sleep(1);
//   stdcout << "分离线程"<<std::endl;
//    pthread_cancel(tid);
// //   std::cout << "取消目标线程：    "<<std::endl;
// //   pthread_cancel(tid);
//   //Task *task;
//   // 1. 一般而言不许等待新线程退出，如果不等待，导致类似僵尸的问题
//   // 2.为了获取新线程的执行解耦
//   // res *retval = nullptr;
//   // 为什么这里就直接是新线程的退出信息，没有退出信号？没有异常分析？
//   //int n = pthread_join(tid, (void **)&retval);
//   void *ret = nullptr;
//   int n = pthread_join(tid,&ret);
//   if (n == 0) {
//     std::cout << "join success: " << (long long)ret << std::endl;
//   }
//   else
//   {
//     std::cout <<"join error: "<<n<<std::endl;
//   }
//   delete retval;
//   return 0;
// }

// int g_val = 100;

// void *Routine(void *args) {
//     sleep(3);
//     Task *t = static_cast<Task *>(args);
//     t->Excute();
//     std::cout << t->Result() << std::endl;
//     delete t;
//     return nullptr;  //  直接返回
// }

// int main() {
//     srand(time(nullptr) ^ getpid());
//     const int num = 10;
//     std::vector<pthread_t> tids;  // 只保留 vector

//     for (int i = 0; i < num; i++) {
//         int x = rand() % 10 + 1;
//         usleep(234);
//         int y = rand() % 7 + 1;
//         Task *t = new Task(x, y);
//         pthread_t tid;
//         pthread_create(&tid, nullptr, Routine, t);
//         tids.push_back(tid);  // 添加 tid
//     }

//     // 打印所有线程 ID
//     for (auto &tid : tids) {
//         printf("tid: 0x%lx\n", tid);
//     }

//     // 随机取消子线程
//     while (true) {
//         printf("我是主线程tid: 0x%lx, pid: %d, g_val: %d, &g_val: %p\n",
//                pthread_self(), getpid(), g_val, &g_val);
//         sleep(1);

//         int who = rand() % tids.size();
//         if (tids[who] != -1) {
//             pthread_cancel(tids[who]);
//             tids[who] = (pthread_t)-1;  // 标记为已取消
//             printf("取消线程 %d\n", who);
//         }
//     }

//     return 0;
// }
// void PrintName(const std::string &name) {
//   printf("我是新线程tid: %s, tid 0x%lx, pid: %d\n", name.c_str(),
//          pthread_self(), getpid());
// }

// void *Routine(void *args) {
//     sleep(3);
//     Task *t = static_cast<Task *>(args);
//     t->Excute();
//     std::cout<<t->Result() << std::endl;
// //   std::string name = static_cast<const char *>(args);
// //   PrintName(name);
// //   printf("----------------------------\n");
// //   while (true) {
// //     sleep(1);
// //     g_val++;
//     // if (name == "thread-8")
//     // {
//     //     std::cout << "thread-8 say# 我要异常了" << std::endl;
//     //     int a = 10;
//     //     a /= 0;  // 触发 SIGFPE
//     //     std::cout << "这行不会打印" << std::endl;
//     // }
// //   }
//     return nullptr;
// }

// int main() {
//   // 注册信号处理，确认异常是否触发
// //   signal(SIGFPE, [](int signo) {
// //     std::cout << "捕获到 SIGFPE 信号: " << signo << std::endl;
// //     exit(1);
// //   });
//   srand(time(nullptr)^getpid());
//   const int num = 10;
//   for (int i = 0; i < num; i++) {
//     pthread_t tid;
//     // 构建线程，数据不一致问题
//     // char threadname[64];//被新线程共享的资源，是的
//     // char *threadname = new char[64];
//     // snprintf(threadname, sizeof(threadname), "thread-%d", i + 1);
//     //不设置线程名了
//     int x = rand()%10+1;
//     usleep(234);
//     int y = rand()%7+1;
//     Task *t = new Task(x,y);
//     pthread_create(&tid, nullptr,Routine, t); //
//     每一次拿到的就只是开始缓冲区的地址
//     //sleep(1);
//   }

// //   while (true) {
// //     printf("我是主线程tid: 0x%lx, pid: %d, g_val: %d, &g_val: %p\n",
// //            pthread_self(), getpid(), g_val, &g_val);
// //     sleep(1);
// //   }
//   return 0;
// }