#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>


void main(){
	pid_t pid;
	
	printf("pid = %u\nppid = %u\n", getpid(), getppid());
	printf("this statement will be executed one time in parent process\n");
	pid = fork();
	
	printf("this execution will be executed twice(pid:%d)\n", pid);
	if(pid == -1)printf("can't create process!\n");

	if(pid > 0){
		printf("parent process fork return value : %u\n", pid);
		printf("parent pid : %d  ppid : %d\n", getpid(), getppid());
		sleep(1);
	}
	else{
		printf("child process fork return value : %u\n", pid);
		printf("childe pid : %d  ppid : %d\n", getpid(), getppid());
		sleep(1);
	}
}
