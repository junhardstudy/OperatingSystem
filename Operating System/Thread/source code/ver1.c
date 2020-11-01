#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

volatile int global_number;
int weight = 10;//for performing add, and sub 10, 100, 1000 times.
int flag_mode = 0;//to run program normal mode(flag_mode = 0) or build_mode(flag_mode = 1)for monitoring variable, status, and thread information.

void *add(void* data){
	/*********************************************************
	 * This function is add 1 to global variable for ten times.
	 * Also this function run by thread in main()function.
	 *
	 * Variables
	 * i : for control loop
	 * pid : for current process id
	 * tid : for current thread id
	 * thread_name : to inform what is current thread which is
	 				 call by main() function
	 *********************************************************/
	
	int i;
	pid_t pid;
	pthread_t tid;
	char* thread_name;


	if(flag_mode == 1){
		pid = getpid();
		tid = pthread_self();
		thread_name = (char*)data;
		printf("current thread name : %s\n", thread_name);
		printf("process id : %d\n", (unsigned int)pid);
		printf("current thread id : %d\n", (unsigned int)tid);
	}

	for(i = 0; i < weight; i++)global_number += 1;

}

void *sub(void *data){
	/****************************************************************
	 * This function is subtract 1 to global variable for ten times.
	 * Also this function run by thread ins main() function.
	 *
	 * Variables
	 * i : for control loop
	 * pid : for current process id
	 * tid : for current thread id
	 * thread-name : to inform what is current thread which is call
	 * 				 by main() function
	 ****************************************************************/
	
	int i;
	pid_t pid;
	pthread_t tid;
	char* thread_name;

	if(flag_mode == 1){
		pid = getpid();
		tid = pthread_self();
		thread_name = (char*)data;
		printf("current thread name : %s\n", thread_name);
		printf("processid : %d\n", (unsigned int)pid);
		printf("current thread id : %d\n", (unsigned int)tid);
	}

	for(i = 0; i < weight; i++)global_number -= 1;

}

int main(int argc, char* argv[]){
	/*********************************************************************
	 * The main() function is control whole program flow.
	 * The Whole flow is make 5000 add, and sub thread to main argument 
	 * integer.
	 * And above is repeated by 30 times about main() argument integer.
	 * If second argument is "build_mode", then program will be run 
	 * debuggin mode which is show process id, thread id, and name.
	 *
	 * Variables:
	 * p_thread1 & p_thread2 : for storing thread data type.
	 * thr_id1 & thr_id2 : for storing created thread id, and this is used
	 * 						for error handling
	 * i & j & k: control variables in loop operation.
	 *********************************************************************/
	
	pthread_t p_thread1[5000];
	pthread_t p_thread2[5000];
	int thr_id1;
	int thr_id2;
	int i, j, k;//control variables in loop operation.

	char p1_name[] = "Add_thread";
	char p2_name[] = "Sub_thread";

	
	if(argc == 3 && !strcmp(argv[2], "build_mode")){//activate build(debugging) mode.
		flag_mode = 1;
	}
	else{
		if(argc != 2){
			printf("plz, type one integer number for main arguments!\n");
			exit(-1);
		}
	}

	for(k = 10; k <= 1000; k = k*10){
		weight = k;
		printf("%d start\n", weight);
		for(j = 0; j < 30; j++){
			global_number = atoi(argv[1]);
			if(flag_mode == 1)printf("initial global variable : %d\n", global_number);

			for(i = 0; i <5000; i++){
				thr_id1 = pthread_create(&p_thread1[i], NULL, add, (void*)p1_name);
				thr_id2 = pthread_create(&p_thread2[i], NULL, sub, (void*)p2_name);
				if(thr_id1 < 0 || thr_id2 < 0){
					perror("thread create error!\n");
					exit(0);
				}
	
			}
	
			for(i = 0; i < 5000; i++){
				pthread_join(p_thread1[i], NULL);
				pthread_join(p_thread2[i], NULL);		
			}

		printf("global = %d\n", global_number);
		
		}
	}
	printf("main process is done!\n");

	return 0;
}
