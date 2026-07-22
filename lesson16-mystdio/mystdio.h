#pragma once
#include<stdio.h>
#define SIZE 1024
#define NON_BUFFER 1//1
#define LINE_BUFFER 2//10
#define FULL_BUFFER 4//100 位图的形式表示
//默认模式MODE
#define MODE 0666

typedef struct myFILE
{
	int fd;//文件描述符
	int flags;//打开模式是什么？
	int flush_mode;
	char outbuffer[SIZE];//缓冲区
	int pos;//buffer
	int cap;
	
}myFILE;

myFILE *myfopen(const char *pathname,const char *mode);//r，w,a,r+,w+
void myfclose(myFILE *fp);
int myfputs(const char *str,myFILE *fp);
void myfflush(myFILE *fp); 
