#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/i2c.h>
#include <linux/fs.h>
#include <linux/of.h>
#include <linux/uaccess.h>

/*  Create a private structure specific to the device wrapping the i2c_client structure */
struct stm32_client {
    struct i2c_client *client;
    struct miscdevice stm32_client_miscop;  /*  This will allow us to implement the fops functionality  */  
    char name[8];                         /*  Provide a name to your device   */
};

/**
 *  Implement user_read and user_write functionality.
 *  stm32_client_read()
 *  stm32_client_write()
 */
static ssize_t stm32_client_read(struct file *fp, char __user *u_buffer, size_t size, loff_t *offset){
    return 0;

}

static ssize_t stm32_client_write(struct file *fp, const char __user *u_buffer, size_t size, loff_t *offset){
    int xRet;
    unsigned long val;
    char buf[4];
    int bytes_written;
 
    struct stm32_client *stm32_device;

    /*  what is the use of container?   */
    stm32_device = container_of(fp->private_data, struct stm32_client, stm32_client_miscop);

    bytes_written = copy_from_user(buf, u_buffer, size);
    
    /* Convert char array to char string */
    buf[size-1] = '\0';
    
    /* Convert the string to an unsigned long */
    xRet = kstrtoul(buf, 0, &val);

    i2c_smbus_write_byte(stm32_device->client, val);
    
    return bytes_written;
}


/*  Regsiter the user_read and user_write with fops */
static const struct file_operations stm32_fops = {
    .owner = THIS_MODULE,
    .read = stm32_client_read,
    .write = stm32_client_write,
};

/*  Implement the probe functionality   */
static int stm32_client_probe(struct i2c_client *client){
    static int counter = 0;
    
    /*  Allocate the memory using devm_kzalloc  */
    struct stm32_client *stm32_device;

    stm32_device = devm_kzalloc(&client->dev, sizeof(struct stm32_client), GFP_KERNEL);

    /*  */
    i2c_set_clientdata(client, stm32_device);
    stm32_device->client = client;

    /*  Initialize the misc device, the counter will be incremented with every probe()  */
    sprintf(stm32_device->name, "stm32_device%02d", counter++);
    dev_info(&client->dev, "stm32 probe on = %s\n", stm32_device->name);
    pr_info("stm32_probe!");

    stm32_device->stm32_client_miscop.name = stm32_device->name;
    stm32_device->stm32_client_miscop.minor = MISC_DYNAMIC_MINOR;
    stm32_device->stm32_client_miscop.fops = &stm32_fops;

    /*  Register the misc device    */
    misc_register(&stm32_device->stm32_client_miscop);

    dev_info(&client->dev, "stm32_probe is exited on %s\n", stm32_device->name);

    return 0;
}

/*  Implement the remove functionality  */
static void stm32_client_remove(struct i2c_client *client){
    /*  declare the private data structure  */
    struct stm32_client *stm32_device;
    
    /*  Get the device structure from the bus device context    */
    stm32_device = i2c_get_clientdata(client);

    dev_info(&client->dev, "stm32_remove on %s\n", stm32_device->name);

    /*  Deregister the device   */
    misc_deregister(&stm32_device->stm32_client_miscop);
    dev_info(&client->dev, "stm32_remove exited on %s\n", stm32_device->name);
}

/*  Declare a list of devices that is supported by the driver   */
static const struct of_device_id stm32_dt_ids[] = {
    { .compatible = "STM32F4,STM32"},
    { }
};
MODULE_DEVICE_TABLE(of, stm32_dt_ids);

/*  i2C device id   */
const static struct i2c_device_id stm32_idtable[] = {
    { .name = "stm32" },
    { }
};
MODULE_DEVICE_TABLE(i2c, stm32_idtable);

/*  Instantiate the i2c_driver structure    */
static struct i2c_driver stm32_client_i2c_driver = {
    .driver = {
        .name = "STM32",
        .owner = THIS_MODULE,
        .of_match_table = stm32_dt_ids,
    },
    .probe = stm32_client_probe,
    .remove = stm32_client_remove,
    .id_table = stm32_idtable,
};
/*  When using this macro there is no need for init or exit function    */
module_i2c_driver(stm32_client_i2c_driver);

/*  Module related general information  */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("sanketgunjal@mirafra.com");
MODULE_DESCRIPTION("I2C driver to communicate with stm32_i2c");
