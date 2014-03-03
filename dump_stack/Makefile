obj-m := dump_stack.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod dump_stack.ko

rmmod:
	rmmod dump_stack.ko

reload:
	-make rmmod
	make insmod
