#include<stdio.h>
#include<unistd.h>
int main(){
	printf("hhhhhh\r\n");
	fflush(stdout);
	sleep(3);
    printf("修改");
	return 0;
}

