#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <string>
#include <vector>
#include <functional>
#include <unistd.h>
#include <sys/wait.h>

///////////////////////////////子进程要完成的任务/////////////////////////
void SyncDisk()
{
    std::cout << getpid() << ": 刷新数据到磁盘任务" << std::endl;
    sleep(1);
}

void Download()
{
    std::cout << getpid() << ": 下载数据到系统中" << std::endl;
    sleep(1);
}

void PrintLog()
{
    std::cout << getpid() << ": 打印日志到本地" << std::endl;
    sleep(1);
}

void UpdateStatus()
{
    std::cout << getpid() << ": 更新一次用户的状态" << std::endl;
    sleep(1);
}

typedef void (*task_t)(); // 函数指针

task_t tasks[4] = {SyncDisk, Download, PrintLog, UpdateStatus}; // 任务表 ---面向对象化

///////////////////////////////进程池相关////////////////////////////////
enum
{
    OK = 0,
    PIPE_ERROR,
    FORK_ERROR
};

// 子进程的入口函数
void DoTask(int fd)
{
    while (true)
    {
        int task_code = 0;
        ssize_t n = read(fd, &task_code, sizeof(task_code)); // 问题1: 子进程需要sleep吗？不需要！
        if (n == sizeof(task_code))
        {
            if (task_code >= 0 && task_code < 4)
            {
                tasks[task_code](); // 执行任务表中的任务
            }
        }
        else if (n == 0)
        {
            // 父进程要结束，我也应该要退出了！
            std::cout << getpid() << ": task quit ..." << std::endl;
            break;
        }
        else
        {
            perror("read");
            break;
        }
    }
}

const int gprocessnum = 5;
using cb_t = std::function<void(int)>;

class ProcessPool
{
private:
    // 父进程管理"通道"
    class Channel
    {
    public:
        Channel(int wfd, pid_t pid) : _wfd(wfd), _sub_pid(pid)
        {
            _sub_name = "sub-channel-" + std::to_string(_sub_pid);
        }
        ~Channel()
        {
        }
        void Write(int index)
        {
            ssize_t n = write(_wfd, &index, sizeof(index)); // 约定的4字节发送吗？
            (void)n;
        }
        std::string Name()
        {
            return _sub_name;
        }
        void ClosePipe()
        {
            std::cout << "关闭wfd: " << _wfd << std::endl;
            close(_wfd);
        }
        void Wait()
        {
            pid_t rid = waitpid(_sub_pid, nullptr, 0);
            (void)rid;
            std::cout << "回收子进程: " << _sub_pid << std::endl;
        }
        void PrintInfo()
        {
            printf("wfd: %d, who: %d, channel name: %s\n", _wfd, _sub_pid, _sub_name.c_str());
        }

    private:
        int _wfd;              // 1. wfd
        pid_t _sub_pid;        // 2. 子进程是谁
        std::string _sub_name; // 3. 子channel的名字

        // int cnt;
    };

public:
    ProcessPool()
    {
        srand((unsigned int)time(nullptr) ^ getpid());
    }
    ~ProcessPool() {}
    void Init(cb_t cb)
    {
        CreateProcessChannel(cb);
    }
    void Run()
    {
        int cnt = 10;
        // while (cnt--)
        while (true)
        {
            std::cout << "------------------------------------------------" << std::endl;
            // 1. 选择一个任务
            int itask = SelectTask();
            std::cout << "itask: " << itask << std::endl;

            // 2. 选择一个channel(管道+子进程)，本质是选择一个下标数字
            int index = SelectChannel();
            std::cout << "index: " << index << std::endl;

            // 3. 发送一个任务给指定的channel(管道+子进程)
            printf("发送 %d to %s\n", itask, channels[index].Name().c_str());
            SendTask2Salver(itask, index);
        }
    }
    void Quit()
    {
        // version3:
        for (auto &channel : channels)
        {
            channel.ClosePipe();
            channel.Wait();
        }
        // version2: 逆向回收
        // int end = channels.size()-1;
        // while(end >= 0)
        // {
        //     channels[end].ClosePipe();
        //     channels[end].Wait();
        //     end--;
        // }

        // bug演示
        // for (auto &channel : channels) // 为什么不能这样写？应该怎么写？bug?
        // {
        //     channel.ClosePipe();
        //     channel.Wait();
        // }

        // version1
        // // 1. 让所有子进程退出
        // for (auto &channel : channels)
        // {
        //     channel.ClosePipe();
        // }

        // // 2. 回收子进程
        // for (auto &channel : channels)
        // {
        //     channel.Wait();
        // }
    }
    void Debug()
    {
        for (auto &c : channels)
        {
            c.PrintInfo();
        }
    }

private:
    void SendTask2Salver(int itask, int index)
    {
        if (itask >= 4 || itask < 0)
            return;
        if (index < 0 || index >= channels.size())
            return;

        channels[index].Write(itask);
    }

    int SelectChannel()
    {
        static int index = 0;
        int selected = index;
        index++;
        index %= channels.size();

        return selected;
    }
    int SelectTask()
    {
        int itask = rand() % 4;
        return itask;
    }
    void CreateProcessChannel(cb_t cb)
    {
        // 1. 创建多个管道和创建多个进程
        for (int i = 0; i < gprocessnum; i++)
        {
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
                exit(FORK_ERROR);
            }
            else if (id == 0)
            {
                // 子进程关闭历史wfd， 影响的是自己的fd表
                if (!channels.empty())
                {
                    for (auto &channel : channels)
                        channel.ClosePipe();
                }

                // child
                close(pipefd[1]); // read
                cb(pipefd[0]);    // 回调: 让子进程调用出去，回调完成，他还会回来的！！！
                exit(OK);         // 根本不会执行后续代码，执行完自己的DoTask()函数之后，自己就退出了
            }
            else
            {
                // 父进程 write
                close(pipefd[0]);
                channels.emplace_back(pipefd[1], id);
                // Channel ch(pipefd[1], id);
                // channels.push_back(ch);
                std::cout << "创建子进程: " << id << " 成功..." << std::endl;
                sleep(1);
            }
        }
    }

private:
    // 0. 未来组织所有channel的容器
    std::vector<Channel> channels;
};

int main()
{
    // 1. 初始化进程池
    ProcessPool pp;
    pp.Init(DoTask);
    pp.Debug();

    // 2. 父进程控制子进程,channels
    pp.Run();

    // 3. 释放和回收所有资源(释放管道，回收子进程)
    pp.Quit();

    return 0;
}