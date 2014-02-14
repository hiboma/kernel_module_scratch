obj-m := warn_on.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod warn_on.ko

rmmod:
	rmmod warn_on.ko
