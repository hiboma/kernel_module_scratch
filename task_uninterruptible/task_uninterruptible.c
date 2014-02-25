#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/sched.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("task_uninterruptible test");
MODULE_LICENSE("GPL");

static struct dentry *dir_dentry, *file_dentry;
static int value;

static ssize_t tu_read(struct file *file, char __user *buf,
			 size_t count, loff_t *pos)
{
	set_current_state(TASK_UNINTERRUPTIBLE);
	schedule_timeout(100 *HZ);
	return 0;
}

static struct file_operations fops = {
	.read  = tu_read,
};

static int __init task_uninterruptible_init(void)
{
	dir_dentry = debugfs_create_dir("task_uninterruptible", NULL);
	if (!dir_dentry) {
		printk("failed debugfs_create_dir");
		return -ENODEV;
	}
	
	file_dentry = debugfs_create_file("file", 0666, dir_dentry, &value, &fops);
	return 0;
}

static void __exit task_uninterruptible_exit(void)
{
	debugfs_remove_recursive(dir_dentry);
}

module_init(task_uninterruptible_init);
module_exit(task_uninterruptible_exit);
