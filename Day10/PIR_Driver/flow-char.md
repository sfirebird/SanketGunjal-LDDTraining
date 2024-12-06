# Understanding the and building the flow of the pir_driver

#### Initialization:      
Whatever pin that you want to use for the pir sensor and the led, you need to the following in initializaiton:
- request the gpio_pin that you are using
- set the direction of the requested gpio_pin
- register the driver

Instead of using the mknode command manually through command line, I can just use the device_create function, these are the steps:

Include the ```<linux/device.h>``` header file

Read more about Linux Device Model [here](https://linux-kernel-labs.github.io/refs/heads/master/labs/device_model.html?highlight=device_create#:~:text=The%20device_create()%20function%20initializes,device%20(%20minor%3Amajor%20).)

1. Get a device number (major number), you can assign your own number or let the kernel give you the number dynamically.
    c
    `` 
    int register_chrdev(unsigned int major, const char *name, const struct file_operation *fops)
    ``
2. You need to create a device class in /sys/class/ and this is required for creating a device node.
    - Create a structure, ```struct class *dev_class```
    - in ```class.h``` header file, ```struct class * __must_check class_create(const char *name);```
    - call the api and allocate the return the dev_class, ```dev_class = class_create(CLASS_NAME);```
3. Create a device belonging to the class, in /dev/ directory.

NOTE:
- In clean up function, the way you destroy the resources also matters, for example using the function class_destroy() before device_destroy()


Pending:
0. write about device_create();
1. write about MKDEV()
2. write about PTR_ERR()
3. class_destroy()
4. device_destroy()

