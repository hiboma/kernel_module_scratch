obj-m := task_uninterruptible.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod task_uninterruptible.ko

rmmod:
	rmmod task_uninterruptible.ko

reload:
	-make rmmod
	make insmod
