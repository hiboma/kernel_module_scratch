# http://imoarai.cocolog-nifty.com/blog/2010/05/makefile-15e9.html
subdirs = kthread_run warn_on kmalloc

.PHONY: all $(subdirs)

all: $(subdirs)
$(subdirs):
	$(MAKE) -C $@ 
