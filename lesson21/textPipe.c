#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
int main()

{
    // 1.创建管道
    int pipefd[2] = {0};
    int n = pipe(pipefd);
    if (n < 0)
    {
        perror("pipe");
        return 1;
    }
    printf("pipefd[0]: %d,pipefd[1]: %d\n", pipefd[0], pipefd[1]); // 3,4
    // 2.创建子进程:w
    pid_t id = fork();
    if (id == 0)
    {
        // child
        close(pipefd[0]);
        char *msg = "Hello, World!";
        int cnt = 5;
        char outbuffer[1024];
        while (1)
        {
            // sleep(10);//让其往管道里写数据的速度不固定
            snprintf(outbuffer, sizeof(outbuffer), "f->c# %s %d %d\n", msg, cnt--, getpid()); // 指定缓冲区大小的标准输出,字符串格式化方案
            // 这个东西会自动给buffer加上\0,所以strlen不需要+1
            write(pipefd[1], outbuffer, strlen(outbuffer)); // 这里要不要+1？不要，因为管道也是文件
            // // if(cnt%3==0)//条件不满足，就一直写，让其往管道里写数据的速度不固定
            sleep(1);
            // close(pipefd[1]); // 关闭写段，告诉父进程我写完了
            // break;
        }
        printf("write endpoint quit\n");
        close(pipefd[1]);
        exit(0);
    }
    // parent:r
    // 关闭读写段
    close(pipefd[1]);
    char buf[1024] = {0};
    while (1)
    {
        ssize_t n = read(pipefd[0], buf, sizeof(buf) - 1); // 只关系读到了几个字节，不在乎读取到的是什么内容，管道是字节流，读到多少就返回多少
        if (n > 0)

        {
            buf[n] = 0;
            printf("c->f# %s\n", buf); // 加上\n让每一次buffer都不一样
        }
        else if (n == 0)
        {
            printf("child exit\n");
            break;
        }
        else
        {
            perror("read");
            break;
        }
        close(pipefd[0]);
        break;
    }
    sleep(10);
    // 前三步做完我们没有对应的通信
    int status = 0;
    pid_t rid = waitpid(id, &status, 0);
    if(rid>0)
    {
        printf("child quit code: %d signal: %d\n",(status>>8)&0xFF,status&0x7F);
    }
    (void)rid; // 防止编译器告警，因为你定义了但是不用人家
    return 0;
}
