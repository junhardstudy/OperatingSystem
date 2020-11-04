## Socket programming

	* Localhost환경에서 server는 두 개의 client와 연결하여 가위, 바위, 보 게임을 진행하는 프로그램입니다.
	
	* Server process에서는 client가 접속 할 때마다, <strong>해당 client 처리를 위한 스레드를 생성</strong>합니다.
	
	* 도중에 각 client로 부터 가위, 바위, 보 메시지를 받고 저장할 때 critical section이 발생하므로 mutex가 적용되어 있습니다.

	* main의 argument로 사용자가 원하는 port번호를 입력받아 사용할 수 있습니다.
## server.c

각 client로 부터의 연결을 accept하고, 가위, 바위, 보 결과를 처리한 후 각 client에게 대응하는 결과를 보내주는 프로그램입니다.

socket programming에서 server는 socket() -> bind() -> listen() -> accept() -> recv(), or send() -> close()순서로 진행 됩니다.

```c
struct sockaddr_in server_socket_addr;
.
.
.
server_socket_addr.sin_family = AF_INET;
server_socket_addr.sin_addr.s_addr = INADDR_ANY;
```

우선 server socket을 생성하기 전에, socket에 대한 address 정보를 생성하기 위해 sockaddr_in 구조체를 생성합니다.

그리고 그 에 대응하는 멤버값들을 지정해줍니다.
sin_family멤버는 소켓이 통신할 수 있는 주소 체계를 가지고 있는 멤버입니다. AF_INET은 IPv4 인터넷 프로토콜을 의미합니다.

sin_addr.s_addr은 호스트 IP주소 입니다. INADDR_ANY는 서버의 IP주소를 자동으로 찾아서 대입해주는데 0값을 가집니다.
(서버의 경우 여러 클라이언트로부터 접속을 받기 위해 여러 네트워크 인터페이스가 존재하여 ip주소도 여러개가 됨. 따라서 자신으로부터 오는 모든 메시지를 받기 위해 사용.
ipv4 주소체계에서 0.0.0.0은 자신의 호스트에 들어오는 모든 패킷을 수신함.)
<br>
<br><br>

```c
if (argc > 1) { 
    port = atoi(argv[1]); 
}
else {
    port = default_port_num;
}

if (port > 0)
    server_socket_addr.sin_port = htons((u_short)port);
else {
    fprintf(stderr, "bad port number %s/n", argv[1]);
    exit(1);
}   
```
main argument로 포트 번호를 입력받으면, 해당 값을 port번호로 사용하게 됩니다. 
sin_port 멤버는 서버 소켓 address에 저장될 포트번호를 가집니다.(길이는 총 16비트) 저장될 때 host byte order에서 
netwrok byte oreder로 바꿔야 하므로 htons()함수를 이용합니다.
<br><br>
<br>
```c
if (((int)(ptrp = getprotobyname("tcp"))) == 0) {
    printf("can't getprotobyname() function return\n");
    exit(1);
}
port = ptrp->p_proto;
```
TCP 프로토콜에 대응되는 프로토콜 정보를 얻기 위해 getprotobyname()함수를 호출하였으며, 대응되는 protoent 구조체 포인터 ptrp에서 멤버인 p_proto를 통해
TCP 프로토콜에 대응되는 포트 번호를 가져옵니다.

여기까지 server socket을 생성하기 위한 포트번호, 그리고 생성된 socket을 bind하기 위해 필요한 socket에 대한 주소 정보들(sockaddr_in)입니다.
<br><br>
<br>
```c
socketfd = socket(PF_INET, SOCK_STREAM, port);
if (socketfd < 0) {
    printf("can't create socket!\n");
    exit(1);
}
```
소켓을 생성하는 부분입니다.

리눅스에서는 소켓을 파일단위로 관리하고 처리합니다. 따라서 socketfd에 파일디스크럽터 번호를 저장하게 됩니다.

socket() 함수를 호출할 때, 소켓이 사용할 프로토콜인 PF_INET(ipv4 인터넷 프로토콜), 서비스 타입인 TCP type(SOCK_STREAM), getprotobyname에서 구한 port번호인
TCP 프로토콜을 인자로 넘겨줍니다.

