#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/uaccess.h>

#define DRIVER_NAME             "PIR Driver"

/*  Define the GPIO Pin interfacing with PIR sensor.
 *  Read from this pin.
 */
#define PIR_SENSOR_IO_PIN       17

/**
 * Define the GPIO Pin connected to the LED
 */
#define LED_IO_PIN              18

static int driver_major_number;

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
static int pir_close(struct inode *inode, struct file *inode){
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

static int pir_read(struct file *file, char __user *buf, size_t len, loff_t *offset){
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
    int bytes_sent;
    if(bytes_sent = copy_to_user(buf, buffer, sizeof(buffer))){
        pr_err("Failed to send data to user\n");
        return -EFAULT;
    }

    pr_info("%s: successfully read %d!\n", get_pir_value);
    return bytes_sent;
}

static void gpio_write(int control_status){
    /*  Depending upon the pir control the led  */
    if(control_status){
        gpio_set_value(LED_IO_PIN, control_status);
        pr_info("%s: Gotcha! Turning on the Alarm!\n");
    }
    else{
        
        gpio_set_value(LED_IO_PIN, control_status);
        pr_info("%s: GPIO Low!\n");
    }

    return 0;
}

/**
 * Map your driver functions
 */
static struct file_operations gpio_fops = {
    .open = pir_open,
    .close = pir_close, 
    .read = pir_read, 
};

/**
 * Initialization and cleanup functions
 */
static int __init pir_init(void){
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

    /*  Registe the device  */
    xRetVal = register_chrdev(driver_major_number, DRIVER_NAME, gpio_fops);
    if(xRetVal){
        pr_err("Failed to register the \"%s\" device, Error Returns = %d\n", DRIVER_NAME, xRetVal);
        goto resolve;
    }

    pr_info("GPIO Driver loaded\n");
    return 0;

    resolve:
        gpio_free(PIR_SENSOR_IO_PIN);
        return xRetVal;
}

static int __exit pir_exit(void){
    unregister_chrdev(driver_major_number, DRIVER_NAME);
    gpio_free(PIR_SENSOR_IO_PIN);
    gpio_free(LED_IO_PIN);
    pr_info("PIR Driver unloaded!\n");
}


module_init(pir_init);
module_exit(pir_exit);


