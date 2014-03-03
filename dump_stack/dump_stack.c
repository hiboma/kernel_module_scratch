#include <linux/module.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("dump_stack test");
MODULE_LICENSE("GPL");

static int __init dump_stack_init(void)
{
	dump_stack();
	return 0;
}

static void __exit dump_stack_exit(void)
{
}

module_init(dump_stack_init);
module_exit(dump_stack_exit);
