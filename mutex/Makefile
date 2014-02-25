obj-m := mutex.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod mutex.ko

rmmod:
	rmmod mutex.ko

reload:
	-make rmmod
	make insmod
