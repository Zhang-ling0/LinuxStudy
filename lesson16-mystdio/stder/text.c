#include<stdio.h>
#include<unistd.h>
#include<string.h>

int main()
{
	//标准输出：1
	printf("这是一个正常消息\n");
	fprintf(stdout,"这也是一个正常的日志消息\n");
	const char *s1 = "这是一个正常消息，write\n";
	write(1,s1,strlen(s1));

	//标准错误：2
	fprintf(stderr,"这是一个错误消息\n");
	const char *s2 = "这是一个错误消息，write\n";
	write(2,s2,strlen(s2));
	perror("perror,hello");
	return 0;
}
