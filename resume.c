#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/mutex.h>

#include "resume.h"

MODULE_AUTHOR("Pavel Gromov <pagrom23@gmail.com>");
MODULE_DESCRIPTION("My resume in linux kernel module format");
MODULE_LICENSE("GPL");

int resume_init_module(void);
void resume_cleanup_module(void);
static int resume_device_open(struct inode *, struct file *);
static int resume_device_release(struct inode *, struct file *);
static ssize_t resume_device_read(struct file *, char *, size_t, loff_t *);
static ssize_t resume_device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define RESUME_DEVICE_NAME "resume"  /* Dev name as it appears in /proc/devices   */
#define BUF_LEN 80            /* Max length of the message from the device */

/* Global variables are declared as static, so are global within the file. */

static unsigned long resume_buffer_size = RESUME_BUFFER_SIZE;
static unsigned long resume_block_size = RESUME_BLOCK_SIZE;

static unsigned int resume_major = 0; /* Major number assigned to our device driver */
static int Device_Open = 0;
static struct resume_dev *resume_device = NULL;
static struct class *resume_class = NULL;
static char msg[BUF_LEN];    /* The msg the device will give when asked    */
static char *msg_Ptr;

static struct file_operations resume_fops = {
	.read = resume_device_read,
	.write = resume_device_write,
	.open = resume_device_open,
	.release = resume_device_release
};


/* Functions */

static int
resume_construct_device(struct resume_dev *dev, int minor, 
	struct class *class)
{
	int err = 0;
	dev_t devno = MKDEV(resume_major, minor);
	struct device *device = NULL;
	
	BUG_ON(dev == NULL || class == NULL);

	/* Memory is to be allocated when the device is opened the first time */
	dev->data = NULL;     
    dev->buffer_size = resume_buffer_size;
	dev->block_size = resume_block_size;
	mutex_init(&dev->resume_mutex);
    
	cdev_init(&dev->cdev, &resume_fops);
	dev->cdev.owner = THIS_MODULE;
    
	err = cdev_add(&dev->cdev, devno, 1);
	if (err)
	{
		printk(KERN_WARNING "[target] Error %d while trying to add %s%d",
			err, RESUME_DEVICE_NAME, minor);
        return err;
	}

    device = device_create(class, NULL, /* no parent device */ 
        devno, NULL, /* no additional data */
        RESUME_DEVICE_NAME "%d", minor);

    if (IS_ERR(device)) {
        err = PTR_ERR(device);
        printk(KERN_WARNING "[target] Error %d while trying to create %s%d",
			err, RESUME_DEVICE_NAME, minor);
        cdev_del(&dev->cdev);
        return err;
    }

	return 0;
}

int resume_init_module(void) {

	// Major = register_chrdev(0, DEVICE_NAME, &fops);

	// if (Major < 0) {
	// 	printk ("Registering the character device failed with %d\n", Major);
	// 	return Major;
	// }

	// printk(KERN_DEBUG "Hello world\n");

	// printk("<1>I was assigned major number %d.  To talk to\n", Major);
	// printk("<1>the driver, create a dev file with\n");
	// printk("'mknod /dev/hello c %d 0'.\n", Major);
	// printk("<1>Try various minor numbers.  Try to cat and echo to\n");
	// printk("the device file.\n");
	// printk("<1>Remove the device file and module when done.\n");

	int err = 0;
	dev_t dev = 0;

	/* Get a range of minor numbers (starting with 0) to work with */
	err = alloc_chrdev_region(&dev, 0, 1, RESUME_DEVICE_NAME);
	if (err < 0) {
		printk(KERN_WARNING "[target] alloc_chrdev_region() failed\n");
		return err;
	}
	resume_major = MAJOR(dev);

	/* Create device class */
	resume_class = class_create(THIS_MODULE, RESUME_DEVICE_NAME);
	if (IS_ERR(resume_class)) {
		err = PTR_ERR(resume_class);
		goto fail;
	}

	resume_device = (struct resume_dev *)kzalloc(
		sizeof(struct resume_dev), GFP_KERNEL);
	if (resume_device == NULL) {
		err = -ENOMEM;
		goto fail;
	}

	err = resume_construct_device(resume_device, 0, resume_class);
	if (err)
		goto fail;
	
	return 0;

fail:
	// cfake_cleanup_module(devices_to_destroy);
	return err;
}

void resume_cleanup_module(void) {
	unregister_chrdev(resume_major, RESUME_DEVICE_NAME);
	printk(KERN_DEBUG "Goodbye world\n");
}

/* Methods */

static int resume_device_open(struct inode *inode, struct file *file)
{
	static int counter = 0;
	if (Device_Open)
		return -EBUSY;

	Device_Open++;
	sprintf(msg,"I already told you %d times Hello world!\n", counter++);
	msg_Ptr = msg;

	return SUCCESS;
}


/* Called when a process closes the device file */
static int resume_device_release(struct inode *inode, struct file *file)
{
	Device_Open --;     /* We're now ready for our next caller */

	return 0;
}


/* Called when a process, which already opened the dev file, attempts to
   read from it.
*/
static ssize_t resume_device_read(struct file *filp,
   char *buffer,    /* The buffer to fill with data */
   size_t length,   /* The length of the buffer     */
   loff_t *offset)  /* Our offset in the file       */
{
   /* Number of bytes actually written to the buffer */
   int bytes_read = 0;

   // /* If we're at the end of the message, return 0 signifying end of file */
   // if (*msg_Ptr == 0) return 0;

   // /* Actually put the data into the buffer */
   // while (length && *msg_Ptr)  {

   //       The buffer is in the user data segment, not the kernel segment;
   //       * assignment won't work.  We have to use put_user which copies data from
   //       * the kernel data segment to the user data segment. 
   //       put_user(*(msg_Ptr++), buffer++);

   //       length--;
   //       bytes_read++;
   // }

   // /* Most read functions return the number of bytes put into the buffer */
   return bytes_read;
}


/*  Called when a process writes to dev file: echo "hi" > /dev/hello */
static ssize_t resume_device_write(struct file *filp,
   const char *buff,
   size_t len,
   loff_t *off)
{
   printk ("<1>Sorry, this operation isn't supported.\n");
   return -EINVAL;
}

module_init(resume_init_module);
module_exit(resume_cleanup_module);
