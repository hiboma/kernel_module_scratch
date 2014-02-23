#!/usr/bin/env ruby

require 'fileutils'

mod = ARGV[0] or abort "[usage] #{$0} <module name>"

FileUtils.mkdir_p(mod)

File.write("#{mod}/#{mod}.c", <<-"....")
#include <linux/module.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("#{mod} test");
MODULE_LICENSE("GPL");

static int __init #{mod}_init(void)
{
	return 0;
}

static void __exit #{mod}_exit(void)
{
}

module_init(#{mod}_init);
module_exit(#{mod}_exit);
....

File.write("#{mod}/Makefile", <<-"....")
obj-m := #{mod}.o
KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)
default:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules

insmod:
	insmod #{mod}.ko

rmmod:
	rmmod #{mod}.ko

reload:
	-make rmmod
	make insmod
....
