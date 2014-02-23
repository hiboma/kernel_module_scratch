obj-m := wait_queue_head_t.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod wait_queue_head_t.ko

rmmod:
	rmmod wait_queue_head_t.ko

reload:
	-make rmmod
	make insmod
