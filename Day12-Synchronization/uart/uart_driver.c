#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/serial_reg.h>
#include <linux/io.h>


#define UART_BASE_ADDR          ((phys_addr_t)0x7e201000)
#define DEVICE_NAME             "simple_uart"
#define UART_MAP_SIZE           0x100      // Map 256 bytes of the UART registers
#define BUF_SIZE                1
static void __iomem *uart_base;             // Virtual memory-mapped UART base


static dev_t dev;                   // For getting device number, major, minor
static struct cdev uart_cdev;       // character device structure
static struct class *uart_class;    // dynamic file creation
static char char_buffer[BUF_SIZE];

static int uart_open(struct inode *inode, struct file *file) {
    return 0;
}

static int uart_release(struct inode *inode, struct file *file) {
    return 0;
}

static ssize_t uart_read(struct file *file, char __user *buf, size_t count, loff_t *ppos) {
    if(!(readb(uart_base + UART_LSR) & UART_LSR_DR)) // Wait for data
        return -EAGAIN;

    char_buffer[0] = readb(uart_base + UART_RX);
    if (copy_to_user(buf, char_buffer, BUF_SIZE))
        return -EFAULT;
    return BUF_SIZE;
}

static ssize_t uart_write(struct file *file, const char __user *buf, size_t count, loff_t *ppos) {
    if (copy_from_user(char_buffer, buf, BUF_SIZE))
        return -EFAULT;
    while (!(readb(uart_base + UART_LSR) & UART_LSR_THRE)) // Wait for space
        cpu_relax();
    
    writeb(char_buffer[0], uart_base + UART_TX);
    return BUF_SIZE;
}

static const struct file_operations uart_fops = {
    .owner = THIS_MODULE,
    .open = uart_open,
    .release = uart_release,
    .read = uart_read,
    .write = uart_write,
};

static int __init uart_init(void) {
    int ret;
    /*  Map UART physical address to the virtual address    */
    uart_base = ioremap(UART_BASE_ADDR, UART_MAP_SIZE);
    if (!uart_base) {
        printk(KERN_ERR "Failed to map UART base address\n");
        return -ENOMEM;
    }

    // Register a character device
    ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        printk(KERN_ERR "Failed to allocate char device region\n");
        iounmap(uart_base);
        return ret;
    }

    cdev_init(&uart_cdev, &uart_fops);
    //uart_cdev.owner = THIS_MODULE;

    ret = cdev_add(&uart_cdev, dev, 1);
    if (ret < 0) {
        printk(KERN_ERR "Failed to add cdev\n");
        unregister_chrdev_region(dev, 1);
        return ret;
    }

    uart_class = class_create(DEVICE_NAME);
    if (IS_ERR(uart_class)) {
        printk(KERN_ERR "Failed to create device class\n");
        cdev_del(&uart_cdev);
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(uart_class);
    }

    if((IS_ERR(device_create(uart_class, NULL, dev, NULL, DEVICE_NAME)))){
        printk(KERN_ERR "Failed to create device node\n");
        class_destroy(uart_class);
        cdev_del(&uart_cdev);
        unregister_chrdev_region(dev, 1);
        iounmap(uart_base);
        return -ENOMEM;
    }
    printk(KERN_INFO "UART driver initialized\n");
    return 0;
}

static void __exit uart_exit(void) {
    device_destroy(uart_class, dev);
    class_destroy(uart_class);
    cdev_del(&uart_cdev);
    unregister_chrdev_region(dev, 1);
    printk(KERN_INFO "UART driver removed\n");
}

module_init(uart_init);
module_exit(uart_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sanket");
MODULE_DESCRIPTION("UART Driver RPI-4");