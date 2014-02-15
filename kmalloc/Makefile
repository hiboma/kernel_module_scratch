obj-m := kmalloc.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod kmalloc.ko

rmmod:
	rmmod kmalloc.ko
