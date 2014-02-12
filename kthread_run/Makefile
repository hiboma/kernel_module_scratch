obj-m := kthread_run.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod kthread_run.ko

rmmod:
	rmmod kthread_run.ko