#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>
#include <linux/irq.h>

#define GPIO_BASE	0xfe200000
#define GPIO_SIZE	256
#define GPIO_SET(g)	(*(gpio+7)=1<<g)
#define GPIO_CLR(g)	(*(gpio+10)=1<<g)
#define GPIO_GET(g)	(*(gpio+13)&(1<<g))
#define GPIO_IN(g)	(*(gpio+((g)/10)) &=~(7<<(((g)%10)*3)))
#define GPIO_OUT(g)	(*(gpio+((g)/10)) |=(1<<(((g)%10)*3)))

#define GPIO_MAJOR	245
#define GPIO_MINOR	0
#define GPIO_DEVICE 	"ledtest"
#define RED_LED		26
#define GREEN_LED	6
#define TEC_SWITCH	21
#define TEC_SWITCH_STRING "21"

static int gpio_open(struct inode*, struct file*);
static int gpio_release(struct inode*, struct file*);
static ssize_t gpio_read(struct file*, char*, size_t, loff_t*);
static ssize_t gpio_write(struct file*, const char*, size_t, loff_t*);
long led_control_unlocked_ioctl(struct file*, unsigned int, unsigned long);

volatile unsigned *gpio;
static char msg[BLOCK_SIZE] = {0};
struct cdev gpio_cdev;
static struct file_operations gpio_fop ={
	.owner = THIS_MODULE,
	.open=gpio_open, 
	.release=gpio_release,
	.read=gpio_read,
	.write=gpio_write,
        .unlocked_ioctl = led_control_unlocked_ioctl,	
};
char gpio_irq_num = 0;
int state = 0;

irqreturn_t irq_gpio(int irq, void * device_id){
	/*********************************************************************
	 *This function is called when tact switch pushing interrupt
	 *is occur. From global variable state which is show current
	 *led state, this function set the state value into  1(on), or 0(off).
	**********************************************************************/
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

int start_module(void){
	unsigned int cnt=1;
	static void* map;
	int add;
	dev_t devno;
	
	printk(KERN_INFO "START MODULE\n");
	devno=MKDEV(GPIO_MAJOR, GPIO_MINOR);
	register_chrdev_region(devno, 1, GPIO_DEVICE);
	cdev_init(&gpio_cdev, &gpio_fop);
	gpio_cdev.owner = THIS_MODULE;
	
	add=cdev_add(&gpio_cdev, devno, cnt);
	map=ioremap(GPIO_BASE, GPIO_SIZE);
	gpio=(volatile unsigned int*)map;
	
	//GPIO_PIN mode set in and out.
	GPIO_IN(GREEN_LED);
	GPIO_OUT(GREEN_LED);
	GPIO_IN(RED_LED);
	GPIO_OUT(RED_LED);

	//find the interrupt address of GPIO pin 21.
	gpio_irq_num = gpio_to_irq(TEC_SWITCH);

	//request interrupt handler to kernel.
	if(request_irq(gpio_irq_num, irq_gpio, IRQF_TRIGGER_RISING, "gpio_21", NULL)<0){
		printk("request irq error!\n");
	}
	//irq_set_irq_type(gpio_irq_num, IRQ_TYPE_EDGE_RISING);
	//->another trigger setting function.	
	printk("device driver installation is success!\n");
	
	return 0;
}


void end_module(void){
	dev_t devno = MKDEV(GPIO_MAJOR,GPIO_MINOR);
	unregister_chrdev_region(devno, 1);	
	cdev_del(&gpio_cdev);
	free_irq(gpio_irq_num, NULL);//interrupt module is removed from kernel.
	if(gpio)	
		iounmap(gpio);
	printk(KERN_INFO "END MODULE\n");
}
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

long led_control_unlocked_ioctl(struct file * file, unsigned int command, unsigned long argument){
	/*************************************************************
	 *This function is called when main.c call ioctl() function.
	 *By parameter command and argument, the function preforming
	 *red led blank 3times, or on/off.
	**************************************************************/
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

static ssize_t gpio_write(struct file* file, const char* buf, size_t len, loff_t* off){
	/*********************************************************
	 *This function is performing RED LED blank 3times, or on/
	 *off. But the function does not call, because it is a
	 *function implemented for study purposes.
	 *********************************************************/
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

MODULE_LICENSE("GPL");
module_init(start_module);
module_exit(end_module);



