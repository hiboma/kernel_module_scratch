#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/debugfs.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("mutex test");
MODULE_LICENSE("GPL");

DEFINE_MUTEX(oreore_mutex);

static struct dentry *debugfs_mutex;
static int value;

static ssize_t mutex_write(struct file *file, const char __user *buf,
			 size_t count, loff_t *pos)
{
	mutex_lock(&oreore_mutex);
	return count;
}

static ssize_t mutex_read(struct file *file, char __user *buf,
			 size_t count, loff_t *pos)
{
	mutex_unlock(&oreore_mutex);
	return 0;
}

static struct file_operations fops = {
	.read	= mutex_read,
	.write	= mutex_write,
};

static int __init mutex_module_init(void)
{
	debugfs_mutex = debugfs_create_file("mutex", 0666, NULL, &value, &fops);
	return 0;
}

static void __exit mutex_module_exit(void)
{
	debugfs_remove_recursive(debugfs_mutex);
}

module_init(mutex_module_init);
module_exit(mutex_module_exit);
