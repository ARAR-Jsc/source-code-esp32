#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#define  DEVICE_NAME "ebbchar"
#define  CLASS_NAME  "ebb"
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

static int __init ebbchar_init(void)
{
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
	return 0;
}

static void __exit ebbchar_exit(void)
{
    device_destroy(ebbcharClass, MKDEV(majorNumber, 0));
	class_unregister(ebbcharClass);
	class_destroy(ebbcharClass);
	unregister_chrdev(majorNumber, DEVICE_NAME);
	printk(KERN_INFO "EBBChar: goodbye from LKM\n");
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
	sprintf(message, "%s(%zu letters)", buffer, len); //appending received string with it's len
	sizeOfMessage = strlen(message);
	printk(KERN_INFO "EBBChar: Received %zu characters from the user\n");
	return 0;
}

static int dev_release(struct inode* inodep, struct file* filep)
{
	printk(KERN_INFO "EBBChar: Device successfully closed\n");
	return 0;
}


module_init(ebbchar_init);
module_exit(ebbchar_exit);

