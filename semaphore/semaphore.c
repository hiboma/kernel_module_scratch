#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/semaphore.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("semaphore test");
MODULE_LICENSE("GPL");

static DECLARE_MUTEX(sem);
static int value;
static struct dentry *dentry_semaphore;

static ssize_t sem_write(struct file *file, const char __user *buf,
			 size_t count, loff_t *pos)
{
	int ret = 0;

	/* possible to return -EINTR  */
	ret = down_interruptible(&sem);
	if (ret)
		return ret;

	return count;
}

static ssize_t sem_read(struct file *file, char __user *buf,
			 size_t count, loff_t *pos)
{
	up(&sem);
	return 0;
}

static const struct file_operations fops = {
	.read  = sem_read,
	.write = sem_write,
};

static int __init semaphore_init(void)
{
	dentry_semaphore = debugfs_create_file("semaphore", 0666,
					       NULL, &value, &fops);
	return 0;
}

static void __exit semaphore_exit(void)
{
	debugfs_remove(dentry_semaphore);
}

module_init(semaphore_init);
module_exit(semaphore_exit);
