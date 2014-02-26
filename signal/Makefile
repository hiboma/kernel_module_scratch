obj-m := signal.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod signal.ko

rmmod:
	rmmod signal.ko

reload:
	-make rmmod
	make insmod
