#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#include <pthread.h>
#include <stdio.h>
#include <string.h>

void* serverthread(void* parm);       /* thread function prototype    */

pthread_mutex_t  mut;

#define PROTOPORT         5193          /* default protocol port number */
#define QLEN              6             /* size of request queue        */

int visits = 0;                        /* counts client connections     */

main(int argc, char* argv[])
{
    struct   hostent* ptrh;     /* pointer to a host table entry */
    struct   protoent* ptrp;     /* pointer to a protocol table entry */
    struct   sockaddr_in sad;     /* structure to hold server's address */
    struct   sockaddr_in cad;     /* structure to hold client's address */
    int      sd, sd2;             /* socket descriptors */
    int      port;                /* protocol port number */
    int      alen;                /* length of address */
    pthread_t  tid[2];             /* variable to hold thread ID */
	int i;

    pthread_mutex_init(&mut, NULL);
    memset((char*)&sad, 0, sizeof(sad)); /* clear sockaddr structure   */
    sad.sin_family = AF_INET;            /* set family to Internet     */
    sad.sin_addr.s_addr = INADDR_ANY;    /* set the local IP address */
    if (argc > 1) {                        /* if argument specified     */
        port = atoi(argv[1]); /* convert argument to binary*/
    }
    else {
        port = PROTOPORT;     /* use default port number   */
    }
    if (port > 0)                          /* test for illegal value    */
        sad.sin_port = htons((u_short)port);
    else {                                /* print error message and exit */
        fprintf(stderr, "bad port number %s/n", argv[1]);
        exit(1);
    }


    /* Map TCP transport protocol name to protocol number */

    if (((int)(ptrp = getprotobyname("tcp"))) == 0) {
        fprintf(stderr, "cannot map \"tcp\" to protocol number");
        exit(1);
    }

    /* Create a socket */
    sd = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
    if (sd < 0) {
        fprintf(stderr, "socket creation failed\n");
        exit(1);
    }

    /* Bind a local address to the socket */
    if (bind(sd, (struct sockaddr*) & sad, sizeof(sad)) < 0) {
        fprintf(stderr, "bind failed\n");
        exit(1);
    }

    /* Specify a size of request queue */
    if (listen(sd, QLEN) < 0) {
        fprintf(stderr, "listen failed\n");
        exit(1);
    }

    alen = sizeof(cad);
    /* Main server loop - accept and handle requests */
    fprintf(stderr, "Server up and running.\n");
    while (1) {

        printf("SERVER: Waiting for contact ...\n");

        if ((sd2 = accept(sd, (struct sockaddr*) & cad, &alen)) < 0) {
            fprintf(stderr, "accept failed\n");
            exit(1);
        }
        pthread_create(&tid[i], NULL, serverthread, (void*)sd2);
		
	}
    close(sd);
}

void* serverthread(void* parm)
{
    int tsd;
    char buf[1000];           /* buffer for string the server sends */
	int check_sum;
	int visit = 0;
    tsd = (int)parm;
	
	while(1){
		check_sum = recv(tsd, buf, sizeof(buf), 0);
		if(!strcmp(buf, "quit\n")||check_sum == 0){
			printf("client is disconnected\n");
			break;
		}
		printf("received from client : %s", buf);
//		send(tsd, buf, sizeof(buf), 0);
	}
	
	printf("Server is waiting for connect ...\n");
	close(tsd);
    pthread_exit(0);
	exit(1);
}
