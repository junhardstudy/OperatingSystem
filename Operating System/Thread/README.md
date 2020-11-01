# Thread
스레드는 프로레스내에서 실행되는 여러 흐름의 단위를 의미 합니다. 스레드에 대한 메모리 구조는 아래와 같은데,

![threadmemoryview](./image/threadmemoryview.jpg)

스레드는 프로세스와 달리 프로세스 내에서 각 스레드마다 stack만 따로 할당 받고, code, data, heap영역은 공유하게 됩니다. 

## 리눅스 스레드 API 관련 함수들

1. pthread_create()함수
	기본 스레드 생성함수로, 첫번째 인자는 스레드가 성공적으로 생설되었을때 생성된 스레드의 식별자를 가지게 됩니다. 
	2번째 인자는 스레드 특성을 이용할 때 넘겨주는 인자 입니다. 3번째 인자는 스레드가 생성되면서 실행할 스레드 함수이며, 
	4번째 인자는 그 때의 스레드 함수의 인자입니다.
	
	```
	소스코드
	```
	
2. pthread_exit()
	현재 실행중인 스레드를 종료할 때 사용하는 함수입니다. 
	
	```
	소스코드
	```

3. pthread_join()
	특정 스레드가 종료될 때까지 기다리다가 해당 스레드가 종료시 resource를 해제 시켜줍니다. 첫번째 인자는 기다릴 스레드 pthread_t가 들어가고
	두번째 인자는 pthread_exit()함수가 전달한 종료 값을 전달해주게 됩니다.
	
	```
	소스코드
	```

***

아래 프로그램은 main()에서 argument로 정수 하나를 받아, 그 정수에 대해 1을 총 10번 더하는 add()함수와 1을 총 10번 빼는 sub()함수를 각각 
5000개의 스레드로 생성하여, 총 10000개의 스레드가 작업을 완료할 때 그 결과값을 나타내는 프로그램 입니다. 해당 결과값은 올바른 결과값이 아닌데, 이는
동기화 문제로 본 저장소에서 Semaphore, mutex 디렉토리에 관련된 내용이 있습니다.

### main() 함수

```c
int main(int argc, char* argv[]){	
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
```

여기서 스레드를 생성하는 pthread_create()함수를 호출할 때 4번째 인자로 함수 파라미터 연습으로 값을 넘겨주었습니다. 
