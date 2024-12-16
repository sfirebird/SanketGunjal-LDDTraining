/*  writing Linux kernel modules. */
#include <linux/module.h>

/*  Provides core functionality, used for logging */
#include <linux/kernel.h>

/*  provides initialization macros  */
#include <linux/init.h>

/*  APIs for handling the GPIO  */
#include <linux/gpio.h>

/*  Helps to interact with the device tree by providing macro and functions */
#include <linux/of.h>

/*  device tree to work with gpio   */
#include <linux/of_gpio.h>

/*  Include the kernel Delay functions  */
#include <linux/delay.h>

/*  Store the led number from the dts   */
static int gpio_led = -1;

/*  Function Prototypes */
static int __init gpio_led_init(void);
static void __exit gpio_led_exit(void);

/**
 * @brief GPIO Initialization Function, responsible for, 
 * - Getting the device tree node for the desired GPIO
 * - Get the GPIO Number from the DTS
 * - Request the GPIO and initialize the GPIO as Input/output or as AF.
 * - Perform any operation on the LED.
 */
static int __init gpio_led_init(void){
    int xRet = 0;

    /*  This will point to device_node in the device_tree   */
    struct device_node *node;
    
    /*  Find a node by its “name” property  */
    node = of_find_node_by_name(NULL, "gpio_led");
    if (!node) {
        pr_err("GPIO LED node not found in Device Tree\n");
        return -ENODEV;
    }

    /*  Used in conjunction with <linux/of.h> header file to get the gpio information from the DT   */
    gpio_led = of_get_named_gpio(node, "gpios", 0);
    if (gpio_led < 0) {
        pr_err("Failed to get GPIO from Device Tree\n");
        return gpio_led;
    }

    pr_info("LED GPIO found: %d\n", gpio_led);

    // Request the GPIO and set it as output
    xRet = gpio_request(gpio_led, "LED GPIO");
    if (xRet) {
        pr_err("Failed to request GPIO %d\n", gpio_led);
        return xRet;
    }

    xRet = gpio_direction_output(gpio_led, 0);  // Start with LED OFF
    if (xRet) {
        pr_err("Failed to set GPIO direction\n");
        gpio_free(gpio_led);
        return xRet;
    }

    // Toggle the LED (turn ON and then OFF after a delay)
    gpio_set_value(gpio_led, 1);  // Turn LED ON
    msleep(1000);  // Wait for 1 second
    gpio_set_value(gpio_led, 0);  // Turn LED OFF

    return 0;
}
/**
 * @brief Free the LED resource utilized by the driver
 */
static void __exit gpio_led_exit(void){
    if(gpio_led >= 0){
        /*  Turn off the led    */
        gpio_set_value(gpio_led, 0);

        /*  Free the resource for reuse */
        gpio_free(gpio_led);
    }

    pr_info("GPIO unloaded successfully!");
}

/*  Register your modules with the Linux Kernel */
module_init(gpio_led_init);
module_exit(gpio_led_exit);

/*  Mdoule Information  */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("SanketGunjal");
MODULE_DESCRIPTION("Driver to control the GPIO Pin using the Device Tree");
