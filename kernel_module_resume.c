#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");

static int init(void) {
	printk(KERN_DEBUG "Hello world\n");
	return 0;
}

static void cleanup(void) {
	printk(KERN_DEBUG "Goodbye world\n");
}

module_init(init);
module_exit(cleanup);
