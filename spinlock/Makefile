obj-m := spinlock.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod spinlock.ko

rmmod:
	rmmod spinlock.ko

reload:
	-make rmmod
	make insmod
