# Device driver programming

	* 라즈베리 파이를 이용하여 여러 센서들(device)을 추상화하여 user application process에서 system intercae를 통해 디바이스에 접근할 수 있도록 해주는 프로그램입니다.
	
	* Polling 방식과 interrupt 방식 둘 다 사용하여 구현하였습니다.
	
	
## User application code

	```
		mknod 생성 커맨드
	
	```

	Linux에서는 device를 하나의 file 단위로 관리를 하고있습니다. 따라서 user application process는 직접 디바이스를 제어하지 않고, 간접적으로
	device file을 이용하여 device를 제어하게 됩니다. device file은 device node라고도 불리며, mknod 명령어를 통해 생성합니다.
	
```
	int fd = -1;
	printf("[user level] LED control HomeWork ap is start!\n");
	fd = open("/dev/ledtest", O_RDWR);

	if(fd < 0){
		printf("[user level] device driver file can not open\n");
		printf("[user level] program will be terminated!\n");
		exit(1);
	
	}
```
	
	
	user application process에서 open()함수를 이용하여 해당 device file에 접근하게 되면, kernel에서는 open()함수를 서비스 해주기 위해
	linux kernel에서 등록한 device driver를 검색하게 됩니다. device driver검색은 linux kernel에서 각 device group별로 검색을 하게
	됩니다. 만약 해당 device driver를 찾지 못하게 되면, open()함수는 -1값을 return 받게 되고 그에 따라service를 요청한 user application 
	process에서 오류 메시지를 띄우고 프로그래믕ㄹ 종료하게 하였였습니다.
	
	```
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
	```
	
	반대로 올바르게 검색되었다면 이를 실행시켜 user application process와 link시켜 줍니다. 이후부터는 user application process가 해당
	device file에 file제어 함수들을 이용하여 제어 요청을 하게되면 device driver가 해당 요청을 받아 직접 디바이스를 제어하게 됩니다.
	
	이번 과제의 경우, 여러 제어가 요구 되므로 단순히 write, read보다는 ioctl()함수를 통해 센서를 제어하도록 하였습니다.
	
	```
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
	```
	이외로 getch()함수의 경우, linux에서 구현이 되어 있지 않으므로 인터넷에 제공된 소스를 사용하였습니다.[출처]https://anythink.tistory.com/entry/Linux-%EB%A6%AC%EB%88%85%EC%8A%A4%EC%97%90%EC%84%9C-getch-%EC%82%AC%EC%9A%A9

	
## Device driver module code	
	
### module
	
	```
	
	```
	
	리눅스 디바이스 드라이버는 kernel module로서 취급하여 linux kernel에 등록하여 사용하게 되는데, 등록방법은 kernel module로 컴파일 된 
	.ko파일을 insmod 명령어를 통해 kernel에 등록 시킬 수 있습니다. (이외에 아예 kernel source내에 포함 시켜 같이 컴파일 할 수도 있음)
	
### Device driver interface

	```
	static struct file_operations gpio_fop ={
		.owner = THIS_MODULE,
		.open=gpio_open, 
		.release=gpio_release,
		.read=gpio_read,
		.write=gpio_write,
		.unlocked_ioctl = led_control_unlocked_ioctl,	
	};
	```
	
	module을 등록할 대, file_operations라는 구조체도 kernel에 알려주게 됩니다. 이를 통해 device driver는 사용자가 file_operations를 사용해 등록한 함수들이 인터페이스
	역할을 하여 입/출력등을 제어하게 됩니다.
	
	각각 write, read, release, open등에 대응되는 함수 이름을 넘겨주었고, 그 중에 특별한 것이 .unlocked_ioctl입니다.
		
	```
	long led_control_unlocked_ioctl(struct file * file, unsigned int command, unsigned long argument){
	int i;
	printk("ioctl is occur!\n");
	switch(command){
		case 0:
			printk("red led will be blank 3 times. \n");
		       	for(i=0; i<3; i++){
				GPIO_SET(RED_LED);
				msleep(500);
				GPIO_CLR(RED_LED);
				msleep(500);

			}
			break;
		case 1:
			if(argument == 0){
				printk("red led is off\n");
				GPIO_CLR(RED_LED);
			}
			if(argument == 1){
				printk("red led is on\n");
				GPIO_SET(RED_LED);
			}
		       break;
		       
		default : printk("ioctl error!\n");
	}
	return 0;
}
	```
		
	.unlocked_ioctl이 받는 함수 led_control_unlocked_ioctl은 빨간색 LED를 on, off, 또는 3번 점멸 하는 등 여러 제어가 필요하게 되므로 write보다는 ioctl 함수를
	통해 제어하도록 하였습니다. led_control_unlocked_ioctl의 2번째 parameter인 command는 user application에서 ioctl()함수를 호출하면서 전달받게 됩니다.
	
	
	
	
### Device driver's major number and minor number
	
	```
		#define GPIO_MAJOR	245
		#define GPIO_MINOR	0
		
		devno=MKDEV(GPIO_MAJOR, GPIO_MINOR);
		
	```
	
	* Major number
		1. 커널에서 디바스 드라이버를 구분/ 연결하는데 사용
		
		2. 같은 디바이스 종류를 지칭. 1Byte의 length를 가짐.
		
		3. Linux에서 그룹 내 디바이스 종류들을 구분하기 위해 붙이는 번호
		
	* Minor number
		1. 하나의 디바이스 드라이버 내에 access하는 여러 device를 구분하기 위해 사용
		
		2. 각 device의 부가적인 정보를 나타냄. 2Bytes의 length를 가짐.
		
		3. 하나의 device driver가 여러 개의 device를 제어 가능.

### Polling

### Interrupt