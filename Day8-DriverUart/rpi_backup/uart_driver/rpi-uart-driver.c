#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/serial_reg.h>
#include <linux/serial_core.h>
#include <linux/tty.h>
#include <linux/tty_flip.h>

#define DEVICE_NAME "uart_driver"

/*  Do we have the flexibility to use any name for the port?    */
#define UART_PORT "/dev/ttyAMA0"

/*  Why are we creating the file structure globally?    */
static struct file *uart_file;

/*  What is mm_segment_t structure responsible for? */
static mm_segment_t old_fs;

/* Open UART device */
static int uart_open(void)
{
    /* How are we opening the file?
    * the filp open is a function that will take the path to the file, specify the flags and modes.
    * it will return the file pointer for the file that it opened, in our case it will be /dev/ttyAMA0
    */
    uart_file = filp_open(UART_PORT, O_RDWR | O_NOCTTY | O_NONBLOCK, 0);
    
    /*  IS_ERR() is a C function that will detect the error in the pointer being passed 
        https://docs.kernel.org/core-api/kernel-api.html#c.IS_ERR
    */
    if (IS_ERR(uart_file)) {
        pr_err("Failed to open UART device %s\n", UART_PORT);
        return PTR_ERR(uart_file);
    }

    /*  I don't understand what is going on here?   */
    old_fs = get_fs();
    set_fs(KERNEL_DS);
    pr_info("UART device opened successfully\n");
    return 0;
}

/* Close UART device */
static void uart_close(void)
{
    /*  close the file only if uart_file contains null? */
    if (uart_file) {
        /*  check if the prototype of filp_close    */
        filp_close(uart_file, NULL);
        set_fs(old_fs);
        pr_info("UART device closed\n");
    }
}

/* Write data to UART */
static ssize_t uart_write(const char *buf, size_t len)
{
    /*  what exactly is happening here? */
    if (!uart_file || IS_ERR(uart_file)) {
        pr_err("UART device is not opened\n");
        return -ENODEV;
    }

    /* Check what is kernel write api   */
    return kernel_write(uart_file, buf, len, &uart_file->f_pos);
}

/* Read data from UART */
static ssize_t uart_read(char *buf, size_t len)
{
    if (!uart_file || IS_ERR(uart_file)) {
        pr_err("UART device is not opened\n");
        return -ENODEV;
    }

    /*  Check how the kernel_read is being invoked  */
    return kernel_read(uart_file, buf, len, &uart_file->f_pos);
}

/* Module Init */
static int __init uart_driver_init(void)
{
    int ret;

    pr_info("Initializing UART driver\n");

    /*  What is uart_open   */
    ret = uart_open();
    if (ret)
        return ret;

    /* Example: Send a test string */
    uart_write("Hello UART\n", 11);
    return 0;
}

/* Module Exit */
static void __exit uart_driver_exit(void)
{
    /*  how the uart is getting closed? */
    uart_close();
    pr_info("Exiting UART driver\n");
}

module_init(uart_driver_init);
module_exit(uart_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("techdhaba");
MODULE_DESCRIPTION("Basic UART driver for Raspberry Pi");