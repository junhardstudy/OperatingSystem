# Thread
스레드는 프로레스내에서 실행되는 여러 흐름의 단위를 의미 합니다. 스레드에 대한 메모리 구조는 아래와 같은데,

![threadmemoryview](./image/threadmemoryview.jpg)

스레드는 프로세스와 달리 프로세스 내에서 각 스레드마다 stack만 따로 할당 받고, code, data, heap영역은 공유하게 됩니다. 

## 리눅스 스레드 API 함수들

1. pthread_create()함수

2. pthread_exit()

3. pthread_join()

