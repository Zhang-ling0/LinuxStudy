#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
int main()
{
	printf("stdin->fd: %d\n",stdin->_fileno);
	printf("stdout->fd: %d\n",stdout->_fileno);
	printf("stderr->fd: %d\n",stderr->_fileno);
//	const char *msg = "hhhhhhh\n";
//	write(1,msg,strlen(msg));
// 	int fd = open("log.txt",O_WRONLY|O_CREAT|O_TRUNC,0666);
// 	int fd1 = open("log.txt",O_WRONLY|O_CREAT|O_TRUNC,0666);
// 	int fd2 = open("log.txt",O_WRONLY|O_CREAT|O_TRUNC,0666);
//	if(fd<0)
//	{
//		perror("open");
//	}
//	printf("fd: %d\n",fd);
//	printf("fd1: %d\n",fd1);
//	printf("fd2: %d\n",fd2);
//	const char *msg = "123";
//	write(fd,msg,strlen(msg));//要不要加一，不需要，\0是我们C语言的规定，和你的文件有什么关系？？
//	close(fd);
	return 0;
}



////只有一个比特位是1
//#define ONE   (1<<0)
//#define TWO   (1<<1)
//#define THREE (1<<2)
//#define FOUR  (1<<3)
//
//void Print(int flag)
//{
//	if(flag & ONE)
//		printf("one\n");
//	if(flag & TWO)
//		printf("two\n");
//	if(flag & THREE)
//		printf("three\n");
//	if(flag & FOUR)
//		printf("four\n");
//}
//int main()
//{
//	Print(ONE);
//	printf("\n");
//	Print(ONE|TWO);
//	printf("\n");
//	Print(ONE|TWO|THREE);
//	printf("\n");
//	Print(ONE|TWO|THREE|FOUR);
//	printf("\n");
//	Print(ONE|FOUR);
//	printf("\n");
//	return 0;
//}

//int main(int argc,char *argv[])
//{
//	if(argc != 2){
//		printf("Usage: %s filename\n",argv[0]);
//		return 1;
//	}
//	const char *filename = "log.txt";
//	FILE *fp = fopen(filename,"a");
//	if(fp == NULL)
//	{
//		perror("fopen");
//		return 1;
//	}
//	int cnt = 1;
//	while(cnt <= 10)
//	{
//		cnt++;
//		const char *s = "hello word\n";
//		fputs(s,fp);
//	}
//	while(1)
//	{
//		char buffer[128];
//		if(!fgets(buffer,sizeof(buffer),fp))
//			break;
//		printf("from file: %s\n",buffer);
//	}
//	fclose(fp);
//	return 0;
//}
