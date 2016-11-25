#include <linux/module.h>
#include <linux/slab.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("kzalloc test");
MODULE_LICENSE("GPL");

void *kz, *km;

static int __init kzalloc_init(void)
{
	kz	= kzalloc(PAGE_SIZE, GFP_KERNEL);
	km	= kmalloc(PAGE_SIZE, GFP_KERNEL);
	pr_info("kzalloc: %p kmalloc: %p\n", kz, km);

	return 0;
}

static void __exit kzalloc_exit(void)
{
	kfree(kz);
	kfree(km);
}

module_init(kzalloc_init);
module_exit(kzalloc_exit);


