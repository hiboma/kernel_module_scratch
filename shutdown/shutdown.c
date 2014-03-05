#include <linux/module.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("shutdown test");
MODULE_LICENSE("GPL");

/*
 * http://stackoverflow.com/questions/18296686/shutdown-embedded-linux-from-kernel-space
 */
static char *shutdown_argv[] = { "/sbin/shutdown", "-h", "-P", "now", NULL };

static int __init shutdown_init(void)
{
	call_usermodehelper(shutdown_argv[0], shutdown_argv, NULL, UMH_NO_WAIT);
	return 0;
}

static void __exit shutdown_exit(void)
{
}

module_init(shutdown_init);
module_exit(shutdown_exit);
