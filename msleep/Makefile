obj-m := msleep.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod msleep.ko

rmmod:
	rmmod msleep.ko

reload:
	-make rmmod
	make insmod
