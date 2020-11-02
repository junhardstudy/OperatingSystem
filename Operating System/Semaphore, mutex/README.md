## pthread_mutex 관련 API 함수들
```c
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
```
1. pthread_mutex_init()
	mutex를 초기화하는 함수입니다. 2번째 인자 값은 해당 mutex의 속성을 주고, default는 NULL입니다. 
	
2. pthread_mutex_lock()
	해당 critical section을 access하는 스레드가 lock하는 함수입니다.

3. pthread_mutex_unlock()
	lock 되어 있는 해당 critical section을 unlcok해주는 함수입니다.
***

```c
int main(int argc, char* argv[]){
	pthread_t p_thread1[10];
	pthread_t p_thread2[10];
	int thr_id1;
	int thr_id2;
	int i, j, k, total_process;//control variables in loop operation.

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

	for(total_process = 0 ; total_process < 2; total_process++){
		if(mutex_flag == 1)printf("apply mutex\n");
	for(k = 10; k <= 1000; k = k*10){
		weight = k;
		printf("%d start\n", weight);
		for(j = 0; j < 30; j++){
			global_number = atoi(argv[1]);
			if(flag_mode == 1)printf("initial global variable : %d\n", global_number);

			for(i = 0; i <10; i++){//create total 20 thread and perform. 
				thr_id1 = pthread_create(&p_thread1[i], NULL, add, (void*)p1_name);
				thr_id2 = pthread_create(&p_thread2[i], NULL, sub, (void*)p2_name);
				if(thr_id1 < 0 || thr_id2 < 0){
					perror("thread create error!\n");
					exit(0);
				}//if thread can'be created, then error message is show, and terminate program.
	
			}
	
			for(i = 0; i < 10; i++){
				pthread_join(p_thread1[i], NULL);
				pthread_join(p_thread2[i], NULL);		
			}//wait untile total 20 thread is done.

		printf("global = %d\n", global_number);
		
		}
	}
	mutex_flag = 1;//switch to show result by applying mutex.
	}
	if(mutex_flag == 1)pthread_mutex_destroy(&mutex);//return and destroy mutex resource.
	printf("main process is done!\n");

	return 0;
}
```
main( )에서 argument로 정수 하나를 받아, 그 정수에 대해 1을 총 10번 더하는 add( )함수와 1을 총 10번 빼는 sub( )함수를 각각 10개의 쓰레드로 생성하여, 
총 20개의 쓰레드가 작업을 합니다. 이 때 전역변수의 값이 참조되고 대입 되는 지점까지는 critical section이므로 pthread_mutex관련 함수 및 라이브러리를 이용하여, 
이를 방지한 프로그램 입니다.

main()에서는 필수로 한개의 정수 argument를 받야하며, 해당 정수값은 스레드들이 이용할 공유 전역 변수인 global에 저장됩니다.

main()에서 2번째 argument로 "build_mode"문자열을 받게 되면, 현재 실행되는 스레드의 name과 id, 그리고 프로세스 id가 출력됩니다.

### 출력 mode
```c
if(mutex_flag == 1)printf("apply mutex\n");
.
.
.
mutex_flag = 1;
```
처음에는 mutex를 적용하지 않고 공유 resource에 대해서 여러 스레드가 access하는 경우를 출력하고
그 다음으로 flag_mode를 1로 바꾸어, mutex가 적용되었을 때의 결과를 출력하게 됩니다.
<br>
<br>
<br>

```c
if(argc == 3 && !strcmp(argv[2], "build_mode")){//activate build(debugging) mode.
	flag_mode = 1;
}
```
main()에서 2번째 argument로 "build_mode"를 입력받게 되면, 현재 실행되고 있는 스레드 name, id, 그리고 프로세스의 id가 출력되게 됩니다.

### Critical section

스레드가 공유 변수나 resource의 값을 변경하는 경우와 같은 critical section에서 다른 스레드가 해당 resource에 접근하게 되면 사용자와 프로그래머가 의도치 않은
값을 사용할 수 있습니다.

```c
void *add(void* data){	
	int i;
	pid_t pid;
	pthread_t tid;
	char* thread_name;

	int tmp;

	if(flag_mode == 1){
		pid = getpid();
		tid = pthread_self();
		thread_name = (char*)data;
		printf("current thread name : %s\n", thread_name);
		printf("process id : %d\n", (unsigned int)pid);
		printf("current thread id : %d\n", (unsigned int)tid);
	}

	if(mutex_flag == 1)pthread_mutex_lock(&mutex);//create lock
	tmp = global_number;//critical section.
	for(i = 0; i < weight; i++){
		tmp++;		
	}

	for(i = 0; i < 50; i++){
	meaninglessOperation();
	}//!!!!global variable highly broken by this function!

	global_number = tmp;
	if(mutex_flag == 1)pthread_mutex_unlock(&mutex);//disable lock.

}
```

```c
void *sub(void *data){
	int i;
	pid_t pid;
	pthread_t tid;
	char* thread_name;

	int tmp;


	if(flag_mode == 1){
		pid = getpid();
		tid = pthread_self();
		thread_name = (char*)data;
		printf("current thread name : %s\n", thread_name);
		printf("processid : %d\n", (unsigned int)pid);
		printf("current thread id : %d\n", (unsigned int)tid);
	}
	

	if(mutex_flag == 1)pthread_mutex_lock(&mutex);//create lock
	tmp = global_number;//critical section
	for(i = 0; i < weight; i++){
		tmp--;		
	}

	for(i = 0; i < 50; i++){
	meaninglessOperation();
	}//!!!!!global variable highly broken by this function!

	global_number = tmp;
	if(mutex_flag == 1)pthread_mutex_unlock(&mutex);//disalbe lock

}
```

위 add(), sub() 함수에서는 아래와 같은 코드 section이 critical section이 됩니다.
```c
tmp = global_number;//critical section.
```
이를 해결하기 위해 해당 공유 전역 변수인 global_number의 값이 변경되는 순간에는 다른 스레드가 접근을 할 수 없게 하여야 합니다.

```c
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
```
우선 mutx lock을 하기 전에 모든 lock들을 초기화하는게 필요합니다. 
PTHREAD_MUTEX_INITIALIZER는 해당 mutx를 default 속성으로 초기화 시킵니다.

단, dynamic mutex와 같이 해당 구조체를 선언하고 나중에 초기화 하려면 아래와 같이 하여야 합니다.
```c
pthread_mutex_t *mutex;
.
.
.
mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
pthread_mutex_init(mutex, NULL); 
```
<br>
<br>
<br>
<br>

```c
if(mutex_flag == 1)pthread_mutex_lock(&mutex);
/*********************************************
			critical section
*********************************************/
if(mutex_flag == 1)pthread_mutex_unlock(&mutex);
```
critical section 전,후로 mutex lock을 하여 어떤 스레드가 global_number값을 변경하는 동안은 다른 스레드가 해당 전역변수에 접근하지 못하게 됩니다.

### 결과 화면

<pre>global = -1993
global = 2007
global = 2007
global = -993
global = 7
global = 2007
global = 1007
apply mutex
10 start
global = 7
global = 7
global = 7
global = 7
global = 7</pre>

mutex lock을 하기전에는 여러 스레드가 global 변수에 access하기 때문에 의도하지 않은 값이 발생하지만,
mutex lock을 적용한 이후에는 어떤 스레드가 global 변수에 access 할 때는 다른 스레드가 access할 수 없어 의도한 값이 나오게 됩니다. 


