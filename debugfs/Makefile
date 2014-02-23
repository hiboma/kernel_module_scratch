obj-m := debugfs.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod debugfs.ko

rmmod:
	rmmod debugfs.ko

reload:
	-make rmmod
	make insmod

