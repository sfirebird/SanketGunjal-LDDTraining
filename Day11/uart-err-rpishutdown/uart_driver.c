#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>

#include <linux/gpio.h>
#include <linux/serial.h>
#include <linux/errno.h>
#include <linux/cdev.h>

#define UART_DRIVER         "rpi4_uart"
#define UART_PORT           "/dev/rpi4_uart_device"

// File operation function prototypes
static int uart_open(struct inode *inode, struct file *file);
static int uart_release(struct inode *inode, struct file *file);
static ssize_t uart_read(struct file *file, char __user *buf, size_t len, loff_t *offset);
static ssize_t uart_write(struct file *file, const char __user *buf, size_t len, loff_t *offset);

/*  Related to the character devices    */
dev_t dev = 0;
static struct cdev uart_dev;

/*  Make the UART_DRIVER accessible to the userspace    */
static struct class *uart_class;

// Define UART file operations
static struct file_operations uart_fops = {
    /* Why do we need to specify the owner? what does the kernel do with this information */
    .owner = THIS_MODULE,
    .open = uart_open,
    .release = uart_release,
    .read = uart_read,
    .write = uart_write,
};

// Global file pointer for UART device
static struct file *uart_file = NULL;

// UART Open function - Opens the UART device
static int uart_open(struct inode *inode, struct file *file)
{
    // Open the UART device
    uart_file = filp_open(UART_PORT, O_RDWR | O_NOCTTY | O_NONBLOCK, 0);
    if (IS_ERR(uart_file)) {
        pr_err("Failed to open UART device %s\n", UART_PORT);
        return PTR_ERR(uart_file);  // Return the error code
    }

    pr_info("UART device opened successfully\n");
    return 0;
}

// UART Release function - Closes the UART device
static int uart_release(struct inode *inode, struct file *file)
{
    if (uart_file && !IS_ERR(uart_file)) {
        filp_close(uart_file, NULL);
        pr_info("UART device closed\n");
    }
    return 0;
}

// UART Read function - Reads data from UART device
static ssize_t uart_read(struct file *file, char __user *buf, size_t len, loff_t *offset)
{
    ssize_t ret;

    if (!uart_file || IS_ERR(uart_file)) {
        pr_err("UART device is not opened\n");
        return -ENODEV;  // No such device
    }

    /*  Understand how the kernel_read function works   */
    ret = kernel_read(uart_file, buf, len, offset);
    if (ret < 0) {
        pr_err("Failed to read from UART device\n");
        return ret;  // Return read error
    }

    pr_info("Read %zd bytes from UART device\n", ret);
    return ret;
}

// UART Write function - Writes data to UART device
static ssize_t uart_write(struct file *file, const char __user *buf, size_t len, loff_t *offset)
{
    ssize_t ret;

    if (!uart_file || IS_ERR(uart_file)) {
        pr_err("UART device is not opened\n");
        return -ENODEV;  // No such device
    }

    /*  Understand how the kernel_write function works   */
    ret = kernel_write(uart_file, buf, len, offset);
    if (ret < 0) {
        pr_err("Failed to write to UART device\n");
        return ret;  // Return write error
    }

    pr_info("Written %zd bytes to UART device\n", ret);
    return ret;
}

// Module Init function - Registers the character device
static int __init uart_init(void)
{
    int ret;

    /*  Allocate major number   */
    if((ret = alloc_chrdev_region(&dev, 0, 1, UART_DRIVER)) < 0){
        pr_err("%s: Error with function alloc_chrdev_region, return code %d\n", UART_DRIVER, ret);
        return ret;
    }
    pr_info("%s: Major Number = %d, Minor Number = %d\n", UART_DRIVER, MAJOR(dev), MINOR(dev));
    
    /*  Initializa the cdev structure   */
    cdev_init(&uart_dev, &uart_fops);

    /*  Add the character device to the kernel  */
    if((ret = cdev_add(&uart_dev, dev, 1)) < 0){
        pr_err("%s: Error with function cdev_add, return code %d\n", UART_DRIVER, ret);
        unregister_chrdev_region(dev, 1);
        return ret;
    }

    /*  Create the entry in /sys   */
    if(IS_ERR(uart_class = class_create(UART_DRIVER))){
        pr_err("%s: Error with function class_create, return code %d\n", UART_DRIVER, ret);
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(uart_class);
    }

    /*  Create the device belonging to the uart_class   */
    if(IS_ERR(device_create(uart_class, NULL, dev, NULL, "rpi4_uart_device"))){
        pr_err("%s: Error with function device_create, return code %ld\n", UART_DRIVER, PTR_ERR(uart_class));
        class_destroy(uart_class);
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(uart_class);
    }

    pr_info("%s: Succesfully loaded, Phew:~!\n", UART_DRIVER);
    return 0;
}

// Module Exit function - Unregisters the character device
static void __exit uart_exit(void)
{
    unregister_chrdev_region(dev, 1);
    device_destroy(uart_class, dev);
    class_destroy(uart_class);
    unregister_chrdev_region(dev, 1);
    pr_info("%s: Driver unloaded!\n", UART_DRIVER);
}

module_init(uart_init);
module_exit(uart_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sanket Gunjal");
MODULE_DESCRIPTION("Raspberry Pi UART Driver!");