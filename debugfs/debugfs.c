#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/sched.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("debugfs test");
MODULE_LICENSE("GPL");

static u8 value;
static int file_value;
static struct dentry *dentry_u8, *dentry_file;
static wait_queue_head_t queue;

static ssize_t debugfs_test_read(struct file *file, char __user *userbuf,
				 size_t count, loff_t *ppos)
{
	return 0;
} 

static ssize_t debugfs_test_write(struct file *file, const char __user *buf,
				  size_t count, loff_t *ppos)
{
	return count;
}

static struct file_operations fops = {
	.read  = debugfs_test_read,
	.write = debugfs_test_write,
};

static int __init debugfs_init(void)
{
	//DECLARE_WAIT_QUEUE_HEAD(queue);
	init_waitqueue_head(&queue);

	dentry_u8 = debugfs_create_u8("u8", 0666, NULL, &value);
	if (!dentry_u8) {
		printk("Error failed debugfs_create_u8");
		return -ENODEV;
	}

	dentry_file = debugfs_create_file("fuga", 0666, NULL, &file_value, &fops);
	if (!dentry_file) {
		printk("Error failed debugfs_create_u8");
		return -ENODEV;
	}
		
	return 0;
}

static void __exit debugfs_exit(void)
{
	debugfs_remove(dentry_u8);
	debugfs_remove(dentry_file);
}

module_init(debugfs_init);
module_exit(debugfs_exit);
