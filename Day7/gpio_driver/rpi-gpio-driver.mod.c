#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/export-internal.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

#ifdef CONFIG_UNWINDER_ORC
#include <asm/orc_header.h>
ORC_HEADER;
#endif

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif



static const struct modversion_info ____versions[]
__used __section("__versions") = {
	{ 0x122c3a7e, "_printk" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0xfe990052, "gpio_free" },
	{ 0xb742fd7, "simple_strtol" },
	{ 0xca8105b2, "gpio_to_desc" },
	{ 0x385ef24e, "gpiod_set_raw_value" },
	{ 0x12a4e128, "__arch_copy_from_user" },
	{ 0xdcb764ad, "memset" },
	{ 0x7682ba4e, "__copy_overflow" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x47229b5c, "gpio_request" },
	{ 0x849c1982, "gpiod_direction_output_raw" },
	{ 0x4c09d36e, "__register_chrdev" },
	{ 0x637bbcaf, "gpiod_get_raw_value" },
	{ 0x656e4a6e, "snprintf" },
	{ 0x6cbbfc54, "__arch_copy_to_user" },
	{ 0x3939fa16, "module_layout" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "3BC4BFA3BD5B8E3C0B277C1");
