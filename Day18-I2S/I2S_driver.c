/* 
 * i2s_audio_driver.c - A simple I2S audio virtual driver
 * 
 * This driver simulates an I2S audio interface. It accepts audio data from user-space
 * through a write call and prints logs simulating audio playback.
 * 
 * The driver avoids the use of class_create and requires manual device node creation.
 * 
 * Author: YourName
 * License: GPL
 */

#include <linux/module.h>       // Required for all kernel modules
#include <linux/fs.h>           // For file_operations and device registration
#include <linux/uaccess.h>      // For copy_from_user to safely access user-space data
#include <linux/cdev.h>         // For character device registration
#include <linux/init.h>         // For module initialization and cleanup macros

/* --- DEFINITIONS --- */
#define DEVICE_NAME "i2s_audio" // Name of the device node: /dev/i2s_audio
#define BUFFER_SIZE 4096        // Size of the internal buffer for audio data

/* --- GLOBAL VARIABLES --- */
static char audio_buffer[BUFFER_SIZE];  // Buffer to hold incoming audio data
static int major_number;                // Major number for the device
static struct cdev i2s_cdev;            // Character device structure

/* 
 * i2s_open - Called when the device file is opened.
 * @inode: Pointer to the inode structure.
 * @file: Pointer to the file structure.
 * 
 * Returns: 0 on success.
 */
static int i2s_open(struct inode *inode, struct file *file)
{
    pr_info("I2S device opened\n");
    return 0; // Always succeed
}

/* 
 * i2s_release - Called when the device file is closed.
 * @inode: Pointer to the inode structure.
 * @file: Pointer to the file structure.
 * 
 * Returns: 0 on success.
 */
static int i2s_release(struct inode *inode, struct file *file)
{
    pr_info("I2S device closed\n");
    return 0; // Always succeed
}

/* 
 * i2s_write - Called when data is written to the device.
 * @file: Pointer to the file structure.
 * @user_buffer: Buffer containing data from user space.
 * @count: Number of bytes to write.
 * @pos: File position offset (not used here).
 * 
 * Returns: Number of bytes successfully written or error code.
 */
static ssize_t i2s_write(struct file *file, const char __user *user_buffer, size_t count, loff_t *pos)
{
    size_t bytes_to_copy;

    /* Determine the number of bytes to copy (minimum of count and BUFFER_SIZE) */
    bytes_to_copy = min(count, BUFFER_SIZE);

    /* Safely copy data from user space to kernel space */
    if (copy_from_user(audio_buffer, user_buffer, bytes_to_copy)) {
        pr_err("Failed to copy data from user space\n");
        return -EFAULT; // Return an error if copying fails
    }

    pr_info("Received %zu bytes of audio data\n", bytes_to_copy);
    /* Simulated audio playback: In real drivers, this is where hardware interaction happens */
    pr_info("Playing simulated audio...\n");

    return bytes_to_copy; // Return the number of bytes successfully "written"
}

/* 
 * File operations structure - Maps system calls (e.g., open, write) to driver functions.
 */
static const struct file_operations i2s_fops = {
    .owner = THIS_MODULE,  // Pointer to the module that "owns" this file
    .open = i2s_open,      // Function to call on open()
    .release = i2s_release,// Function to call on close()
    .write = i2s_write,    // Function to call on write()
};

/* 
 * i2s_init - Module initialization function.
 * 
 * This function registers the character device, allocates a major number dynamically,
 * and informs the user to manually create the device node.
 * 
 * Returns: 0 on success, error code otherwise.
 */
static int __init i2s_init(void)
{
    dev_t dev;
    int ret;

    /* Allocate a major number dynamically */
    ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        pr_err("Failed to allocate major number\n");
        return ret; // Return error if allocation fails
    }
    major_number = MAJOR(dev); // Extract the major number

    /* Initialize the character device structure */
    cdev_init(&i2s_cdev, &i2s_fops);
    i2s_cdev.owner = THIS_MODULE;

    /* Add the character device to the system */
    ret = cdev_add(&i2s_cdev, dev, 1);
    if (ret < 0) {
        pr_err("Failed to add cdev\n");
        unregister_chrdev_region(dev, 1);
        return ret;
    }

    /* Inform the user to manually create the device node */
    pr_info("I2S driver initialized successfully\n");
    pr_info("To use the device, create the device node manually:\n");
    pr_info("  sudo mknod /dev/%s c %d 0\n", DEVICE_NAME, major_number);

    return 0; // Success
}

/* 
 * i2s_exit - Module cleanup function.
 * 
 * This function unregisters the character device and frees allocated resources.
 */
static void __exit i2s_exit(void)
{
    dev_t dev = MKDEV(major_number, 0);

    /* Remove the character device */
    cdev_del(&i2s_cdev);

    /* Unregister the major number */
    unregister_chrdev_region(dev, 1);

    pr_info("I2S driver unloaded successfully\n");
}

/* --- MODULE INFORMATION MACROS --- */
module_init(i2s_init);   // Marks i2s_init as the initialization function
module_exit(i2s_exit);   // Marks i2s_exit as the cleanup function

MODULE_LICENSE("GPL");               // License type: GPL
MODULE_AUTHOR("Sanket");           // Author of the module
MODULE_DESCRIPTION("I2S Audio Virtual Driver"); // Short description
MODULE_VERSION("1.0");               // Module version