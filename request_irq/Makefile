obj-m := request_irq.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod request_irq.ko

rmmod:
	rmmod request_irq.ko

reload:
	-make rmmod
	make insmod
