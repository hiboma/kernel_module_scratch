obj-m := filp_open.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod filp_open.ko

rmmod:
	rmmod filp_open.ko

reload:
	-make rmmod
	make insmod
