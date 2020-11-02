# Thread
스레드는 프로레스내에서 실행되는 여러 흐름의 단위를 의미 합니다. 스레드에 대한 메모리 구조는 아래와 같은데,

![threadmemoryview](./image/threadmemoryview.jpg)

스레드는 프로세스와 달리 프로세스 내에서 각 스레드마다 stack만 따로 할당 받고, code, data, heap영역은 공유하게 됩니다.

스레드(들)는 각 스레드가 속한 프로세스내에서는 unique한 id를 가지게 됩니다. 이를 pthread_t 타입으로 구분하게 됩니다.

## 리눅스 스레드 API 관련 함수들

```c
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
```

1. pthread_create()함수

	기본 스레드 생성함수로, 첫번째 인자는 스레드가 성공적으로 생설되었을때 생성된 스레드의 식별자를 가지게 됩니다. 
	2번째 인자는 스레드 특성을 이용할 때 넘겨주는 인자 입니다. 3번째 인자는 스레드가 생성되면서 실행할 스레드 함수이며, 
	4번째 인자는 그 때의 스레드 함수의 인자입니다.
	
	pthread_create()함수의 동작을 살펴보기 위해 10개의 스레드를 생성하는 코드 입니다.
	pthread_create()함수 호출시 void 포인터 함수인 target_func()가 시작되며, command window에 현재 수행되는 스레드 id와
	각 스레드 함수가 전달받은 인수인 value 값을 변경하고, 표시 해 줍니다.
	
	
2. pthread_exit()

	현재 실행중인 스레드를 종료할 때 사용하는 함수입니다. 
	
	
3. pthread_join()

	특정 스레드가 종료될 때까지 기다리다가 해당 스레드가 종료시 resource를 해제 시켜줍니다. 첫번째 인자는 기다릴 스레드 pthread_t가 들어가고
	두번째 인자는 pthread_exit()함수가 전달한 종료 값을 전달해주게 됩니다.

### command 결과
	
<pre><font color="#8AE234"><b>jun@jun-VirtualBox</b></font>:<font color="#729FCF"><b>~/Desktop/study/thread</b></font>$ ./threadtest 
thread id(140545642907392) thread function call and count is 1
thread id(140545634514688) thread function call and count is 2
thread id(140545557001984) thread function call and count is 3
thread id(140545548609280) thread function call and count is 4
thread id(140545540216576) thread function call and count is 5
thread id(140545531823872) thread function call and count is 6
thread id(140545523431168) thread function call and count is 7
thread id(140545515038464) thread function call and count is 8
thread id(140545506645760) thread function call and count is 9
thread id(140545498253056) thread function call and count is 10
<font color="#8AE234"><b>jun@jun-VirtualBox</b></font>:<font color="#729FCF"><b>~/Desktop/study/thread</b></font>$ </pre>
	

***

아래 프로그램은 main()에서 argument로 정수 하나를 받아, 그 정수에 대해 1을 총 10번 더하는 add()함수와 1을 총 10번 빼는 sub()함수를 각각 
5000개의 스레드로 생성하여, 총 10000개의 스레드가 작업을 완료할 때 그 결과값을 나타내는 프로그램 입니다. 해당 결과값은 올바른 결과값이 아닌데, 이는
동기화 문제로 본 저장소에서 Semaphore, mutex 디렉토리에 관련된 내용이 있습니다.

### main() 함수

```c
if(argc == 3 && !strcmp(argv[2], "build_mode")){//activate build(debugging) mode.
	flag_mode = 1;
}
else{
	if(argc != 2){
		printf("plz, type one integer number for main arguments!\n");
		exit(-1);
	}
}
```
main()의 argument로 "build_mode"를 입력받게 되면 현재 실행되고 있는 프로세스의 id(pid)와 스레드 id, 스레드 name을 스레드가 생성 될 때마다 출력하게 됩니다.
<br>
<br>

```c
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
```
본 프로그램은 mutex와 같은 스레드간의 자원공유에 대한 문제를 더욱 부각 시키기 위해, 스레드 생성과 소멸 부분을 k번 만큼 더 반복하도록 하였습니다.
스레드 생성을 위해 pthread_create()함수를 호출하였으며, 각 인자값은 첫번째는 스레드id, attribute는 null, 스레드 생성 시 수행되어야 할 함수인 add() 또는 sub(),
마지막으로 스레드 함수가 파라미터를 받을 수 있다는 것을 부각하기 위해 일부러 char포인터를 넘겨주었습니다.

스레드 생성에 관한 반복문이 끝나게 되면, 생성된 각 스레드가 종료될때까지 기다리기 위해 pthread_join()함수가 호출됩니다.

### add(), sub() 함수

스레드 생성 시 호출되어지는 스레드 함수로서, build_mode인 경우 여기서 해당 스레드 id, name, 프로세스 id가 호출되게 됩니다.

```c
void *add(void* data){	
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
```

```c
void *sub(void *data){
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
```


### 실행 결과중 일부 결과

<pre>100 start
global = 10
global = 10
global = 10
global = 10
global = 10
global = 110
global = -90</pre>

위와 같이 1을 총 10번 더하고 10번 빼게 되면 값의 변화가 없어야 하는데, 값이 변하는 경우가 있습니다. 
이는 global 전역변수에 대한 스레드간의 resource 동기화 문제 입니다.
 