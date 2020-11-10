#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

void select_prog();

void main(int argc, char* argv[]){
	/*******************************************************************************
	 If one  argument, 'build_mode', is typed, then program will be run with showing
	 program flow information wich is child process status, parent process status,
	 and current process id number.
	*******************************************************************************/
	pid_t pid;

	pid = fork();
	if((argc == 2) && !strcmp(argv[1], "build_mode")) printf("curren process id(after fork() call) : %d\n", pid);

	if(pid < 0){//if fork() function doesn't work, then program will be shutdown with showing alert statement
		printf("process can't create!, program will be determinated!\n");
		exit(-1);
	}
	else if(pid == 0){//if child process is sucessfully created, then select_prog() function wich is execute other program is called
		if((argc == 2) && !strcmp(argv[1], "build_mode")) printf("current process id : %d\n", pid);
		select_prog();
	}
	else{
		wait(NULL);
		if((argc == 2) && !strcmp(argv[1], "build_mode")){
			printf("current process id : %d\n", pid);
			printf("parent process done!\n");
		}

		//for monitoring program flow, when first argument string is 'build_mode'
	}
}

void select_prog(){
	/*******************************************************************************
		This function is execute other program by using execl() function.

		This function is only running addition(sum), deduction(sub), division(div),
		and factorial(fact).
		If other command enter to this program, then the program will be terminated
		with showing alert message.

		Variables
		1.char buff[255] : for storing command line
		2.char* file_name : for file directory or execute command
		3.char* first, seccond : for storing number which is chainging char into int
	 *******************************************************************************/
	char buf[255];
	char* file_name;
	char* first;
	char* second;

	printf("plz, command line : ");
	scanf(" %[^\n]", buf);
	
	file_name = strtok(buf, " ");
	if(!strcmp(file_name, "./sum") || !strcmp(file_name, "./sub") || !strcmp(file_name, "./div")){
		
		first = strtok(NULL, " ");
		second = strtok(NULL, "\0");

		execl(file_name, file_name, first, second, NULL);
	}
	else if(!strcmp(file_name, "./fact")){
		first = strtok(NULL, "\0");
		execl(file_name, file_name, first, NULL);
		printf("execl error occur!\n");
		exit(-1);
	}
	
	else{
		printf("'%s' is not devised in this program!\n", buf);
		exit(-1);
	}


}
