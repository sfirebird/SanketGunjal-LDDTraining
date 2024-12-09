#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define DEVICE_NAME "Sanket_uart_driver"
#define CLASS_NAME "Sanket_uart_class"
#define UART_BASE_ADDR 0xfe201000  // Example base address of UART (this will vary depending on the platform)
#define UART_REG_OFFSET 0x100     // Example offset for UART registers

// Define UART hardware register addresses
#define UART_DATA       0x00  // Data register offset
#define UART_STATUS     0x18  // Status register offset
#define UART_CONTROL    0x30  // Control register offset

// Define status flags
#define UART_STATUS_RX_READY   (1 << 0)  // Receiver ready
#define UART_STATUS_TX_READY   (1 << 1)  // Transmitter ready

// Device variables
static int major_number;
static struct class *uart_class = NULL;
static struct device *uart_device = NULL;
static void __iomem *uart_base = NULL;  // MMIO base address for UART

// File operations
static int dev_open(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "UART Driver: Device opened.\n");
    return 0;
}

static int dev_release(struct inode *inodep, struct file *filep) {
    printk(KERN_INFO "UART Driver: Device closed.\n");
    return 0;
}

static ssize_t dev_read(struct file *filep, char __user *buffer, size_t len, loff_t *offset) {
    char uart_char;
    unsigned int status;

    // Wait for data to be ready in the UART RX register
    status = readl(uart_base + UART_STATUS);
    if (!(status & UART_STATUS_RX_READY)) {
        return -EAGAIN;  // No data ready
    }

    // Read data from the UART data register
    uart_char = readl(uart_base + UART_DATA) & 0xFF;

    // Copy data to user space
    if (copy_to_user(buffer, &uart_char, 1)) {
        return -EFAULT;
    }

    printk(KERN_INFO "UART Driver: Read character '%c'.\n", uart_char);
    return 1;  // 1 byte read
}

static ssize_t dev_write(struct file *filep, const char __user *buffer, size_t len, loff_t *offset) {
    char uart_char;
    unsigned int status;

    if (len == 0) {
        return 0;  // No data to write
    }

    // Copy data from user space to kernel space
    if (copy_from_user(&uart_char, buffer, 1)) {
        return -EFAULT;
    }

    pr_info("%s: copy_from_user = %c\n", DEVICE_NAME, uart_char);

    // Wait for the UART TX register to be ready
    status = readl(uart_base + UART_STATUS);
    pr_info("%s: status = %d\n", DEVICE_NAME, status);
    while (!(status & UART_STATUS_TX_READY)) {
        pr_info("%s: uart_tx not ready!", DEVICE_NAME);
        return -EAGAIN;  // UART transmitter not ready
    }

    // Write the character to the UART data register
    writel(uart_char, uart_base + UART_DATA);
    pr_info("UART Driver: Sent character '%c'.\n", uart_char);
    return 1;  // 1 byte written
}

// File operations struct
static struct file_operations fops = {
    .open = dev_open,
    .release = dev_release,
    .read = dev_read,
    .write = dev_write,
};

// Initialize the driver
static int __init uart_driver_init(void) {
    printk(KERN_INFO "UART Driver: Initializing UART driver.\n");

    // Request MMIO region for UART base address
    uart_base = ioremap(UART_BASE_ADDR, UART_REG_OFFSET);
    if (!uart_base) {
        printk(KERN_ALERT "UART Driver: Failed to remap UART base address.\n");
        return -ENOMEM;
    }

    // Allocate a major number
    major_number = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_number < 0) {
        printk(KERN_ALERT "UART Driver: Failed to register a major number.\n");
        return major_number;
    }
    printk(KERN_INFO "UART Driver: Registered with major number %d.\n", major_number);

    // Register the device class
    uart_class = class_create(CLASS_NAME);
    if (IS_ERR(uart_class)) {
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "UART Driver: Failed to register device class.\n");
        return PTR_ERR(uart_class);
    }

    // Register the device driver
    uart_device = device_create(uart_class, NULL, MKDEV(major_number, 0), NULL, DEVICE_NAME);
    if (IS_ERR(uart_device)) {
        class_destroy(uart_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        printk(KERN_ALERT "UART Driver: Failed to create the device.\n");
        return PTR_ERR(uart_device);
    }

    printk(KERN_INFO "UART Driver: Device created successfully.\n");
    return 0;
}

// Cleanup the driver
static void __exit uart_driver_exit(void) {
    iounmap(uart_base);  // Unmap the UART MMIO region
    device_destroy(uart_class, MKDEV(major_number, 0));
    class_destroy(uart_class);
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "UART Driver: Exiting and cleaning up.\n");
}

module_init(uart_driver_init);
module_exit(uart_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prashant");
MODULE_DESCRIPTION("A simple UART device driver.");
MODULE_VERSION("1.0");