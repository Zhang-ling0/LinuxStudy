#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<stdlib.h>
int main(){
	printf("我是一个父进程：pid: %d,ppid: %d\n",getpid(),getppid());
	pid_t id = fork();
	if(id<0)
	{
		perror("fork");
		exit(1);
	}
	else if(id == 0)
	{
		//child
		int cnt = 5;
		while(cnt--)
		{
			printf("我是一个子进程：pid: %d,ppid: %d\n",getpid(),getppid());
			sleep(1);
			//int *p = NULL;
			//*p = 100;//0地址不能写
		}
		exit(0);
	}
	else{
		int status = 0;
		pid_t rid = waitpid(id,&status,0);//如果你等待的进程不是你的
		if(rid>0)
		{
			if(WIFEXITED(status)){
				printf("wait success,退出的子进程是： %d,exit_code: %d\n",rid,WEXITSTATUS(status));
			}
			else
			{
				printf("子进程是异常退出的!\n");
			}
		}
		else{
			printf("ret: %d\n",rid);
			perror("waitpid");
		}
	}
	return 0;

}
