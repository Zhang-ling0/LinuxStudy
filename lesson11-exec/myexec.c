#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/types.h>


int main()
{
	printf("我变成了一个进程： %d\n",getpid());
	pid_t id = fork();
	if(id == 0)
	{
		sleep(2);
		printf("下面的代码都是子进程在执行:\n");
		char *myargv[] = {
			(char*)"othercmd",
			(char*)"-a",
			(char*)"-b",
			(char*)"-c",
			(char*)"-d",
			(char*)"-f",
			NULL
		};
		char *myenv[] = {
			(char*)PATH=/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin:/usr/games:/usr/local/games:/snap/bin
		};
	}
}

//int main()
//{
//	printf("我变成了一个进程：%d\n",getpid());
//	pid_t id = fork();
//	if(id==0)
//	{
//		//执行另一个程序的代码
//		printf("子进程立即执行ls命令\n");
//		execl("/usr/bin/ls","ls","-a","-l",NULL);
//		exit(0);
//	}
//	wait(NULL);
//	printf("我的代码运行中......\n");
//	printf("我的代码运行中......\n");
//	printf("我的代码运行中......\n");
//	printf("我的代码运行中......\n");
//	printf("我的代码运行中......\n");
//	printf("我的代码运行中......\n");
//	printf("我的代码运行中......\n");
//	return 0;
//}
