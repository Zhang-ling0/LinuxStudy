#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
int main()
{
	close(0);
	int fda = open("log1.txt",O_WRONLY|O_CREAT|O_APPEND,0666);
	printf("fda: %d\n",fda);//3
	dup2(fda,1);
	//stdout->1
	printf("lalalalalala\n");
//	printf("hahahahahaha\n");
//	int fda = open("log1.txt",O_RDONLY);
//	printf("fda: %d\n",fda);//0
//	dup2(fda,0);
//	char buffer[64];
//	fgets(buffer,sizeof(buffer),stdin);//stdin->0,只认数字0
//	printf("%s\n",buffer);
//	printf("fda: %d\n",fda);
//	printf("fda: %d\n",fda);
//	printf("fda: %d\n",fda);
//	printf("fda: %d\n",fda);
//	printf("fda: %d\n",fda);
//	printf("fda: %d\n",fda);
//	printf("fda: %d\n",fda);
//	printf("fda: %d\n",fda);
//	close(fda);
//	int fdb = open("log2.txt",O_WRONLY|O_CREAT|O_TRUNC,0666);
//	printf("fda: %d\n",fdb);
//	int fdc = open("log3.txt",O_WRONLY|O_CREAT|O_TRUNC,0666);
//	printf("fda: %d\n",fdc);
	return 0;
}
