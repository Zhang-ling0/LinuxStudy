#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<string.h>
#define MAXARGS 32
#define MAXSIZE 128
// shell自己内部维护的第一张表：命令行参数表
// 故意设置成全局的
char *gargv[MAXARGS];
int gargc = 0;//shell内部维护一张命令行参数表
const char *gsep = " ";//双引号的空格
//我们shell自己的工作路径
char cwd[MAXSIZE];
//第一步：输出一个命令行
//分割符不是一个字符，而是一个字符串，直接写个空格上去会报错的
const char *GetUserName()
{
	char *name = getenv("USER");
	if(name == NULL)
		return "None";
	return name;
}
const char *GetHostName()
{
	char *hostname = getenv("HOSTNAME");
	if(hostname == NULL)
		return "None";
	return hostname;
}
const char *GetPwd()
{
	char *pwd = getenv("PWD");
	//char *pwd = getcwd(cwd,sizeof(cwd));
	if(pwd == NULL)
		return "None";
	return pwd;
}
void PrintfCommandLine()
{
	//如果不加\r\n就会一直在缓冲区里不打印
	printf("[%s@%s %s]# ",GetUserName(),GetHostName(),GetPwd());//用户名 @主机名 当前路径，可以调用对应的系统中的
	fflush(stdout);
}
int GetCommand(char commandline[],int size)
{

	//2.1 用户输入的时候,至少会摁一下回车会捕获到\n
	if(NULL == fgets(commandline,size,stdin))
		return 0;//这里的意思是获取命令行失败，你回去重新打一次让我重新获取
	commandline[strlen(commandline)-1]='\0';//直接将最后一个原本是\n的字符复制为\0
		return strlen(commandline);
}
int ParseComand(char commandline[])
{
	
	gargc = 0;
	//故意写成=
	memset(gargv,0,sizeof(gargv));//每一次都要将该表清空，不然会重复记录
	gargv[0] = strtok(commandline,gsep);//????
	while((gargv[++gargc] = strtok(NULL,gsep)));//先清空，再赋值、最后判断是否为空
	//printf("gargc: %d\n",gargc);
	//int i =0;
	//for(; gargv[i];i++)
	//	printf("gargv[%d]: %s\n",i,gargv[i]);
	return gargc;//解析文件也有可能解析失败
}
int ExecuteCommand(){
	//能不能让你的bash自己执行命令：ls -a -l
	//不能，程序替换一执行，你的shell就没了，你怎么给命令行进行解析
	//必须创建子进程来进行程序替换
	pid_t id = fork();
	if(id<0)
		return -1;
	else if(id == 0)
	{
		//子进程:如何执行，gargv,gargc这个时候再做程序替换
		execvp(gargv[0],gargv);
		exit(1);
	}
	else
	{
		//父进程
		int status = 0;
		pid_t rid = waitpid(id,&status,0);
		if(rid >0)
		{
			//printf("wait child process success!\n");
			//TODO
		}
	}
	return 0;

}
//return val;
//0:不是内建命令
//1：内建命令&&执行完毕
int CheckBuiltinExecute()
{
	//我们怎么判断是否是内建命令呢？
	if(strcmp(gargv[0],"cd") == 0)
	{
		//内建命令
		if(gargc ==2)
		{
			//新的目标路径：gargv[1]
			//1.更改进程内核中的路径
			chdir(gargv[1]);
			//2.更改环境变量
			char pwd[1024];
			getcwd(pwd,sizeof(pwd));//
			snprintf(cwd,sizeof(cwd),"PWD = %s",pwd);//内建路径切换的时候，格式化输出,等于要将路径显示为：cwd=/home/...
								 //将当前工作路径保存到缓冲区中
			putenv(cwd);//将环境变量导出到当前进程的上下文中
		}
	return 1;
	}
	return 0;
}
int main()
{
	//用一个缓冲区来接收我们的命令行
	char command_line[MAXSIZE] = {0};
	while(1)
	{
		//1.打印命令行字符串 
		PrintfCommandLine();
		//2.获取用户输入
		if(0== GetCommand(command_line,sizeof(command_line)))
			continue;
		//上面实际上是一串字符，下面要将这一串字符划分
		//3. 解析字符串->"ls -a -l"→"ls" "-a" "-l命令行解释器，就要对用户输入的命令字符串进行解析" 
		ParseComand(command_line);
		//4.这个命令，到底是让父进程bash自己执行（内建命令）？还是让子进程执行？
		if(CheckBuiltinExecute() != 0)
		{
		//检查是否是内建命令
			continue;
		}

		//5. 让子进程执行这个命令
		//printf("%s\n",command_line);
		ExecuteCommand();
	}
	return 0;
}
