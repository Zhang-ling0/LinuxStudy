#pragma once
#include<stdio.h>
#define SIZE 1024
#define NON_BUFFER 1//1
#define LINE_BUFFER 2//10
#define FULL_buffer 4//100 位图的形式表示
typedef struct -MyFILE
{
	int fd;//文件描述符
	int flag;//打开模式是什么？
	int flash_mode;
	char outbuffer[SIZE];//缓冲区
	int pos;//buffer
	int cap;
	
}myFILE;

myFILE *myfopen(const char *pathname,const char *mode);//r，w,a,r+,w+
void myfclose(myFILE *fp);
int myfputs(const char* str，myFILE *fp);
void myffluesh(myFILE *fp); 
