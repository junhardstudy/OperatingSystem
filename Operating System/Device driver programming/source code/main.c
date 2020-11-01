#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <termio.h>

int getch(void);

void main(int argc, char** argv){
	char buf[BUFSIZ];
	char key;
	char i = 0;
	int fd = -1;
	printf("[user level] LED control HomeWork ap is start!\n");
	fd = open("/dev/ledtest", O_RDWR);

	if(fd < 0){
		printf("[user level] device driver file can not open\n");
		printf("[user level] program will be terminated!\n");
		exit(1);
	
	}
	
	printf("[user level] device driver file open is success!\n");
	

	while(1){
		printf("Prees the button 'A' or 'B' ('Q' button is exit): ");
		key = getch();
		if(key == 'q'){
			printf("application is exit\n");
			close(fd);
			exit(1);
		}
		if(key != 'a' && key != 'b'){
			printf("%c is not supported key!\n", key);
			continue;
		}
		
		printf("you pressed the %c button\n", key);
		if(key == 'a'){
			ioctl(fd, 0, 0);
		}
		if(key == 'b'){
			printf("press the '1(RED LED ON)' or '0(RED LED OFF)' button : ");
			key = getch();
			if(key == '0'){
				ioctl(fd, 1, 0);
				printf("RED LED is off\n");
			}
			else if(key == '1'){
				ioctl(fd, 1, 1);
				printf("RED LED is on\n");
			}
			else{
				printf("'%c' button does not support!\n", key);
				continue;
			}
		}
		printf("\n");
	}

}

int getch(void){
    int ch;
    struct termios buf, save;
    tcgetattr(0,&save);
    buf = save;
    buf.c_lflag &= ~(ICANON|ECHO);
    buf.c_cc[VMIN] = 1;
    buf.c_cc[VTIME] = 0;
    tcsetattr(0, TCSAFLUSH, &buf);
    ch = getchar();
    tcsetattr(0, TCSAFLUSH, &save);
    return ch;
}
