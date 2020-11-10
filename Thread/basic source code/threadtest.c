#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

void *target_func(void *);

void main(){
	pthread_t p_thread[10];
	int i;
	int thr_id;
	int* value;

	value = (int*)malloc(sizeof(int));
	*value = 0;

	for(i=0; i<10; i++){
		thr_id = pthread_create(&p_thread[i], NULL, target_func, (void*)value);
		
		//sleep(1);
		pthread_join(p_thread[i], NULL);
	}
}


void *target_func(void *value){
	int data;
	data = ++(*(int*)value);
	printf("thread id(%ld) thread function call and count is %d\n", pthread_self(), data);
	pthread_exit(NULL);
}

