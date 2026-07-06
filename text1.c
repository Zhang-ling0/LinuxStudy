#include<stdio.h>
#include<unistd.h>
int main(){
	int count=10;
	while(count>=0){
	printf("%-2d\r",count);
	fflush(stdout);
	sleep(3);
	count--;
	}
	printf("\n");
	return 0;
}
