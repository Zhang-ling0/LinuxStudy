#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
int main()
{
	while(1){
		pid_t id = getpid();
		printf("我是一个进程：pid:%d\n", id);
		sleep(1);
	}
}
