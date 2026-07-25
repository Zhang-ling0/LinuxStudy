#include"mystdio.h"
#include"mystring.h"
#include<unistd.h>
int main()
{
	myFILE *fp = myfopen("log.txt","w");
	if(fp == NULL)
	{
		printf("myfopen error!\n");
		return 0;
	}

	//const char *msg = "hello";
	//int cnt = 10;
	//while(cnt--){
	//	myfputs(msg,fp);
	//	sleep(1);
	//	printf("debug:outbuffer =  %s,pos = %d\n",fp->outbuffer,fp->pos);
	//}

	//myfclose(fp);
	mystrlen();
	return 0;
}

