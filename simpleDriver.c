#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
////#include <asm-generic/io.h>
#include <linux/interrupt.h>
#include <linux/gpio.h>

////#include <sys/io.h>
#define  DEVICE_NAME "ebbchar"
#define  CLASS_NAME  "ebb"
#define  BASE_ADDR   0x01C20800

#define PA_CFG0_REG *((volatile uint32_t*)(ioremap(BASE_ADDR + 0x00, 4)))
#define PA_CFG1_REG *((volatile uint32_t*)(ioremap(BASE_ADDR + 0x04, 4)))
#define PA_PULL0_REG *((volatile uint32_t*)(ioremap(BASE_ADDR + 0x1C, 4)))
#define PA_DAT_REG *((volatile uint32_t*)(ioremap(BASE_ADDR + 0x10, 4)))
#define PA_EINT_CFG0_REG *((volatile uint32_t*)(ioremap(BASE_ADDR + 0x200, 4)))
#define PA_EINT_CTL_REG *((volatile uint32_t*)(ioremap(BASE_ADDR + 0x210, 4)))
#define PA_EINT_STATUS_REG *((volatile uint32_t*)(ioremap(BASE_ADDR + 0x214, 4)))



MODULE_LICENSE("GPL");


static int majorNumber;
static char message[256] = {0};
static short sizeOfMessage;
static int numberOpens = 0;
static struct class* ebbcharClass = NULL;
static struct device* ebbcharDevice = NULL;


static int dev_open(struct inode *, struct file *);
static int dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char*, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char*, size_t, loff_t *);
static struct file_operations ops = {
	.open = dev_open,
	.read = dev_read,
	.write = dev_write, 
	.release = dev_release,
};


static irqreturn_t gpio_intr(int irq, void *ident)
{
	printk(KERN_INFO "Interrupt is triggered\n");
	return IRQ_HANDLED;
}




