# Device driver programming

* 라즈베리 파이를 이용하여 여러 센서들(device)을 추상화하여 user application process에서 system intercae를 통해 디바이스에 접근할 수 있도록 해주는 프로그램입니다.
	
* Interrupt 방식 구현
	
	
## User application code

```
mknod /dev/led_module 245 0	
```
Linux에서는 device를 하나의 file 단위로 관리를 하고있습니다. 따라서 user application process는 직접 디바이스를 제어하지 않고, 간접적으로
device file을 이용하여 device를 제어하게 됩니다. device file은 device node라고도 불리며, mknod 명령어를 통해 생성합니다.
<br>
<br>
<br>
<br>		
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
<br>
<br>
<br>
<br>
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
<br>
<br>
<br>
<br>	
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
이외로 getch()함수의 경우, linux에서 구현이 되어 있지 않으므로 인터넷에 제공된 소스를 사용하였습니다.[출처] https://anythink.tistory.com/entry/Linux-%EB%A6%AC%EB%88%85%EC%8A%A4%EC%97%90%EC%84%9C-getch-%EC%82%AC%EC%9A%A9

	
## Device driver module code	
	
### module
	
```
insmod led_module.ko
```	
리눅스 디바이스 드라이버는 kernel module로서 취급하여 linux kernel에 등록하여 사용하게 되는데, 등록방법은 kernel module로 컴파일 된 
.ko파일을 insmod 명령어를 통해 kernel에 등록 시킬 수 있습니다. (이외에 아예 kernel source내에 포함 시켜 같이 컴파일 할 수도 있음)
<br>
<br>
<br>
<br>
### Device driver's major number and minor number
	
```
#define GPIO_MAJOR	245
#define GPIO_MINOR	0
		
devno=MKDEV(GPIO_MAJOR, GPIO_MINOR);		
```	
Major number인 GPIO_MAJOR의 경우, kernel에서 device driver를 구분 및 연결하는데 사용 됩니다. 총 1byte의 길이를 가질 수 있습니다.

Minor number인 GPIO_MINOR의 경우, 하나의 디바이스 드라이버 내에 access하는 여러 device들이 있을 수 있고 그런 경우 device를 구분하기 위해 사용 됩니다.
총 2bytes의 길이를 가질 수 있습니다. 

