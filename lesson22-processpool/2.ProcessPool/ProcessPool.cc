#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <unistd.h>
#include<sys/wait.h>
#include <functional>
//================子进程要完成的任务===========================
void SyncDisk()
{
    std::cout << getpid() << "：刷新数据到磁盘任务" << std::endl;
    sleep(1);
}
void Download()
{
    std::cout << getpid() << "：下载数据到系统中" << std::endl;
    sleep(1);
}
void PrintLog()
{
    std::cout << getpid() << "：打印日志到本地" << std::endl;
    sleep(1);
}
void UpdateStatus()
{
    std::cout << getpid() << "：更新一次用户的状态" << std::endl;
    sleep(1);
}
typedef void (*task_t)();                                      // 定义函数指针
task_t task[4] = {SyncDisk, Download, PrintLog, UpdateStatus}; // 任务表

enum // 定义错误标准
{
    OK = 0,
    PIPE_ERROR,
    FORK_ERROR
};
//============================子进程应该怎么做？===========================
// typedef std::function<void(int)> task_t;
using cb_t = std::function<void(int)>;
void DoTask(int fd)
{
    while (true)
    {
        int task_code = 0;//定义任务码并初始化为0
        size_t n = read(fd,&task_code,sizeof(task_code));
        //4字节读取4字节写入，客户端和服务端都默认以4字节读写
        //问题1：子进程需要sleep吗？
        if(n==sizeof(task_code))//字节流的形式，返回值n等于4个字节说明写入成功
        {
            if(task_code>=0&&task_code<4)
            {
                task[task_code]();
            }
        }
        else if(n == 0)
        {
            //父进程写入失败，子进程就应该退出了
            std::cout << getpid()<<"：task quit ..."<<std::endl;
            break;
        }
        else
        {
            perror("read");
            break;
        }
        sleep(1);
    }
}

const int gprocessnum = 5;
//====================构建进程池相关操作==========================
// 描述通道
// 父进程管理通道
class ProcessPool
{
public:
    class Channel
    {
        //========================描述管道================================
    public:
        Channel(int wfd, pid_t pid)
            : _wfd(wfd), _sub_pid(pid)
        {
            _sub_name = "sub-channel-" + std::to_string(_sub_pid);
        }
        void PrintInfo()
        {
            printf("wfd=%d,who= %d,channel name=%s\n", _wfd, _sub_pid, _sub_name.c_str());
        }
        std::string Name()
        {
            return _sub_name;
        }
        void Write(int index)
        {
            //将也就是将任务码写道管道里，而管道就是一个文件
            ssize_t n = write(_wfd,&index,sizeof(index));
            (void)n;
        }
        void ClosePipe(){
            std::cout << "关闭wfd:"<<_wfd<<std::endl;
            close(_wfd);
        }
        void Wait()
        {
            pid_t rid = waitpid(_sub_pid,nullptr,0);
            (void)rid;
            std::cout << "回收子进程： "<<_sub_pid<<std::endl;
        }
        ~Channel()
        {
        }

    private:
        int _wfd;              // 1.写端wfd
        pid_t _sub_pid;        // 2.子进程是谁？pid
        std::string _sub_name; // 3.子进程名称
    };
    //========================创建进程并于管道联系起来========================
private:
    int SelectTask()
    {
        int itask = rand() % 4; // 随机数取4的余数
        return itask;
    }
    int SelectChannel()
    {
        static int index = 0; // 让index全局可用,并一直保存
        int selected = index;
        index++; // 轮询的方式
        index %= channels.size();
        return selected;
    }
    void SendTaskToSalver(int itask, int index)
    {
        // 首先需要判断itask和index是否合法
        if (itask > 4 || itask < 0)
            return;
        if (index < 0 || index >= channels.size())
            return;
        channels[index].Write(itask);
 
    }
public:
    ProcessPool() 
    {
        //种下一个随机种子，后面的位运算是为了怎加随机概率，每别的意思
        srand((unsigned int)time(nullptr)^getpid());
    };
    ~ProcessPool() {};
    void Debug()
    {
        for (auto &c : channels)
        {
            c.PrintInfo();
        }
    }
    //=======================进程池控制==============================
    void Run()
    {
        int cnt = 10;
        while(cnt--){
        //while(true){
        std::cout << "------------------------------------------------" << std::endl;
        // 1.选择一个任务
        int itask = SelectTask();
        std::cout << "itask" << itask << std::endl;
        // 2.选择一个channel(管道+子进程)，本质是选择一个下标数字
        int index = SelectChannel();
        std::cout << "indek: " << index << std::endl;
        // 3.发送一个指定任务给指定的channel(管道+子进程)
        printf("发送 %d to %s\n", itask, channels[index].Name().c_str());
        SendTaskToSalver(itask, index);
         sleep(1);
        }
       
    }
    void Quit()
    {
        //我今天就像这样回收
        //version3
        for(auto &channels : channels)
        {
            channels.ClosePipe();
            channels.Wait();
        }
        // //version2:逆向回收
        // int end = channels.size() -1;
        // while(end>=0){
        //     channels[end].ClosePipe();
        //     channels[end].Wait();
        //     end--;
        // }
        //bug演示
        // for(auto &channels : channels)
        // {
        //     channels.ClosePipe();
        //     channels.Wait();
        // }
        // //version1
        // //1.让所有子进程退出
        // for(auto &channel : channels)
        // {
        //     channel.ClosePipe();
        // }
        // //2. 回收子进程
        // for(auto &channel :channels)
        // {
        //     channel.Wait();
        // }
                  
    }

    void Init(cb_t cb)
    {
        CreateProcessChannel(cb);
    }

private:
    void CreateProcessChannel(cb_t cb)
    {

        // 1.创建多个管道和多个进程
        for (int i = 0; i < gprocessnum; i++) // 这个循环只有父进程执行，子进程不会执行
        {
            // 1.创建管道
            int pipefd[2] = {0};
            int n = pipe(pipefd);
            if (n < 0)
            {
                std::cerr << "pipe create error" << std::endl;
                exit(PIPE_ERROR);
            }
            pid_t id = fork();
            if (id < 0)
            {
                std::cerr << "fork error" << std::endl;
                exit(PIPE_ERROR);
            }
            else if (id == 0)
            {
                //子进程关闭历史fd，影响的是自己的文件描述符
                if(!channels.empty()){
                    for(auto &channels : channels)
                        channels.ClosePipe();
                }
                // child
                close(pipefd[1]); // 关闭写端
                cb(pipefd[0]);    // 回调函数
                exit(OK);         // 根本不会执行后续代码，执行完自己的DoTask()函数后，直接退出
            }
            else
            {
                // parent
                close(pipefd[0]); // 关闭读端
                channels.emplace_back(pipefd[1],id);
                Channel ch(pipefd[1], id);
                channels.push_back(ch);
                std::cout << "创建子进程：" << id << "成功..." << std::endl;
                sleep(1);
            }
        }
    }

private:
    std::vector<Channel> channels; // 未来组织所有Channel对象的容器
};

int main()
{
    // 1. 初始化进程池
    ProcessPool pp;
    pp.Init(DoTask);
   // pp.Debug();
    // 2. 父进程控制子进程
    pp.Run();
    // 3.释放和回收所有资源（释放管道，回收子进程）
    pp.Quit();

    return 0;
}