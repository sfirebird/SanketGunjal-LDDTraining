#include <linux/module.h>
#include <linux/kernel.h>

/* Module initialization */
static int __init basic_driver_init(void){
	printk(KERN_INFO "Basic Driver Loaded!\n");
	return 0;
}

static void __exit basic_driver_exit(void){
	printk(KERN_INFO "Basic Driver Unloaded!\n");
}

module_init(basic_driver_init);
module_exit(basic_driver_exit);

MODULE_DESCRIPTION("A basic linux device driver");
MODULE_AUTHOR("Firebird");
MODULE_LICENSE("GPL");