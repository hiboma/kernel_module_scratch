obj-m := wait_for_completion.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod wait_for_completion.ko

rmmod:
	rmmod wait_for_completion.ko

reload:
	-make rmmod 2>/dev/null
	make insmod
