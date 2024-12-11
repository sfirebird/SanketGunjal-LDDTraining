#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/of.h>

#define DEVICE_NAME "my_uart0"
#define CLASS_NAME "uart_class"

// UART0 base address for Raspberry Pi 4
#define UART0_BASE_ADDR 0xFE201000
#define UART0_REG_SIZE  0x48  // Size of UART registers

//static dev_t dev_num;
static struct cdev uart_cdev;
static struct class *uart_class;
static void __iomem *uart_base;
static struct file_operations fops;
static struct device* uart_device;
static int major_number;

// UART registers offsets
#define UART_DR   0x00  // Data Register
#define UART_FR   0x18  // Flag Register
#define UART_IBRD 0x24  // Integer Baud Rate Divisor
#define UART_FBRD 0x28  // Fractional Baud Rate Divisor
#define UART_LCRH 0x2C  // Line Control Register
#define UART_CR   0x30  // Control Register
#define UART_IMSC 0x38  // Interrupt Mask Set/Clear Register

// Useful Flag Register bits
#define UART_FR_RXFE 0x10  // Receive FIFO empty
#define UART_FR_TXFF 0x20  // Transmit FIFO full

static int dev_open(struct inode *inodep, struct file *filep);
static int dev_release(struct inode *inodep, struct file *filep);
static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset);
static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset);


static struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
};

static int __init uart_driver_init(void) {
    int xRet = 0; 

    // Map UART0 registers into kernel space
    uart_base = ioremap(UART0_BASE_ADDR, UART0_REG_SIZE);
    if (!uart_base) {
        printk(KERN_ALERT "UART Driver: Failed to map UART0 base address\n");
        return -ENOMEM;
    }

    // Allocate a major number dynamically
    if((xRet = alloc_chrdev_region(&uart_cdev, 0, 1, DEVICE_NAME))){
        printk(KERN_ALERT "UART Driver: Failed to register a major number\n");
        iounmap(uart_base);
    }
    
    /*
    // Allocate a major number dynamically
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "UART Driver: Failed to register a major number\n");
        iounmap(uart_base);
        return major_number;
    }
    */
    printk(KERN_INFO "UART Driver: Registered with major number %d\n", MAJOR(uart_cdev));

    // Create a device class
    uart_class = class_create(CLASS_NAME);
    if (IS_ERR(uart_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        iounmap(uart_base);
        return PTR_ERR(uart_class);
    }

    // Create the device
    uart_device = device_create(uart_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(uart_device)) {
        class_destroy(uart_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        iounmap(uart_base);
        return PTR_ERR(uart_device);
    }

    printk(KERN_INFO "UART Driver: Device created successfully\n");
    return 0;
}

// Driver cleanup
static void __exit uart_driver_exit(void) {
    device_destroy(uart_class, MKDEV(major_number, 0));
    class_destroy(uart_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    iounmap(uart_base);
    printk(KERN_INFO "UART Driver: Exiting and cleaning up\n");
}

// File operations
static int dev_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "UART Driver: Device opened\n");
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "UART Driver: Device closed\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset) {
    char ch;
    // Wait until the Receive FIFO is not empty
    if (readl(uart_base + UART_FR) & UART_FR_RXFE) {
        cpu_relax();  // Avoid busy waiting
    }

    // Read the character from UART Data Register
    ch = readl(uart_base + UART_DR) & 0xFF;

    // Copy the character to user space
    if (copy_to_user(buffer, &ch, 1)) {
        return -EFAULT;
    }
    return 1;  // 1 byte read
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    char ch;

    // Copy a character from user space to kernel space
    if (copy_from_user(&ch, buffer, 1)) {
        return -EFAULT;
    }

    // Wait until the Transmit FIFO is not full
    if (readl(uart_base + UART_FR) & UART_FR_TXFF) {
        cpu_relax();  // Avoid busy waiting
    }

    // Write the character to the UART Data Register
    writel(ch, uart_base + UART_DR);
    return 1;  // 1 byte written
}

module_init(uart_driver_init);
module_exit(uart_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sanket");
MODULE_DESCRIPTION("UART Driver for Raspberry Pi 4");
MODULE_VERSION("1.0");