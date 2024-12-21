#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/errno.h>

#define DEVICE_NAME "Kernel Logger"

#define MINOR_NUMBER_START      0
#define MINOR_NUMBER_COUNT      1

#define LOGS_SIZE   5
#define BUFFER_SIZE 512

/*  BUFFER to store the klogs   */
static char klog_buffer[LOGS_SIZE][BUFFER_SIZE];
/*  Read from head  */
static int head = 0;
/*  Write from tail */
static int tail = 0;

/*  Keep the track of number of time the device was opened  */
static int klog_dev_open;

/*  Create a character device   */
static struct cdev *pklog_cdev = NULL;

/*  Hold the major and minor number for the driver  */
static dev_t klog_mm_dev;

/*  Function Prototypes */
static ssize_t klog_read_from_logs(struct file *fops, char __user *uBuffer, size_t size, loff_t *offset);
static ssize_t klog_write_to_logs(struct file *fops, const char __user *uBuffer, size_t size, loff_t *offset);
static int driver_open(struct inode *file_inode, struct file *fops);
static int driver_close(struct inode *file_inode, struct file *fops);

/*  Expose the file custom file operations  */
static struct file_operations klog_fops = {
    .open = driver_open,
    .release = driver_close,
    .read = klog_read_from_logs,
    .write = klog_write_to_logs,
};

static ssize_t klog_read_from_logs(struct file *fops, char __user *uBuffer, size_t size, loff_t *offset){
    return 0;
}

static ssize_t klog_write_to_logs(struct file *fops, const char __user *uBuffer, size_t size, loff_t *offset){
    return 0;
}

static int driver_open(struct inode *file_inode, struct file *fops){
    klog_dev_open++;
    pr_info("%s: opened %d times!\n", DEVICE_NAME, klog_dev_open);
    return 0;
}

static int driver_close(struct inode *file_inode, struct file *fops){
    pr_info("%s: driver closed!\n", DEVICE_NAME);
    return 0;
}

static int __init klog_init(void){
    int xRet = -1;
    
    /*  Get the character device number dynamically */
    if((xRet = alloc_chrdev_region(&klog_mm_dev, MINOR_NUMBER_START, MINOR_NUMBER_COUNT, DEVICE_NAME))){
        pr_err("%s: Unable to allocate the major and minor number!\n", DEVICE_NAME);
        return xRet;
    }

    pr_info("%s: Major number = %d, Minor number = %d\n", DEVICE_NAME, MAJOR(klog_mm_dev), MINOR(klog_mm_dev));

    /*  Get a cdev structure    */
    if(!(pklog_cdev = cdev_alloc())){
        pr_err("%s: Unable to allocate the cdev structure!\n", DEVICE_NAME);
        goto err;
    }

    /*  Initialize the cdev structure and attach the fops with the character drive  */
    cdev_init(pklog_cdev, &klog_fops);

    /*  Add the device to the system and make it available to be used   */
    if((cdev_add(pklog_cdev, klog_mm_dev, MINOR_NUMBER_COUNT))){
        pr_err("%s: unable to add the device to the kernel!\n", DEVICE_NAME);
        goto err;
    }

    pr_info("%s: Loaded!\n", DEVICE_NAME);
    return 0;

    err:
        unregister_chrdev_region(klog_mm_dev, MINOR_NUMBER_COUNT); 
        return xRet;
}

static void __exit klog_exit(void){
    /*  delete the cdev */
    cdev_del(pklog_cdev);

    /*  Unregister the char device region occupied by the driver    */
    unregister_chrdev_region(klog_mm_dev, MINOR_NUMBER_COUNT);
    pr_info("%s: Device Unloaded!\n", DEVICE_NAME);
}

module_init(klog_init);
module_exit(klog_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sanket");
MODULE_DESCRIPTION("Kernel Logger Implementation");