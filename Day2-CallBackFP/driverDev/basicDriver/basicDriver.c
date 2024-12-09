#include<linux/module.h>
#include<linux/kerenl.h>

/*	Module Initialization	*/
static int __init basic_driver_init(void){
	printk(KERN_INFO "Basic Driver Loaded!\n");
	return 0;
}

/*	Module cleanup	*/
static void __exit basic_driver_exit(void){
	printk(KERN_INFO "Basic Driver Unloaded!\n");
}

/*	Module initialization and cleanup	*/
module_init(hello_init);
module_exit(hello_exit);


MODULE_DESCRIPTION("A basic linux device driver");
MODULE_AUTHOR("Firebird");
MODULE_LICENSE("GPL");

