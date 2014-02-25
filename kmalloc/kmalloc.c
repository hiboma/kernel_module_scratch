#include <linux/module.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("kmalloc test");
MODULE_LICENSE("GPL");

static int __init kmalloc_init(void)
{
	/* GFP_KERNEL */
	char *p, *pp, *zerop;

	p = kmalloc(4, GFP_KERNEL);
	if (!p)  {
		printk(KERN_ERR "ENOMEM");
		return 1;
	}

	p[0] = 'a';
	p[1] = 'b';
	p[2] = 'c';
	p[3] = '\0';
	printk("kmalloc GFP_KERNEL - %s\n", p);
	kfree(p);

	/* GFP_ATOMIC */
	pp = kmalloc(4, GFP_ATOMIC);
	if (!pp)  {
		printk(KERN_ERR "ENOMEM");
		return 1;
	}

	pp[0] = 'a';
	pp[1] = 'b';
	pp[2] = 'c';
	pp[3] = '\0';
	printk("kmalloc GFP_ATOMIC - %s\n", pp);
    
	kfree(pp);

    zerop = kzalloc(4, GFP_KERNEL);
	zerop[0] = 'a';
	zerop[1] = 'b';
	zerop[2] = 'c';
    // zero padding されているからいらない
    // __GFP_ZERO フラグが立っている
    // zerop[3] = '\0';
    printk("kzalloc GFP_KERNEL - %s\n", zerop);
    kfree(zerop);
    
	return 0;
}

static void __exit kmalloc_exit(void)
{
}

module_init(kmalloc_init);
module_exit(kmalloc_exit);
