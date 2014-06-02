obj-m := path.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod path.ko

rmmod:
	rmmod path.ko

reload:
	-make rmmod
	make insmod
