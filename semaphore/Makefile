obj-m := semaphore.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod semaphore.ko

rmmod:
	rmmod semaphore.ko

reload:
	-make rmmod
	make insmod
