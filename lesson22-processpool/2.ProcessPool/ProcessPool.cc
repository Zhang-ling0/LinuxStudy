#include <iostream>
#include <string>
#include <vector>
#include <cstdio>
#include <unistd.h>
#include <sys/wait.h>
#include <functional>
#include <ctime>
#include <cstdlib>

//================ 子进程要完成的任务 ===========================
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

typedef void (*task_t)();
task_t task[4] = {SyncDisk, Download, PrintLog, UpdateStatus};

enum
{
    OK = 0,
    PIPE_ERROR,
    FORK_ERROR
};

using cb_t = std::function<void(int)>;

//==================== 子进程的入口函数 ============================
void DoTask(int fd)
{
    while (true)
    {
        int task_code = 0;
        ssize_t n = read(fd, &task_code, sizeof(task_code));

        if (n == sizeof(task_code))
        {
            if (task_code >= 0 && task_code < 4)
            {
                task[task_code]();
                sleep(1); // 任务执行完稍作休息
            }
        }
        else if (n == 0)
        {
            std::cout << getpid() << "：task quit ..." << std::endl;
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

//==================== 进程池 ============================
class ProcessPool
{
public:
    class Channel
    {
    public:
        Channel(int wfd, pid_t pid)
            : _wfd(wfd), _sub_pid(pid)
        {
            _sub_name = "sub-channel-" + std::to_string(_sub_pid);
        }

        void PrintInfo()
        {
            printf("wfd=%d, who=%d, channel name=%s\n",
                   _wfd, _sub_pid, _sub_name.c_str());
        }

        std::string Name()
        {
            return _sub_name;
        }

        void Write(int index)
        {
            ssize_t n = write(_wfd, &index, sizeof(index));
            (void)n;
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
            std::cout << "回收子进程： " << _sub_pid << std::endl;
        }

        ~Channel() {}

    private:
        int _wfd;
        pid_t _sub_pid;
        std::string _sub_name;
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
        while (cnt--)
        {
            std::cout << "------------------------------------------------" << std::endl;

            int itask = SelectTask();
            std::cout << "itask: " << itask << std::endl;

            int index = SelectChannel();
            if (index < 0)
            {
                std::cerr << "没有可用的子进程" << std::endl;
                break;
            }
            std::cout << "index: " << index << std::endl;

            printf("发送 %d to %s\n", itask, channels[index].Name().c_str());
            SendTaskToSlave(itask, index);
            sleep(1);
        }
    }

    void Quit()
    {
        // 先关闭所有管道写端 → 让所有子进程同时收到 EOF
        for (auto &ch : channels)
        {
            ch.ClosePipe();
        }

        // 再统一回收所有子进程
        for (auto &ch : channels)
        {
            ch.Wait();
        }
    }

    void Debug()
    {
        for (auto &c : channels)
        {
            c.PrintInfo();
        }
    }

private:
    int SelectTask()
    {
        return rand() % 4;
    }

    int SelectChannel()
    {
        if (channels.empty())
            return -1;

        static int index = 0;
        int selected = index;
        index = (index + 1) % channels.size();
        return selected;
    }

    void SendTaskToSlave(int itask, int index)
    {
        if (itask < 0 || itask >= 4)
            return;
        if (index < 0 || index >= (int)channels.size())
            return;

        channels[index].Write(itask);
    }

    void CreateProcessChannel(cb_t cb)
    {
        for (int i = 0; i < gprocessnum; i++)
        {
            int pipefd[2] = {0};
            if (pipe(pipefd) < 0)
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
                // 子进程：关闭所有历史管道写端
                for (auto &ch : channels)
                {
                    ch.ClosePipe();
                }

                close(pipefd[1]); // 关闭当前管道的写端
                cb(pipefd[0]);    // 执行回调函数（DoTask）
                exit(OK);
            }
            else
            {
                // 父进程：关闭当前管道的读端
                close(pipefd[0]);
                channels.emplace_back(pipefd[1], id);
                std::cout << "创建子进程：" << id << " 成功..." << std::endl;
            }
        }
    }

private:
    std::vector<Channel> channels;
};

//==================== main ============================
int main()
{
    ProcessPool pp;
    pp.Init(DoTask);
    // pp.Debug();

    pp.Run();
    pp.Quit();

    return 0;
}