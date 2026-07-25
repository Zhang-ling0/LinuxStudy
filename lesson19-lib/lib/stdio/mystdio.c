#include "mystdio.h"
#include<stdio.h>
#include<sys/stat.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<fcntl.h>
#include<unistd.h>
#define TRY_FLUSH 1
#define  MUST_FLUSH 2
myFILE *myfopen(const char *pathname,const char *mode)
{
	int fd = -1;
	int flags = 0; 
	if(strcmp(mode,"r")==0)
	{
		flags = O_RDONLY;
		fd = open(pathname,flags,MODE);//以只读的方式写入
	}
	else if(strcmp(mode,"w")==0)
	{
		flags = O_WRONLY|O_CREAT|O_TRUNC;
		fd = open(pathname,flags,MODE);	
	}
	else if(strcmp(mode,"a")==0){
		flags = O_WRONLY|O_CREAT|O_APPEND;
		fd = open(pathname,flags,MODE);	
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
	fp->flags = flags;
	fp->flush_mode = LINE_BUFFER;
	fp->pos = 0;
	fp->cap = SIZE;

	memset(fp->outbuffer,0,SIZE);

	 return fp;
}
static void myfflushcore(myFILE *fp,int flag)
{
	if(fp->pos==0)
		return;//这个时候就不用刷新了
	if((fp->flush_mode&LINE_BUFFER)||(flag & MUST_FLUSH))//行刷新
	{
	//“aasdf\n”
	if(fp->outbuffer[fp->pos-1]=='\n'||(flag & MUST_FLUSH))
	{
		//写到内核当中
		write(fp->fd,fp->outbuffer,fp->pos);
		fp->pos = 0;//清空缓冲区	
	}
	}
	if(fp->flush_mode&FULL_BUFFER)//全缓冲
	{
		//判断条件是是否写满了
		if(fp->pos>=fp->cap||(flag & MUST_FLUSH)){
			write(fp->fd,fp->outbuffer,fp->pos);
			fp->pos = 0;
			memset(fp->outbuffer,0,SIZE);
		}
	}
	if(fp->flush_mode&NON_BUFFER){
		//无刷新模式
	}

}
void myfflush(myFILE* fp)
{
	myfflushcore(fp,MUST_FLUSH);
}

int myfputs(const char* str,myFILE *fp){
	if(strlen(str)==0)
		return 0;//防止写入的字符串为空
	if(fp->pos+strlen(str)>=fp->cap){
		myfflushcore(fp,MUST_FLUSH);
	}
	//暂时不想刷新就先不加\n
	//step1：向文件流里面写，本质是：文件缓冲区->拷贝
	memcpy(fp->outbuffer+fp->pos,str,strlen(str));//追加拷贝
	fp->pos += strlen(str);//更新读写位置
	//step2：如果条件允许，可以自己刷新
	//添加字符串结束符
	fp->outbuffer[fp->pos] = '\0';
	myfflushcore(fp,TRY_FLUSH);//在这里调用myfllus能刷那就刷新一下不能刷就会直接返回的
	 return strlen(str);	
}
void myfclose(myFILE *fp){
	//1. 把文件关闭要强制刷新
	myfflushcore(fp,TRY_FLUSH);
	//2. 关闭文件
	close(fp->fd); 
	//3. free
	free(fp);
}

