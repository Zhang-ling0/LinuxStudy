#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include<string.h>
int main(){
	//向显示器打印字符串，有几种做法？
	//C库函数
	printf("hello printf\n");
	fprintf(stdout,"hello fprintf\n");
	const char *s = "hello fputs\n";
	fputs(s,stdout);
	//系统调用
	const char *ss = "hello write\n";
	write(1,ss,strlen(ss));
	fork();	
//	printf("hello word");
//	sleep(3);
//	exit(0);	
//	close(1);
//        int fda = open("log1.txt",O_WRONLY|O_CREAT|O_TRUNC,0666);
//        printf("fda: %d\n",fda);
//        printf("fda: %d\n",fda);
//        printf("fda: %d\n",fda);
//        printf("fda: %d\n",fda);
	//fflush(stdout);
        //close(fda);
//    const char *filename = "log.txt";
//    int fd = open(filename, O_RDONLY);
//    if(fd < 0)
//    {
//        perror("open");
//        return 1;
//    }
//    char buffer[1024];
//    ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
//    if(n > 0)
//    {
//        buffer[n] = 0;  // 0 == '\0'
//        printf("buffer: %s\n", buffer);
//    }
//    close(fd);
    return 0;
}
