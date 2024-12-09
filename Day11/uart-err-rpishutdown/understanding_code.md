## Understanding the uart_driver.c

#### Why do we need to specify the owner? what does the kernel do with this information?    
    ```C
        static struct file_operations uart_fops = {
            .owner = THIS_MODULE,
            .open = uart_open,
            .release = uart_release,
            .read = uart_read,
            .write = uart_write,
        }
    ```

- The .owner field in the ```struct file_operations``` is a pointer to the module that “owns” the structure. 
- This field is used to prevent the module from being unloaded while its operations are in use. - Almost all the time, it is simply initialized to THIS_MODULE, a macro defined in ```<linux/module.h>```.

#### What is the difference between the ```register_chrdev``` and  ```alloc_chrdev_region``` ?
- So the cdev is just a character device in linux kernel and you link the file_opeartions structure with the cdev structure to provide the implementation of system calls like read, open...
- ```alloc_chrdev_region()```, dynamically allocates a major number and a range of minor numbers for your device(s).
- ```register_chrdev()```, simpler API that registers a character device with a fixed major number, but you can also let the kernel handle the allocation of the major number.
- Does not give you the control that ```alloc_chrdev_region()``` gives.

Steps when using the cdev:        
- Allocate major/minor numbers using ```alloc_chrdev_region()```.       
- Initialize the ```cdev structure```.
- Register the cdev structure with the kernel using ```cdev_add()```.

#### When creating a device after creating a class is it necessary to catch the pointer of type ```struct device *```?



## Behaviour
- After trying to read using /dev/rpi... by using the cat or by using the read() system call the pi automatically shutsdown and when I boot it again I get the something being corrupted in var/log/journal due to not clean shutdown.