소켓이 정상적으로 생성이 되면, 0이상의 값을 return하게 되고 그렇지 않을 경우 에러 메시지를 출력하고 프로그램이 종료됩니다.
<br><br>
<br>
```c
if (bind(socketfd, (struct sockaddr*) & server_socket_addr, sizeof(server_socket_addr)) < 0) {
    printf("can't bind!\n");
    exit(1);
}
```
생성한 소켓에 대한 파일 디스크럽터 번호를 가지고 있는 socketfd와 소켓의 address에 관한 정보를 가지고 있는 구조체(sockaddr_in)를 엮어주는 부분입니다.
인자로서 각각 파일디스크럽터 번호(socketfd), 서버의 IP주소, 그 때의 주소 길이를 넘겨주게 됩니다.
<br><br>
<br>
```c
if (listen(socketfd, max_client_num) < 0) {
    printf("can't invocate listen!\n");
    exit(1);
}
```
이제 client로부터 연결 요청이 들어오는지 확인하기 위해 대기하는 함수 listen()을 호출합니다.
인자로 소켓 파일디스크럽터(socketfd)와 연결 대기열 크기(max_client_num)을 받게 됩니다.
연결 대기열 크기는 연결을 요청한 소켓이 대기하는 연결 대기열의 크기를 의미합니다.
<br><br>
<br>
```c
addr_len = sizeof(client_socket_addr);
printf("Server start, and running...\n");
printf("waiting for connection from client...\n");

while (1) {

    client_socketfd = accept(socketfd, (struct sockaddr*) & client_socket_addr, &addr_len);
	

    if (client_socketfd < 0) {
        printf("something wrong happen in connection with client!\n");
        exit(1);
    }
    else {
	
        pthread_create(&tid, NULL, serverthread, (void*)client_socketfd);
    display_client_info(client_socketfd, tid, "connected");
    pthread_detach(tid);
   	}

}
```
server에서는 server socket뿐만 아니라, 그 때 연결을 요청하는 client에 대한 socket도 필요합니다.
따라서 연결을 요청하는 client에 대한 socket을 생성하는 부분입니다.

accept()의 인자를 보면, 첫번째는 server socket에 대한 파일 디스크럽터 번호가 들어가고 두번째 인자와 세번째 인자가 client socket에 대한 정보가
들어갑니다.

accept()가 정상적으로 수행되면, 연결한 client마다 전용 스레드를 pthread_create를 생성해주게 됩니다. 이 때 스레드 함수에 넘겨줄 인자인 
```c
(void*)client_socketfd
``` 
는 해당 전용 스레드에서 전용 client socket 연결을 종료하는데 사용됩니다.
<br><br>
<br>
```c
void* serverthread(void* parm)
{
    int client_socket_addr;
    char buf[2];
    int your_player_number = 0;
    int your_result;

    client_socket_addr = (int)parm;

   while(1){ 
        recv(client_socket_addr, buf, sizeof(buf), 0);
        pthread_mutex_lock(&mutex);

	your_player_number = current_connection;
	current_connection++;
	
	set_player_info(your_player_number, buf[0]);

	pthread_mutex_unlock(&mutex);

	while(1){
		/* waiting for other player input*/
		if(current_connection >= 2)break;
	}
	
	if(buf[0] == 'Q'){
		display_client_info(client_socket_addr, NULL, "disconnected");
		close(client_socket_addr);
		current_connection = 0;
		pthread_exit(0);
	}

	your_result = proceed_round(your_player_number);
	if(your_result == 1){	
		send(client_socket_addr, win, sizeof(win), 0);
	}
	else if(your_result == 0){
		send(client_socket_addr, tie, sizeof(tie), 0);
	}
	else{
		send(client_socket_addr, loose, sizeof(loose), 0);
	}

	sleep(1);
	//to prevent thread synchronization problem...
        
        printf("received from client : %s", buf);
	
    printf("Server is waiting for connect ...\n");
   current_connection--;
   } 

}
```
생성된 스레드를 위한 스레드함수인 serverthread()에서는 해당 client로 부터 가위, 바위, 보 메시지를 recv()함수를 통해 받습니다.
메시지를 수신받게 되면, 해당 client에 대한 정보를 저장을 하게 되는데 이 때 critical section이 일어나므로 mutex로 lock을 해주었습니다.

