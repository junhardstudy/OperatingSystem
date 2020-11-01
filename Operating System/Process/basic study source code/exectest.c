#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>


void main(){
	execl("/home/jun/Desktop/study/targetprocess","/home/jun/Desktop/study/targetprocess" , NULL);
	printf("but this statement will be not printed!\n");
}
