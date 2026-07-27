#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<string.h>
int main()

{
    //1.创建管道
    int pipefd[2]={0};
    int n =pipe(pipefd);
    if(n<0)
    {
        perror("pipe");
        return 1;
    }
    printf("pipefd[0]: %d,pipefd[1]: %d\n",pipefd[0],pipefd[1]);
    //2.创建子进程:w
    pid_t id = fork();
    if(id==0)
    {
        //child
        exit(0);
        close(pipefd[0]);
        char *msg = "Hello, World!";
        write(pipefd[1],msg,strlen(msg));//这里要不要+1？不要，因为管道也是文件
    }
    //parent:r
    //关闭读写段
    char buf[1024]={0};
    read(pipefd[0],buf,sizeof(buf));
    printf("buf: %s\n",buf);
    close(pipefd[1]);
    //前三步做完我们没有对应的通信
    pid_t rid = waitpid(id,NULL,0);
    (void)rid;//防止编译器告警，因为你定义了但是不用人家
    return 0;
}
