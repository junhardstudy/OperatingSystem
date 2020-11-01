# Process
프로세스는 메모리에 load되어 실행되고 있는 프로그램의 인스턴스를 의미 합니다. 프로세스에 대한 메모리 구조는 아래와 같은데,
	
![processmemoryview](./image/processmemoryview.jpg)

프로세스는 생성될 때 마다, 또는 동일 프로세스에 대해 fork할 때 마다 각각 전용 code, data, stack, heap영역을 할당 받습니다.

리눅스에서는 fork()함수를 통해 새로운 프로세스를 생성할 수 있습니다. 이 때 부모 프로세스를 fork하게 현재 까지 수행한 상태의 자신(부모)의 프로세스를
복제하여 자식프로세스를 생성하게 됩니다.  
pid : process identifier로 os가 각 프로세스에게 부여하는 unique한 일련 번호인데
ppid : 부모프로세스의 id 입니다. 어떤 프로세스로 부터 생성이 되었는지를 알려준다.
pgid : 프로세스가 어느 그룹에 포함되어 있는지에 대한 정보를 알려줍니다. 보통 부모프로세스의 ppid가 pgid가 됩니다.

## 리눅스 프로세서 API 함수들

1. execl(const char*, const char *, ...)
	리눅스에서는 execl()함수를 통해 새로운 프로세스를 실행시킬 수 있습니다.
	단, 이때 아래와 같이 현재 프로세스의 실행 분기을 덮어씌우게 되어, exectest실행 파일이 표시해야할 print문구가 나타나지 않게 됩니다.
	
	```
	exectest.c
	```
	
	```
	targetprocess.c
	```
	<pre><font color="#8AE234"><b>jun@jun-VirtualBox</b></font>:<font color="#729FCF"><b>~/Desktop/study</b></font>$ ./exectest
	target process is executed!
	</pre>


2. fork()
	```
	targetprocess.c
	```
	
3. wait()
	부모 프로세스와 자식 프로세스 간의 synchronization을 하기 위해 사용됩니다. 자식 프로세스의 실행이 완전히 종료되었는지를 알고자 할 때 사용되는데,
	부모 프로세스에서 자식 프로세스가 완전히 종료되면 부모 프로세스가 종료되게 하여 동기화를 실현할 수 있습니다.
	
아래 소스코드는 부모 프로세스에서 자식 프로세스를 fork한뒤에 exec()함수를 사용하여, 생성된 자식 프로세스들이 다른 프로세스를 실행하는 프로그램 입니다.
```

```

	
# Thread
스레드는 프로레스내에서 실행되는 여러 흐름의 단위를 의미 합니다. 스레드에 대한 메모리 구조는 아래와 같은데,

![threadmemoryview](./image/threadmemoryview.jpg)

스레드는 프로세스와 달리 프로세스 내에서 각 스레드마다 stack만 따로 할당 받고, code, data, heap영역은 공유하게 됩니다. 