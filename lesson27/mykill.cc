#include <iostream>
#include <signal.h>
int cnt = 1;
void Usage(const std::string &cmd) // 相当于这里写了一个手册
{
  std::cout << "Usage: " << cmd << " signumber who " << std::endl;
}
void hander(int sig) {
  std::cout << "进程捕捉到信号： " << sig << " pid: " << getpid()
            << " cnt: " << cnt << std::endl;
  // exit(4);
  int n = alarm(2);
  (void)n; // 不敢死循环，这里是闹钟的不断设置
  std::cout << "上一个闹钟的剩余时间： " << n << std::endl;
}
// ./没有kill signumber who
int main(int argc, char *argv[]) // 三个命令行参数
{
  // signal(2,hander);
  // signal(6,hander);
//   signal(SIGALRM, hander);
  alarm(200);
  sleep(1);
  int n = alarm(0);
  std::cout << "进程正在运行： " << cnt << " pid: " << getpid() << "n= "<<n<< std::endl;
  while (true) {
    // std::cout << "进程正在运行： " << getpid() <<std::endl;
    //int n = alarm(3);
    std::cout << "进程正在运行： " << cnt << " pid: " << getpid() << " n= "<<n<< std::endl;
    //cnt++;
    sleep(1);
    // abort();
    // raise(2);
  }
  // if(argc != 3)
  // {
  //     Usage(argv[0]);
  //     exit(1);
  // }
  // int signumber = std::stoi(argv[1]);
  // pid_t pid = std::stoi(argv[2]);

  // int n = kill(pid,signumber);
  // (void)n;
  return 0;
}