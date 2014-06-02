obj-m := alloc_pages.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod alloc_pages.ko

rmmod:
	rmmod alloc_pages.ko

reload:
	-make rmmod
	make insmod
