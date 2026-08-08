#include <iostream>
#include <signal.h>
#include <unistd.h>

void PrintPending(sigset_t &pending)
{
    // 从 31 到 1 递减打印
    for (int signo = 31; signo > 0; signo--)
    {
        if (sigismember(&pending, signo))
        {
            std::cout << "1";
        }
        else
        {
            std::cout << "0";
        }
        // 每 8 个信号加一个空，方便阅读
        if ((signo - 1) % 8 == 0 && signo != 1)
        {
            std::cout << " ";
        }
    }
    std::cout << std::endl;
}

void handler(int signo)
{
    std::cout << "=====================entery handler============" << std::endl;
    sigset_t pending;
    sigemptyset(&pending);
    //2. huoqu pending表
    int n = sigpending(&pending);
    (void)n;
    PrintPending(pending);//打印在信号内部获取的pending
    std::cout << "处理完成: " << signo << std::endl;
    std::cout << "=====================leave handler============" << std::endl;
}

int main()
{
    // 0. 注册 2 号信号的自定义处理函数
    signal(SIGINT, handler);

    // 1. 屏蔽 2 号信号
    sigset_t block_set, old_set;
    sigemptyset(&block_set);
    sigemptyset(&old_set);
    sigaddset(&block_set, SIGINT);  // 屏蔽 2 号信号
    sigprocmask(SIG_SETMASK, &block_set, &old_set);

    std::cout << "我的pid: " << getpid() << std::endl;
    std::cout << "2号信号已被屏蔽，请按 Ctrl+C 发送 SIGINT" << std::endl;

    int cnt = 0;

    while (true)
    {
        sigset_t pending;
        sigemptyset(&pending);
        // 2. 获取 pending 表
        sigpending(&pending);
        // 3. 打印 pending 表
        PrintPending(pending);

        // 4. 20 秒后解除对 2 号信号的屏蔽
        if (cnt == 20)
        {
            std::cout << "解除对 2 号信号的屏蔽，待处理的信号将被递达" << std::endl;
            sigprocmask(SIG_SETMASK, &old_set, nullptr);
        }
        cnt++;
        sleep(1);
    }

    return 0;
}