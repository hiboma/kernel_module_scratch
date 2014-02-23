obj-m := register_chrdev.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod register_chrdev.ko

rmmod:
	rmmod register_chrdev.ko

reload:
	-make rmmod
	make insmod
