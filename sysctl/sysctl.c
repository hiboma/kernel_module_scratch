#include <linux/module.h>
#include <linux/sysctl.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("sysctl test");
MODULE_LICENSE("GPL");

static int value = 5;
static int min   = 0;
static int max   = 100;

static struct ctl_table_header * test_sysctl_header;

// directory entry in /proc/sys/fugafuga
static ctl_table value_table[] = {
	{
		.ctl_name	= CTL_UNNUMBERED,
		.procname	= "value",
		.data		= &value,
		.maxlen		= sizeof(int),
		.mode		= 0644,
		.proc_handler	= &proc_dointvec_minmax,
		.strategy	= &sysctl_intvec,
		.extra1		= &min,
		.extra2		= &max,
	},
	{ .ctl_name = 0 },
};

// subdirectory in /proc/sys
static ctl_table test_root_table[] = {
	{
		.ctl_name	= CTL_UNNUMBERED,
		.procname	= "fugafuga",
		.mode		= 0555,
		.child		= value_table,
	},
	{ .ctl_name = 0 },
};

static int __init sysctl_init(void)
{
	test_sysctl_header = register_sysctl_table(test_root_table);
	return 0;
}

static void __exit sysctl_exit(void)
{
	unregister_sysctl_table(test_sysctl_header);
}

module_init(sysctl_init);
module_exit(sysctl_exit);