그리고 current_connection값을 통해 가위, 바위, 보 게임을 진행하는데 필요한 2명의 client가 모두 접속될때까지 무한 루프를 돌게 됩니다.

만약 2명의 client가 접속하게 되고, 모두 가위, 바위, 보 중에 하나의 메시지를 server로 전송했다면 proceed_round()함수를 호출하여 경기 결과를 처리하게 됩니다.

게임은 여러번 반복될 수 있기에 전체 로직은 무한 루프를 돌게 했습니다. 만약 게임을 중지하고 싶다면 client로 부터 Q 메시지를 받아 게임을 끝낼 수 있습니다.
<br><br>
<br>

## client.c

사용자로부터 가위(S), 바위(R), 보(P), 또는 종료(Q)를 입력받아 server로 메시지를 전달하고 그에 대응하는 결과를 받는 프로그램입니다.

socket programming에서 client는 socket() -> connect() -> recv(), or send() -> close()
<br><br>
<br>

```c
client_socket_addr.sin_family = AF_INET;

host = localhost;

if(argc > 2)port = atoi(argv[1]);
else port = default_port_num;

client_socket_addr.sin_port = htons((u_short)port);
```
client socket의 address에 관한 정보를 저장하는 부분입니다.
client_socket_addr의 멤버 sin_family로 IPv4 프로토콜을 사용하기 위해 AF_INET값을 가지고, 사용자로부터 port번호를 받아 오던가 또는 default_port_num를 
htons()함수를 통하여 netwrok byte order로 바꾸어 sin_port 멤버에 저장하게 됩니다.
<br><br>
<br>
```c
if(((int)(ptrh = gethostbyname(host))) == 0){
	printf("can't map by host name!\n");
	exit(1);
}
.
.
.
if(((int)(ptrp = getprotobyname("tcp"))) == 0){
		printf("can't map by tcp!\n");
	}

```
각각, host = "Localhost"의 프로토콜에 관한 정보와 TCP프로토콜에 관한 정보를 gethostbyname()을 통하여 가져오는 부분입니다.
<br><br>
<br>
```c
client_socketfd = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);

if(client_socketfd < 0){
	printf("socket can't create!\n");
	exit(1);
}
```
소켓에 관한 address정보와 port번호를 이용하여 소켓을 생성하는 부분입니다.
<br><br>
<br>
```c
if((connect(client_socketfd, (struct sockaddr*)&client_socket_addr, sizeof(client_socket_addr))<0)){
	printf("can't connect to server!\n");
	exit(1);
}
```
생성된 소켓을 이용하여 server에 연결을 요청하기 위해 connect()함수를 호출합니다. server에 관한 연결 정보는 client_socket_addr의 멤버인
sin_addr에 저장되어 있습니다.
<br><br>
<br>
```c
소켓 생성(socket()) 전
memcpy(&client_socket_addr.sin_addr, ptrh->h_addr, ptrh->h_length);
```

```c
while(1){
	memset(buf, '\0', sizeof(buf));
	printf("plz type R/S/P(Rock, Scissor, Paper) : ");
	fgets(buf, 4, stdin);

	send(client_socketfd, buf, sizeof(buf), 0);

	if(buf[0] == 'Q'){
		printf("the game will be quit..\n");
                close(client_socketfd);
                exit(0);
        }

	printf("waiting for result...\n");
	
	recv(client_socketfd, result, sizeof(result), 0);

	printf("you are %s\n", result);


}
```
사용자에게 서비스를 제공하는 부분으로, 사용자로부터 가위, 바위, 보를 키보드로부터 입력받아 server에 메시지를 send()함수를 통해 보내게 됩니다.
그리고 사용자로부터 Q를 제외한 R, S, 또는 P를 입력받게 되면 recv()함수에서는 server로부터 메시지를 받을 때 까지 block되게 됩니다.

가위, 바위, 보 게임은 2게임 이상 진행 될 수 있기에 전체 로직은 무한 루프입니다. 만약 게임을 종료하고 싶다면 사용자로부터 Q 입력을 받으면 됩니다.
<br><br>
<br>

***

### 실행 결과

