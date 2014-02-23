obj-m := wake_up_process.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod wake_up_process.ko

rmmod:
	rmmod wake_up_process.ko

reload:
	-make rmmod
	make insmod
