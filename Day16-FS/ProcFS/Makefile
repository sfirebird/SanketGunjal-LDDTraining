obj-m += procfs.o

HEADERS = /lib/modules/$(shell uname -r)/build

all:
	make -C $(HEADERS) M=$(PWD) modules

clean:
	make -C $(HEADERS) M=$(PWD) clean