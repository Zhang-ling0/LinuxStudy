#include<stdio.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<stdlib.h>
#include<vector>
#include<iostream>
typedef void(*callback_t)();
enum
{
	 OK,
	 USAGE_ERR
};
void Task()
{
	int cnt = 5;
	while(cnt--)
	{
		printf("我是一个子进程，我正在完成任务，pid: %d,ppid: %d,cnt: %d\n",getpid(),			getppid(),cnt);
		sleep(1);
	}
}
void Hello()
{
	int cnt = 5;
	while(cnt--)
	{
		printf("我是一个子进程，我正在完成任务，pid: %d,ppid: %d,cnt: %d\n",getpid(),			getppid(),cnt);
		sleep(1);
	}
}
void CreateChildProcess(int num,std::vector<pid_t> *subs,callback_t cb)
{
	for(int i = 0;i < num;i++){
		pid_t id = fork();
		if(id == 0)
		{
			//child
			cb();
			exit(0);
		}
		//父进程
		subs->push_back(id);
	}
}
void WaitAllChild(const std::vector<pid_t> &subs)
{
	for(const auto &pid : subs)
	{
		int status = 0;
		pid_t rid =waitpid(pid,&status,0);
		if(rid>0)
		{
			printf("子进程：%d Exit,exit code: %d\n",rid,WEXITSTATUS(status));
		}
	}
}

//启停多进程的方案
// ./myproc 5 --我要创建5个子进程
int main(int argc,char *argv[])
{
	if(argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " process_num"<<std::endl;
		exit(USAGE_ERR);
	}
	int num = std::stoi(argv[1]);
	std::vector<pid_t> subs;
	//创建多进程
	CreateChildProcess(num,&subs,Hello);
	//父进程
	WaitAllChild(subs);
	
	return OK;
}
//typedef void (*handler_t)(); //函数指针类型
//std::vector<handler_t> handlers; //函数指针数组
//void fun_one(){
//	printf("这是一个临时任务1\n");
//}
//void fun_two(){
//	printf("这是一个临时任务2\n");
//}
//void Load(){
//	handlers.push_back(fun_one);
//	handlers.push_back(fun_two);
//}
//void handler(){
//	if (handlers.empty())
//		Load();
//	for (auto iter : handlers)
//		iter();
//}
//int main(){
//	pid_t id = fork();
//	if(id<0)
//	{
//		printf("%s fork error\n",__FUNCTION__);
//		return 1;
//	}
//	else if(id == 0)
//	{
//		//child
//		printf("子进程正在运行中：pid: %d\n",getpid());
//		sleep(5);
//		exit(1);
//	}
//	else{
//		int status = 0;
//		pid_t ret = 0;
//		do{
//			ret = waitpid(-1,&status,WNOHANG);//非阻塞式等待
//			if(ret == 0){
//				printf("child is running\n");
//			}
//			handler();
//		}while(ret == 0);
//
//		if(WIFEXITED(status) && ret == id){
//			printf("等待子进程5S成功，子进程的返回状态为：%d.\n",
//					WEXITSTATUS(status));
//		}else{
//			printf("等待子进程失败，返回.\n");
//			return 1;
//		}
//	}
//	return 0;
//
//}
