#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>

#define DRIVER_NAME "rpi4b_i2c_driver"
#define DEVICE_ADDR 0x53  // Replace with your I2C device address

struct rpi4b_i2c_dev {
    struct i2c_client *client;
};

static int rpi4b_i2c_probe(struct i2c_client *client)
{
    struct rpi4b_i2c_dev *dev;
    int ret;
    u8 reg_addr = 0x00;  // Example register address to read from
    u8 data;

    dev_info(&client->dev, "Probing I2C device\n");

    /* Allocate driver data */
    dev = devm_kzalloc(&client->dev, sizeof(struct rpi4b_i2c_dev), GFP_KERNEL);
    if (!dev) {
        dev_err(&client->dev, "Failed to allocate memory\n");
        return -ENOMEM;
    }
    i2c_set_clientdata(client, dev);
    dev->client = client;

    /* Example Read Operation */
    ret = i2c_smbus_read_byte_data(client, reg_addr);
    if (ret < 0) {
        dev_err(&client->dev, "Failed to read data: %d\n", ret);
        return ret;
    }
    data = ret;
    dev_info(&client->dev, "Read data: 0x%02x from reg 0x%02x\n", data, reg_addr);

    return 0;
}

static void rpi4b_i2c_remove(struct i2c_client *client)
{
    dev_info(&client->dev, "Removing I2C device\n");
}

/* I2C Device ID Table */
static const struct i2c_device_id rpi4b_i2c_id[] = {
    { "rpi4b_i2c_device", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, rpi4b_i2c_id);

/* I2C Driver Structure */
static struct i2c_driver rpi4b_i2c_driver = {
    .driver = {
        .name = DRIVER_NAME,
    },
    .probe = rpi4b_i2c_probe,
    .remove = rpi4b_i2c_remove,
    .id_table = rpi4b_i2c_id,
};

static int __init rpi4b_i2c_init(void)
{
    pr_info("Initializing %s\n", DRIVER_NAME);
    return i2c_add_driver(&rpi4b_i2c_driver);
}

static void __exit rpi4b_i2c_exit(void)
{
    pr_info("Exiting %s\n", DRIVER_NAME);
    i2c_del_driver(&rpi4b_i2c_driver);
}

module_init(rpi4b_i2c_init);
module_exit(rpi4b_i2c_exit);

MODULE_AUTHOR("sanket");
MODULE_DESCRIPTION("I2C Device Driver for Raspberry Pi 4B");
MODULE_LICENSE("GPL");
MODULE_VERSION("1.0");