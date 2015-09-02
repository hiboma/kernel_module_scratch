#include <linux/module.h>
#include <linux/debugfs.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("msleep test");
MODULE_LICENSE("GPL");

static int v;
static struct dentry *msleep_file;

static ssize_t debugfs_msleep_read(struct file *file, char __user *userbuf,
				 size_t count, loff_t *ppos)
{
	msleep(1000 * 10);
	return 0;
} 

static ssize_t debugfs_msleep_write(struct file *file, const char __user *buf,
				  size_t count, loff_t *ppos)
{
	return count;
}

static struct file_operations fops = {
	.read  = debugfs_msleep_read,
	.write = debugfs_msleep_write,
};

static int __init msleep_init(void)
{
	msleep_file = debugfs_create_file("msleep", 0666, NULL, &v, &fops);
	if (!msleep_file) {
		printk("Error msleep");
		return -ENODEV;
	}
	
	return 0;
}

static void __exit msleep_exit(void)
{
	debugfs_remove(msleep_file);
}

module_init(msleep_init);
module_exit(msleep_exit);
