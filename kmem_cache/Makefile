obj-m := kmem_cache.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod kmem_cache.ko

rmmod:
	rmmod kmem_cache.ko

reload:
	-make rmmod
	make insmod
