obj-m := rpi-gpio-driver.o

ARCH ?= arm64
CROSS_COMPILE ?= aarch64-linux-gnu-
KDIR ?=  ~/linux

all:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(shell pwd) modules

clean:
	make ARCH=$(ARCH) CROSS_COMPILE=$(CROSS_COMPILE) -C $(KDIR) M=$(shell pwd) clean

install:
	scp gpio_driver sam@192.168.0.117:~/
