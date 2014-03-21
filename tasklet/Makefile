obj-m := tasklet.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod tasklet.ko

rmmod:
	rmmod tasklet.ko

reload:
	-make rmmod
	make insmod
