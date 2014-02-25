obj-m := timer.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod timer.ko

rmmod:
	rmmod timer.ko

reload:
	-make rmmod
	make insmod
