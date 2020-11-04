#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

void* serverthread(void* cli_sock); 
void set_player_info(int, char);
int proceed_round(int);
void display_client_info(int, int, char*);

pthread_mutex_t mutex;

#define default_port_num 47777
#define max_client_num 2

int current_connection = 0;
int play_num = 0;
char win[6] = "win";
char loose[6] = "loose";
char tie[6] = "tie";

struct player_info{
	char attack;
}player[2];

main(int argc, char* argv[])
{
    struct protoent* ptrp;
    struct sockaddr_in server_socket_addr;
    struct sockaddr_in client_socket_addr;
    int	socketfd, client_socketfd;
    int port;
    int addr_len;
    int i = 0;
    int check;

    pthread_t tid;
	
    check = pthread_mutex_init(&mutex, NULL);

    if(check){
    	printf("something wrong happen in initializing mutex!\n");
	exit(1);
    }
	
    memset((char*)&server_socket_addr, 0, sizeof(server_socket_addr));

    server_socket_addr.sin_family = AF_INET;
    server_socket_addr.sin_addr.s_addr = INADDR_ANY;

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
    
    if (((int)(ptrp = getprotobyname("tcp"))) == 0) {
        printf("can't getprotobyname() function return\n");
        exit(1);
    }
    port = ptrp->p_proto;
    
    socketfd = socket(PF_INET, SOCK_STREAM, port);
    if (socketfd < 0) {
        printf("can't create socket!\n");
        exit(1);
    }
  
    if (bind(socketfd, (struct sockaddr*) & server_socket_addr, sizeof(server_socket_addr)) < 0) {
        printf("can't bind!\n");
        exit(1);
    }

    if (listen(socketfd, max_client_num) < 0) {
        printf("can't invocate listen!\n");
        exit(1);
    }

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
    
    close(socketfd);
    pthread_mutex_destroy(&mutex);
}

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
void set_player_info(int current_player_number, char attack){
	player[current_player_number].attack = attack;
}

int proceed_round(int me){
	int other;
	if(me == 0)other =1;
	else other = 0;
	
	if(player[me].attack == 'R' && player[other].attack == 'S')return 1;

	if(player[me].attack == 'S' && player[other].attack == 'P')return 1;

	if(player[me].attack == 'P' && player[other].attack == 'R')return 1;
	//win cases

	if(player[me].attack == player[other].attack)return 0;//tie case

	return -1;//loose case
	
}


void display_client_info(int client_socket_addr, int client_thread, char* state){
	sleep(1);
	if(!strcmp(state, "connected")){
	printf("client socket address(%d) has create thread(%d)\n", client_socket_addr);
	printf("client connection state is %s\n\n\n", state);
	}
	else{
		printf("client sockett address(%d) has %s from server\n\n", client_socket_addr, state);
	}
}


