#include <linux/module.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("timer test");
MODULE_LICENSE("GPL");

static struct timer_list timer;

static void timer_callback(unsigned long __data)
{
	pr_info("tick tack\n");
}

static int __init timer_init(void)
{
	init_timer(&timer);
	timer.expires  = jiffies + 3*HZ; // 3sec
	timer.data     = 0;
	timer.function = timer_callback;
	add_timer(&timer);
	return 0;
}

static void __exit timer_exit(void)
{
	del_timer(&timer);
}

module_init(timer_init);
module_exit(timer_exit);
