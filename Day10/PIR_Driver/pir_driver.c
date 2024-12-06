#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#define DRIVER_NAME             "PIR_Driver"

#define CLASS_NAME              "rpi4"

/*  Define the GPIO Pin interfacing with PIR sensor.
 *  Read from this pin.
    GPIO17
 */
#define PIR_SENSOR_IO_PIN       529

/**
 * Define the GPIO Pin connected to the LED
   GPIO18
 */
#define LED_IO_PIN              530

static int driver_major_number;
static struct class *rpi_class;
static struct device *rpi_device;

/**
 * @brief
 * @param
 * @return
 */
static int pir_open(struct inode *inode, struct file *file){
    pr_info("%s: opened!\n", DRIVER_NAME);
    return 0;
}

/**
 * @brief
 * @param
 * @return

static int gpio_open(struct inode *inode, struct file *file){

}
*/

/**
 * @brief
 * @param
 * @return
 */
static int pir_close(struct inode* inode, struct file* fops){
    pr_info("%s: closed!\n", DRIVER_NAME);
    return 0;
}

/**
 * @brief
 * @param
 * @return

static int gpio_close(struct inode *inode, struct file *inode){

}
*/

static void gpio_write(int control_status){
    /*  Depending upon the pir control the led  */
    if(control_status){
        gpio_set_value(LED_IO_PIN, control_status);
        pr_info("%s: Gotcha! Turning on the Alarm!\n", DRIVER_NAME);
    }
    else{
        gpio_set_value(LED_IO_PIN, control_status);
        pr_info("%s: GPIO Low!\n", DRIVER_NAME);
    }

}

static ssize_t pir_read(struct file *file, char __user *buf, size_t len, loff_t *offset){
    int get_pir_value;
    
    /*  Read the PIR value  */
    get_pir_value = gpio_get_value(PIR_SENSOR_IO_PIN);
    if(get_pir_value){
        gpio_write(get_pir_value);
    }
    
    /*  Convert the integer to string   */
    char buffer[2];
    snprintf(buffer, sizeof(buffer), "%d", get_pir_value);

    /*  send to the user    */
    ssize_t bytes_sent;
    if((bytes_sent = copy_to_user(buf, buffer, sizeof(buffer)))){
        pr_err("Failed to send data to user\n");
        return -EFAULT;
    }

    pr_info("%s: successfully read %d!\n", DRIVER_NAME, get_pir_value);
    return bytes_sent;
}

static ssize_t gpio_write_from_user(struct file *file, const char __user *buf, size_t len, loff_t *offset){
    char buffer[2];

    if (copy_from_user(buffer, buf, len)) {
        pr_err("Failed to receive data from user\n");
        return -EFAULT;
    }

    buffer[len] = '\0';
    int value = simple_strtol(buffer, NULL, 10);

    if (value == 0 || value == 1) {
        gpio_write(value);  // Set GPIO pin value
        pr_info("GPIO Device write: %d\n", value);
    } else {
        pr_err("Invalid value: GPIO accepts 0 or 1\n");
        return -EINVAL;
    }

    return len;
}

/**
 * Map your driver functions
 */
static struct file_operations gpio_fops = {
    .open = pir_open,
    .release = pir_close, 
    .read = pir_read, 
    .write = gpio_write_from_user,
};

/**
 * Initialization and cleanup functions
 */
static int __init pir_driver_init(void){
    int xRetVal;
    
    /*  Request the gpio pin for PIR sensor  */
    xRetVal = gpio_request(PIR_SENSOR_IO_PIN, "PIR GPIO PIN");
    pr_info("xRetVal = %d\n", xRetVal);
    if(xRetVal){
        pr_err("Failed to request the gpio pin: %d, Error returns = %d\n", PIR_SENSOR_IO_PIN, xRetVal);
        return xRetVal;
    }

    /*  Request the gpio pin for LED  */
    xRetVal = gpio_request(LED_IO_PIN, "LED GPIO PIN");
    pr_info("xRetVal = %d\n", xRetVal);
    if(xRetVal){
        pr_err("Failed to request the gpio pin: %d, Error returns = %d\n", LED_IO_PIN, xRetVal);
        return xRetVal;
    }

    /* Set the gpio direction to input for PIR */
    xRetVal = gpio_direction_input(PIR_SENSOR_IO_PIN);
    if(xRetVal){
        pr_err("Failed to set the gpio pin: %d in input direction, Error returns = %d\n", PIR_SENSOR_IO_PIN, xRetVal);
        goto resolve;
    }

    /*  Set the gpio direction to output mode for LED   */
    xRetVal = gpio_direction_output(LED_IO_PIN, 0);
    if(xRetVal){
        pr_err("Failed to set the gpio pin: %d in output direction, Error returns = %d\n", LED_IO_PIN, xRetVal);
        goto resolve;
    }

    /*  Register the device  */
    driver_major_number = register_chrdev(0, DRIVER_NAME, (const struct file_operations *)&gpio_fops);
    if(driver_major_number < 0){
        pr_err("Failed to register the \"%s\" device, Error Returns = %d\n", DRIVER_NAME, xRetVal);
        goto resolve;
    }
    
    /*  Create the class    */
    rpi_class = class_create(CLASS_NAME);
    if(IS_ERR(rpi_class)){
        // write an error message for debugging.
        pr_err("%s: Failed to create the class!\n", DRIVER_NAME);
        unregister_chrdev(driver_major_number, DRIVER_NAME);
        return PTR_ERR(rpi_class);
    }

    /*  Create the device   */
    rpi_device = device_create(rpi_class, NULL, MKDEV(driver_major_number, 0), NULL, DRIVER_NAME);
    if(IS_ERR(rpi_device)){
        // write an error message for debugging.
        pr_err("%s: Failed to create the device!\n", DRIVER_NAME);
        class_destroy(rpi_class);
        unregister_chrdev(driver_major_number, DRIVER_NAME);
        return PTR_ERR(rpi_device);
    }

    pr_info("PIR Driver loaded\n");
    return 0;

    resolve:
        gpio_free(PIR_SENSOR_IO_PIN);
        return xRetVal;
}

static void __exit pir_driver_exit(void){
    gpio_free(PIR_SENSOR_IO_PIN);
    gpio_free(LED_IO_PIN);
    device_destroy(rpi_class, MKDEV(driver_major_number, 0));
    class_destroy(rpi_class);
    unregister_chrdev(driver_major_number, DRIVER_NAME);
    pr_info("PIR Driver unloaded!\n");
}

MODULE_AUTHOR("Sanket Gunjal");
MODULE_DESCRIPTION("Driver to test the PIR Sensor");
MODULE_LICENSE("GPL");


module_init(pir_driver_init);
module_exit(pir_driver_exit);


