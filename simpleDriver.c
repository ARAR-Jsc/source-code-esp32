#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/delay.h>
////#include <asm-generic/io.h>
////#include <sys/io.h>
#include <linux/gpio.h>                //gpio_to_irq()
#include <linux/interrupt.h>
#include <linux/workqueue.h>           //
#define  DEVICE_NAME "ebbchar"
#define  CLASS_NAME  "ebb"
#define  BASE_ADDR   0x01C20800

#define PA_DAT_REG *((volatile uint32_t*)(ioremap(BASE_ADDR + 0x10, 4)))
#define PA_CFG0_REG *((volatile uint32_t*)(ioremap(BASE_ADDR + 0x00, 4)))
#define PA_CFG1_REG *((volatile uint32_t*)(ioremap(BASE_ADDR + 0x04, 4)))
#define PA_PULL0_REG *((volatile uint32_t*)(ioremap(BASE_ADDR + 0x1C, 4)))
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



/* argument DECLARE_WORK:
   name (indecate workqueue or tasklet is use from DECLARE_WORK
   function which run in workqueue
   data for function
*/

void workQueueFunc(struct work_struct *work)
{
	char status = !((PA_DAT_REG >> 10) & 1); //revert LSB bit
	printk(KERN_INFO "this is workqueue\n");
	PA_DAT_REG &= (~(1 << 10)); //remove 10th bit
	////check = ((!((PA_DAT_REG >> 10) & 0x00000001)) << 10); //X = !X
	PA_DAT_REG |= (status << 10); //X = !X
}


DECLARE_WORK (workqueue, workQueueFunc);



////void short_do_tasklet (unsigned long unused)
////{
////	//PA_DAT_REG = PA_DAT_REG;
////	//gpio_set_value(10, 0);
////}


/*name, function do bottom half interrupt, data (argument) */
////DECLARE_TASKLET (short_tasklet, short_do_tasklet, 0);

static irqreturn_t ext_intr(int irq, void* dev_id)
{
	printk(KERN_INFO "Interrupt is triggered\n");
	
	//tasklet
	//tasklet_schedule(&short_tasklet);
	
	//workqueue
	schedule_work(&workqueue);
	

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
	//printk(KERN_INFO "ivalue of PA_CFG1_REG_CHECK : %X\n", ioread32(PA_CFG1_REG_CHECK));
	//printk(KERN_INFO "ivalue of PA_PULL0_REG_CHECK : %X\n", ioread32(PA_PULL0_REG_CHECK));
	printk(KERN_INFO "ivalue of PA_PULL0_REG : %X\n", (PA_PULL0_REG));
	
	PA_CFG1_REG = PA_CFG1_REG & 0xFFFFF111 ; //config output for PA10, PA9, PA8
	PA_PULL0_REG = PA_PULL0_REG & 0xFFC0FFFF; //remove current value before write new value
	PA_PULL0_REG = PA_PULL0_REG | (0xA9<<14); //pull down PA10, PA9, PA8
	PA_DAT_REG = PA_DAT_REG | 0x00000100;
	ssleep(1);
	PA_DAT_REG = PA_DAT_REG | 0x00000200;
	ssleep(1);
	PA_DAT_REG = PA_DAT_REG | 0x00000400;
	printk(KERN_INFO "value of PA_DAT_REG : %X\n", (PA_DAT_REG));
	printk(KERN_INFO "value of PA_CFG1_REG : %X\n", (PA_CFG1_REG));
	//printk(KERN_INFO "value of PA_CFG1_REG_CHECK : %X\n", ioread32(PA_CFG1_REG_CHECK));
	//printk(KERN_INFO "value of PA_PULL0_REG_CHECK : %X\n", ioread32(PA_PULL0_REG_CHECK));
	printk(KERN_INFO "value of PA_PULL0_REG : %X\n", (PA_PULL0_REG));
	printk(KERN_INFO "init driver done\n");
	PA_EINT_CFG0_REG = PA_EINT_CFG0_REG | (1<<28); //set bit 28th falling edge
	//PA_EINT_CFG0_REG = PA_EINT_CFG0_REG & (~(1<<28)); //clear bit 28th
	PA_EINT_CTL_REG = PA_EINT_CTL_REG | (1<<7);
	
	//PA7 --> 0*32 + 7 = 7
	err = request_irq (gpio_to_irq(7), ext_intr, IRQF_SHARED | IRQF_TRIGGER_FALLING,THIS_MODULE->name, THIS_MODULE->name);
	if(err < 0)
	{
		printk(KERN_INFO "request interrupt is failed\n");
		return err;
	}
	return 0;
}

static void __exit ebbchar_exit(void)
{
	device_destroy(ebbcharClass, MKDEV(majorNumber, 0));
	class_unregister(ebbcharClass);
	class_destroy(ebbcharClass);
	unregister_chrdev(majorNumber, DEVICE_NAME);
	printk(KERN_INFO "EBBChar: goodbye from LKM\n");
	PA_DAT_REG = PA_DAT_REG & 0x00000000;
	printk(KERN_INFO "Release interrupt\n");
	free_irq(gpio_to_irq(7), THIS_MODULE->name);
	//cancel_scheduled_work();//

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
	////sprintf(message, "%s(%zu letters)", buffer, len); //appending received string with it's len
	int rsult;
	rsult = copy_from_user(message, buffer, len);
	if(rsult < 0)
	{
		printk(KERN_ERR "copy from user to kernel failed\n");
		return 1; //not return 0 --> cause infinite loop
	}
	sizeOfMessage = strlen(message);
	printk(KERN_INFO "EBBChar: Received %zu characters from the user %s\n", len, message);
	if(strcmp(message, "1\n") == 0)
	{
		printk(KERN_ERR "turn on led\n");
		PA_DAT_REG |= (1 << 10);
	}
	else if(strcmp(message, "0\n") == 0)
	{
		printk(KERN_ERR "turn off led\n");
		PA_DAT_REG &= (~(1 <<  10));
	}
	else
	{
		printk(KERN_ERR "Invalid value\n");
	}
	return len; //should be return  value > 0
}

static int dev_release(struct inode* inodep, struct file* filep)
{
	printk(KERN_INFO "EBBChar: Device successfully closed\n");
	return 0;
}


module_init(ebbchar_init);
module_exit(ebbchar_exit);