static int __init ebbchar_init(void)
{
    int err;
	printk(KERN_INFO "EBBChar: Initializing the EBBChar LKM\n");
    // Try to dynamically allocated a major number for the device -- more difficult but worth it
	majorNumber = register_chrdev(0, DEVICE_NAME, &ops);
	if(majorNumber < 0) 
	{
		printk(KERN_ALERT "EBBChar failed to register a major number\n");
		return majorNumber;
	}
	printk(KERN_INFO "EBBChar: registered correctly with major number\n");
	
	//Register Device class
	ebbcharClass = class_create(THIS_MODULE, CLASS_NAME);
	if(IS_ERR(ebbcharClass)) //check error and clean up if there is an error
	{
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to register device class\n");
		return PTR_ERR(ebbcharClass);
	}

	// Register device driver
	ebbcharDevice = device_create(ebbcharClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
	if(IS_ERR(ebbcharDevice))
	{
		class_destroy(ebbcharClass);
		unregister_chrdev(majorNumber, DEVICE_NAME);
		printk(KERN_ALERT "Failed to create device\n");
		return PTR_ERR(ebbcharDevice);
	}
	
	printk(KERN_INFO "EBBChar: device class created correctly\n");
	printk(KERN_INFO "EBBChar: sleep 1s\n");
	
	
	printk(KERN_INFO "ivalue of PA_DAT_REG : %X\n", (PA_DAT_REG));
	printk(KERN_INFO "ivalue of PA_CFG1_REG : %X\n", (PA_CFG1_REG));
	printk(KERN_INFO "ivalue of PA_PULL0_REG : %X\n", (PA_PULL0_REG));
	
	PA_CFG1_REG = PA_CFG1_REG & 0xFFFFF111 ; //config output for PA10, PA9, PA8
	PA_PULL0_REG = PA_PULL0_REG & 0xFFC0FFFF; //remove current value before write new value
	PA_PULL0_REG = PA_PULL0_REG | (0xA9<<14); //pull down PA10, PA9, PA8
	msleep(100);
	PA_DAT_REG = PA_DAT_REG | 0x00000100;
	ssleep(1);
	PA_DAT_REG = PA_DAT_REG | 0x00000200;
	ssleep(1);
	PA_DAT_REG = PA_DAT_REG | 0x00000400;
	ssleep(1);
	printk(KERN_INFO "value of PA_CFG0_REG : %X\n", (PA_CFG0_REG));
	printk(KERN_INFO "value of PA_DAT_REG : %X\n", (PA_DAT_REG));
	printk(KERN_INFO "value of PA_CFG1_REG : %X\n", (PA_CFG1_REG));
	printk(KERN_INFO "value of PA_PULL0_REG : %X\n", (PA_PULL0_REG));
	printk(KERN_INFO "init driver done\n");

	printk(KERN_INFO "value of PA_CFG0_REG: %x\n", PA_CFG0_REG);
	//if((PA_CFG0_REG >> 30 & 0x07) == 0x07)
	//{
	//	printk(KERN_INFO " PA7 is available\n");
	//}
	
	printk(KERN_INFO "dump gpio_to_irq :%d\n", gpio_to_irq(7));

	//PA7 (0*32 + 7 = 7)
	////err = request_irq(gpio_to_irq(7), gpio_intr, IRQF_SHARED| IRQF_TRIGGER_FALLING, THIS_MODULE->name, THIS_MODULE->name);
	err = request_irq(gpio_to_irq(7), gpio_intr, IRQF_SHARED, THIS_MODULE->name, THIS_MODULE->name);
	printk(KERN_INFO "dvalue of ext interrupt config 0: %X\n", PA_EINT_CFG0_REG);
	printk(KERN_INFO "dvalue of ext interrupt control 0: %X\n", PA_EINT_CTL_REG);
	printk(KERN_INFO "dvalue of ext interrupt status 0: %X\n", PA_EINT_STATUS_REG);
	printk(KERN_INFO "value of PA_DAT_REG : %X\n", (PA_DAT_REG));
	printk(KERN_INFO "value of PA_CFG1_REG : %X\n", (PA_CFG1_REG));
	printk(KERN_INFO "value of PA_PULL0_REG : %X\n", (PA_PULL0_REG));
	PA_EINT_CFG0_REG = PA_EINT_CFG0_REG | (1 << 28);
	PA_EINT_CTL_REG = PA_EINT_CTL_REG | (1 << 7);
	return 0;
}

static void __exit ebbchar_exit(void)
{
	device_destroy(ebbcharClass, MKDEV(majorNumber, 0));
	class_unregister(ebbcharClass);
	class_destroy(ebbcharClass);
	unregister_chrdev(majorNumber, DEVICE_NAME);
	printk(KERN_INFO "EBBChar: goodbye from LKM\n");
	free_irq(gpio_to_irq(7), THIS_MODULE->name);
	printk(KERN_INFO "value of PA_CFG0_REG: %x\n", PA_CFG0_REG);
	PA_DAT_REG = PA_DAT_REG & 0x00000000;
}


static int dev_open(struct inode * inodep, struct file *filep)
{
	numberOpens++;
	printk(KERN_INFO "EBBChar: Device has been opened %d time(s)\n", numberOpens);
	return 0;
}


static ssize_t dev_read(struct file* filep, char* buffer, size_t len, loff_t *offset)
{
	int error_count = 0;
	// copy to user
	error_count = copy_to_user(buffer, message, sizeOfMessage);

	if(error_count == 0)
	{
		printk(KERN_INFO "EBBCahr: Sent %d characters to the user\n", sizeOfMessage);
		return (sizeOfMessage);
	}
	else
	{
		printk(KERN_INFO "EBBChar: Failed to send %d characters to the user\n", error_count);
		return -EFAULT;
	}
}


static ssize_t dev_write(struct file* filep, const char* buffer, size_t len, loff_t *offset)
{
	int rsult = copy_from_user(message, buffer, len);
	if(rsult < 0)
	{
		printk(KERN_ERR "copy user to kernel failed\n");
		return 1;
	}
	printk(KERN_INFO "dump string user kernel: %s\n", message);
	sizeOfMessage = strlen(message);
	printk(KERN_INFO "EBBChar: Received %zu characters from the user\n", len);
	
	if(strcmp(message, "1\n") == 0)
	{
		PA_DAT_REG = PA_DAT_REG | (1<<10);
		////printk(KERN_INFO "value of PA_DAT_REG 1: %X\n", PA_DAT_REG);
	}
	else if(strcmp(message, "0\n") == 0)
	{
		PA_DAT_REG = PA_DAT_REG &(~(1<<10));
		////printk(KERN_INFO "value of PA_DAT_REG 0: %X\n", PA_DAT_REG);
	}
	else
	{
		printk(KERN_ALERT "Invalid value\n");
	}	

	return len;
}

static int dev_release(struct inode* inodep, struct file* filep)
{
	printk(KERN_INFO "EBBChar: Device successfully closed\n");
	return 0;
}


module_init(ebbchar_init);
module_exit(ebbchar_exit);

