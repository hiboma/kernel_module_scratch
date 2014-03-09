#include <linux/module.h>
#include <linux/debugfs.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("wait_for_completion test");
MODULE_LICENSE("GPL");

static struct dentry *dentry_dir, *dentry_file;
DECLARE_COMPLETION(done);

static ssize_t completion_read(struct file *file, const char __user *buf,
				  size_t count, loff_t *ppos)
{
	/* TASK_UNINTERRUPTIBLE */
	wait_for_completion_killable(&done);
	return 0;
}

static ssize_t completion_write(struct file *file, const char __user *buf,
				  size_t count, loff_t *ppos)
{
	complete(&done);
	return count;
}

static const struct file_operations fops = {
	.read	= completion_read,
	.write	= completion_write,
};

static int __init wait_for_completion_init(void)
{
	dentry_dir = debugfs_create_dir("wait_for_completion", NULL);
	if (!dentry_dir) {
		pr_info("failed to debugfs_create_directory");
		return -ENODEV;
	}
	dentry_file = debugfs_create_file("completion", 0666,
					  dentry_dir, NULL, &fops);
	return 0;
}

static void __exit wait_for_completion_exit(void)
{
	debugfs_remove(dentry_file);
	debugfs_remove(dentry_dir);
}

module_init(wait_for_completion_init);
module_exit(wait_for_completion_exit);