<pre><font color="#8AE234"><b>jun@jun-VirtualBox</b></font>:<font color="#729FCF"><b>~/Desktop/study/socket</b></font>$ ./server
Server start, and running...
waiting for connection from client...</pre>

서버 프로그램(server)을 실행 시켰을 경우, client로 부터 연결이 accept될 때까지 기다리게 됩니다.
<br><br>
<br>


<pre><font color="#8AE234"><b>jun@jun-VirtualBox</b></font>:<font color="#729FCF"><b>~/Desktop/study/socket</b></font>$ ./client
plz type R/S/P(Rock, Scissor, Paper) : </pre>

클라이언트 프로그램(client)을 실행 시켰을 경우, 서버와 성공적으로 연결되면 사용자로부터 가위(S), 바위(R), 보(P) 중 한가지를 입력 받게 됩니다.
<br><br>
<br>

<pre><font color="#8AE234"><b>jun@jun-VirtualBox</b></font>:<font color="#729FCF"><b>~/Desktop/study/socket</b></font>$ ./server
Server start, and running...
waiting for connection from client...
client socket address(4) has create thread(9)
client connection state is connected</pre>

위의 클라이언트 프로그램이 정상적으로 서버에 연결되었음을 표시해줍니다.
 
가위바위보 게임을 하기 위해서는 2명의 client가 필요하므로 서버는 아직 남은 한개의 client 접속을 기다리고 있습니다.
<br><br>
<br>

<pre><font color="#8AE234"><b>jun@jun-VirtualBox</b></font>:<font color="#729FCF"><b>~/Desktop/study/socket</b></font>$ ./client
plz type R/S/P(Rock, Scissor, Paper) : R
waiting for result...</pre>

마찬가지로 client또한 다른 플레이어가 서버에 접속해야 게임이 진행이 되므로, 서버로부터 결과를 받을 때 까지 기다리게 됩니다.
<br><br>
<br>

<pre><font color="#8AE234"><b>jun@jun-VirtualBox</b></font>:<font color="#729FCF"><b>~/Desktop/study/socket</b></font>$ ./client
plz type R/S/P(Rock, Scissor, Paper) : </pre>

또 다른 client가 접속한 경우입니다.

<pre><font color="#8AE234"><b>jun@jun-VirtualBox</b></font>:<font color="#729FCF"><b>~/Desktop/study/socket</b></font>$ ./server
Server start, and running...
waiting for connection from client...
client socket address(4) has create thread(9)
client connection state is connected


client socket address(5) has create thread(9)
client connection state is connected</pre>

또다른 client로 부터 성공적으로 accept를 하게 되었습니다.
따라서 게임을 진행하기 위한 조건이 충족되었으므로 승/패/무승부 결과를 각 client에게 보내주게 됩니다.
<br><br>
<br>

<pre><font color="#8AE234"><b>jun@jun-VirtualBox</b></font>:<font color="#729FCF"><b>~/Desktop/study/socket</b></font>$ ./client
plz type R/S/P(Rock, Scissor, Paper) : R
waiting for result...
you are tie
plz type R/S/P(Rock, Scissor, Paper) : </pre>

client둘 다 R(바위)을 낸 경우이므로, 무승부 결과(tie) 메시지를 server로부터 각 client가 받게 됩니다.
<br><br>
<br>

<pre><font color="#8AE234"><b>jun@jun-VirtualBox</b></font>:<font color="#729FCF"><b>~/Desktop/study/socket</b></font>$ ./client
plz type R/S/P(Rock, Scissor, Paper) : R
waiting for result...
you are tie
plz type R/S/P(Rock, Scissor, Paper) : P
waiting for result...
you are loose</pre>

<pre><font color="#8AE234"><b>jun@jun-VirtualBox</b></font>:<font color="#729FCF"><b>~/Desktop/study/socket</b></font>$ ./client
plz type R/S/P(Rock, Scissor, Paper) : R
waiting for result...
you are tie
plz type R/S/P(Rock, Scissor, Paper) : S
waiting for result...
you are win</pre>

이번에는 서로 다른 input을 준 경우로, 가위(S)를 낸 client는 win 메시지를, 보(P)를 낸 client는 loose 메시지를 받게 됩니다.
