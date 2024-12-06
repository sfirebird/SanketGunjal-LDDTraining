#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/of.h>
#include <linux/gpio/consumer.h>
#include <linux/platform_device.h>
#include <linux/cdev.h>

static struct gpio_desc *pir_gpio_in;
static struct gpio_desc *led_gpio_out;

static int probe_from_pir_overlay(struct platform_device ){

}