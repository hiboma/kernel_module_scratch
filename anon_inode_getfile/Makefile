obj-m := anon_inode_getfile.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod anon_inode_getfile.ko

rmmod:
	rmmod anon_inode_getfile.ko

reload:
	-make rmmod
	make insmod
