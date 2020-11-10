#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>

#define default_port_num 47777

char localhost[] = "localhost";

void main(int argc, char* argv[]){
	struct hostent* ptrh;
	struct protoent* ptrp;
	struct sockaddr_in client_socket_addr;
	int client_socketfd;
	int port;
	char *host;

	char buf[2];
	char result[6];

	memset((char*)&client_socketfd, 0, sizeof(client_socketfd));
	client_socket_addr.sin_family = AF_INET;

	host = localhost;

	if(argc > 2)port = atoi(argv[1]);
	else port = default_port_num;

	client_socket_addr.sin_port = htons((u_short)port);


	if(((int)(ptrh = gethostbyname(host))) == 0){
		printf("can't map by host name!\n");
		exit(1);
	}

	memcpy(&client_socket_addr.sin_addr, ptrh->h_addr, ptrh->h_length);
	//char pointer
	//so we have to use memcpy instead of =
	//client_socket_addr.sin_addr = ptrh->h_addr;(X)
	
	if(((int)(ptrp = getprotobyname("tcp"))) == 0){
		printf("can't map by tcp!\n");
	}
	
	client_socketfd = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);

	if(client_socketfd < 0){
		printf("socket can't create!\n");
		exit(1);
	}
	
	if((connect(client_socketfd, (struct sockaddr*)&client_socket_addr, sizeof(client_socket_addr))<0)){
		printf("can't connect to server!\n");
		exit(1);
	}
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

}

