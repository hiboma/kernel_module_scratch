#include <linux/module.h>

MODULE_AUTHOR("hiorya");
MODULE_DESCRIPTION("WARN_ON test");
MODULE_LICENSE("GPL");

static int __init warn_on_init(void)
{
	WARN_ON(NULL == NULL);
	return 0;
}

static void __exit warn_on_exit(void)
{
}

module_init(warn_on_init);
module_exit(warn_on_exit);
