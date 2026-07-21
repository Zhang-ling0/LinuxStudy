#include "mystdio.h"
#include<stdio.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<fcntl.h>
#include<unistd,h>


myFILE *myfopen(const char *pathname,const char *mode)
{
	int fd == -1;
	if(strcmp(mode,"r"==0)
	{
		fd = open(pathname,O_RDONLY);//以只读的方式写入
	}
	else if(strcmp(mode,"w"==0){
		fd = open(pathname,O_WRONLY|O_CREAT|O_TRUNC);	
	}
	else if(strcmp(mode,"a"==0){
		fd = open(pathname,O_WRONLY|O_CREAT|O_APPEND);	
	}
	else{
		//TODO
	}
	if(fd < 0)
	  return NULL;
	//返回myFILE结构体对象
	myFILE *fp = (myFILE *)malloc(sizeof(myFILE));
	if(fp == NULL)
		return NULL;
	fp->fd = fd;

	 return NULL;
}
int myfputs(const char* str，myFILE *fp){
	if(strlen(str)==0)
		return 0;//防止写入的字符串为空
	//暂时不想刷新就先不加\n
	//step1：向文件流里面写，本质是：文件缓冲区->拷贝
	memcpy(fp->outbuffer+fp->pos,str,strlen(str));\
	fp->pos += strlen(str);
	//step2：如果条件允许，可以自己刷新
	myfflush(fp);//在这里调用myfllus能刷那就刷新一下不能刷就会直接返回的
	 return NULL;	
}
void myfclose(myFILE *fp){
	//1. 把文件关闭要强制刷新
	myfflushCORE(fp,TRY_FLUSH);
	//2. 关闭文件
	close(fp->fd);
	//3. free
	free(fp);
	return 0;
}
void myfflush(myFILE* fp)
{
	
}
#define TRY_FLUSH 1
#define  MUST_FLUSH 2
void myfflush(myFILE *fp)
{
	if(fp->pos==0)
		return;//这个时候就不用刷新了
	if(fd->flush_mode&LIENE_BUFFER)//行刷新
	{
	//“aasdf\n”
	if(fp->outbuffer[fp->pos-1]=='\n')
	{
		//写到内核当中
		write(fp->fd,fp->outbuffer,fp->pos);
		fp->pos = 0;//清空缓冲区	
	}
	}
	if(fd->flush_mode&FULL_BUFFER)//全缓冲
	{
		//判断条件是是否写满了
	}
	if(fd->flush_mode&NON_BUFFER){
		//无刷新模式
	}

}
