#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

void *thrfunc();

volatile int global_resource = 10;
pthread_mutex_t mutex;

void main(){
	pthread_t p_thread[2];
	int i;

	pthread_mutex_init(&mutexm, NULL);

	for(i = 0; i < 2; i++){
		pthread_create(&p_thread[i], NULL, thrfunc, NULL);
	}

	for(i = 0; i < 2; i++){
		pthread_join(p_thread[i], NULL);
	}

}

void *thrfunc(){
	int i=0;

	pthread_mutex_lock(&mutex);
	for(i = 0; i < 10; i++){
		global_resource++;
	}
	for(i = 0; i < 10; i++){
		global_resource--;
	}
	pthread_mutex_unlock(&mutex);

	printf("value = %d\n",global_resource); 
}