해당 과제의 경우, major number를 245, minor number를 0번으로 하는 dev_t 구조체를 메크로 함수 MKDEV()를 통하여 얻을 수 있습니다.
<br>
<br>
<br>
<br>
```
register_chrdev_region(devno, 1, GPIO_DEVICE);
register_chrdev_region(devno, 1, GPIO_DEVICE);
cdev_init(&gpio_cdev, &gpio_fop);
gpio_cdev.owner = THIS_MODULE;
	
add=cdev_add(&gpio_cdev, devno, cnt);
``` 
Linux device driver의 경우, 크게 character driver, block driver, network driver 3가지의 group으로 나뉘게 됩니다. 저희는 device를 file처럼
접근하여 직접 제어를 수행하는 형태를 가지므로 register_chrdev_region()함수를 통하여 1개의 char device를 GPIO_DEVICE로 등록해주게 됩니다.
우리가 정의 한 file_operations 구조체를 등록 해 주기위해, cdev_init(), cdev_add()함수를 순차적으로 호출하여 cdev 구조체로 초기화 -> 커널에 등록하는 과정을 수행합니다.
<br>
<br>
<br>
<br>
```
map=ioremap(GPIO_BASE, GPIO_SIZE);
gpio=(volatile unsigned int*)map;

GPIO_IN(GREEN_LED);
GPIO_OUT(GREEN_LED);
GPIO_IN(RED_LED);
GPIO_OUT(RED_LED);
```
라즈베리파이의 gpio핀들에 대한 물리 메모리 주소를 가상 메모리 주소로 맵핑하기 위해 ioremap()를 호출하고, 매크로 함수 GPIO_IN()과 GPIO_OUT()을 통하여 빨간색 LED와 초록색 LED의
in/out 모드를 수행할 address를 설정하였습니다.
<br>
<br>
<br>
<br>
```
gpio_irq_num = gpio_to_irq(TEC_SWITCH);

if(request_irq(gpio_irq_num, irq_gpio, IRQF_TRIGGER_RISING, "gpio_21", NULL)<0){
		printk("request irq error!\n");
	}
```
마지막으로 interrupt 제어를 구현하는 부분입니다.
텍트 스위치가 연결된 pin 번호 21인 TEC_SWITCH에 대한 인터럽트 address를 얻기 위해 gpio_to_irq()함수를 호출 해줍니다. 이렇게 얻은 address와 인터럽트 발생시 수행되어야 할 커널 함수인
irq_gpio()함수, 상승 엣지인 IRQF_TRIGGER_RISING를 가지는 인터럽트 핸들러를 생성하기 위해 request_irq()함수가 사용됩니다.
<br>
<br>
<br>
<br>
### Interrupt
```
irqreturn_t irq_gpio(int irq, void * device_id){
	printk("interrupt is occur!");
	if(state == 0){
		GPIO_SET(GREEN_LED);
		printk("Green LED is turn on!\n");
		state = 1;
	}else{
		GPIO_CLR(GREEN_LED);
		printk("Green LED is turn off!\n");
		state = 0;
	}
	return IRQ_HANDLED;
}
```
모듈 적재시, 인터럽트 핸들러에서 등록할 함수입니다. 텍트 스위치를 누르게 되면, 디바이스 드라이버가 ioctl()함수를 수행하고 있는 중이더라도 인터럽트 방식이기에 이전에 진행되던 동작이 중단되지 않고
초록색 LED를 On/Off 할 수 있게 됩니다.
<br>
<br>
<br>
<br>
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
<br>
<br>
<br>
<br>	
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
<br>
<br>
<br>
<br>
이외에 write, read, release, open함수는 아래와 같이 구현되어 있는데, write 함수의 경우는 구현만 되어 있고 실제 user application에서는 사용되지 않습니다. 위에서 언급했다 싶이
write()함수보다는 ioctl()함수를 통해 여러 제어를 하는게 더 적절해 보였습니다. 만약 사용하고자 한다면 user application process에서 device file에 write할 때, a, 1, 또는 0을
전달해주면 ioctl()함수처럼 똑같은 동작을 수행하게 됩니다.  
```
static int gpio_open(struct inode *inode, struct file *file){
	try_module_get(THIS_MODULE);
	printk("OPEN - gpio device\n");
	return 0;
}

static int gpio_release(struct inode* inode, struct file* file){
	module_put(THIS_MODULE);
	printk("CLOSE - gpio device\n");
	return 0;
}

static ssize_t gpio_read(struct file* file, char* buf, size_t len, loff_t* off){
	int cnt;
	cnt = copy_to_user(buf, msg, strlen(msg)+1);
	printk("GPIO device READ: %s \n", msg);
	return cnt;
}

static ssize_t gpio_write(struct file* file, const char* buf, size_t len, loff_t* off){
	short cnt;
	int i;

	memset(msg, 0, BLOCK_SIZE);
	
	cnt = copy_from_user(msg, buf, len);

	if(msg[0] == 'a'){
		printk("red led will be blank 3\n");
		for(i = 0; i < 3; i++){
		GPIO_SET(RED_LED);
		msleep(500);
		GPIO_CLR(RED_LED);
		msleep(500);
		}
	}
	if(msg[0] == '1'){
		printk("red led is turn on\n");
		GPIO_SET(RED_LED);
	}
	if(msg[0] == '0'){
		printk("red led is turn off\n");
		GPIO_CLR(RED_LED);
	}
	
	printk("gpio device WRITE: %c \n", msg[0]);
	return cnt;
}
```
<br>
<br>
<br>
<br>

## 동작 모습

<iframe width="893" height="502" src="https://www.youtube.com/embed/INjtP3w68kc" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>