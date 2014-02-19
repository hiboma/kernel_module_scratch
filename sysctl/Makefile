obj-m := sysctl.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod sysctl.ko

rmmod:
	rmmod sysctl.ko
