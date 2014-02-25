obj-m := workqueue.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod workqueue.ko

rmmod:
	rmmod workqueue.ko

reload:
	-make rmmod
	make insmod
