obj-m := kzalloc.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod kzalloc.ko

rmmod:
	rmmod kzalloc.ko

reload:
	-make rmmod
	make insmod
