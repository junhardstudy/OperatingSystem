
***

### 실행 결과

<pre><font color="#8AE234"><b>jun@jun-VirtualBox</b></font>:<font color="#729FCF"><b>~/Desktop/study/socket</b></font>$ ./server
Server start, and running...
waiting for connection from client...</pre>

서버 프로그램(server)을 실행 시켰을 경우, client로 부터 연결이 accept될 때까지 기다리게 됩니다.

<pre><font color="#8AE234"><b>jun@jun-VirtualBox</b></font>:<font color="#729FCF"><b>~/Desktop/study/socket</b></font>$ ./client
plz type R/S/P(Rock, Scissor, Paper) : </pre>

클라이언트 프로그램(client)을 실행 시켰을 경우, 서버와 성공적으로 연결되면 사용자로부터 가위(S), 바위(R), 보(P) 중 한가지를 입력 받게 됩니다.

<pre><font color="#8AE234"><b>jun@jun-VirtualBox</b></font>:<font color="#729FCF"><b>~/Desktop/study/socket</b></font>$ ./server
Server start, and running...
waiting for connection from client...
client socket address(4) has create thread(9)
client connection state is connected</pre>

위의 클라이언트 프로그램이 정상적으로 서버에 연결되었음을 표시해줍니다.
 
가위바위보 게임을 하기 위해서는 2명의 client가 필요하므로 서버는 아직 남은 한개의 client 접속을 기다리고 있습니다.

<pre><font color="#8AE234"><b>jun@jun-VirtualBox</b></font>:<font color="#729FCF"><b>~/Desktop/study/socket</b></font>$ ./client
plz type R/S/P(Rock, Scissor, Paper) : R
waiting for result...</pre>

마찬가지로 client또한 다른 플레이어가 서버에 접속해야 게임이 진행이 되므로, 서버로부터 결과를 받을 때 까지 기다리게 됩니다.

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

<pre><font color="#8AE234"><b>jun@jun-VirtualBox</b></font>:<font color="#729FCF"><b>~/Desktop/study/socket</b></font>$ ./client
plz type R/S/P(Rock, Scissor, Paper) : R
waiting for result...
you are tie
plz type R/S/P(Rock, Scissor, Paper) : </pre>

client둘 다 R(바위)을 낸 경우이므로, 무승부 결과(tie) 메시지를 server로부터 각 client가 받게 됩니다.

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
