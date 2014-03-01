#include <linux/module.h>
#include <linux/debugfs.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("spinlock test");
MODULE_LICENSE("GPL");

spinlock_t lock;
static int value;
static struct dentry *dentry_spinlock;

static ssize_t spinlock_read(struct file *file, char __user *buf,
			 size_t count, loff_t *pos)
{
	int counter = 0;
	spinlock_t *lockp;
	unsigned long flags;
	
	spin_lock(&lock);
	counter++;
	spin_unlock(&lock);

	spin_lock_irqsave(&lock, flags);
	counter++;
	spin_unlock_irqrestore(&lock, flags);

	lockp = kmalloc(sizeof(spinlock_t), GFP_KERNEL);
	if (!lockp)
		return -ENOMEM;

	spin_lock_init(lockp);
	spin_lock(lockp);
	counter++;
	spin_unlock(lockp);
	kfree(lockp);

	return 0;
}

static ssize_t spinlock_write(struct file *file, const char __user *buf,
			 size_t count, loff_t *pos)
{
	/* ＼(^o^)／ */
	//spin_lock_irqsave&lock);
	//spin_lock_irqsave(&lock);
	spin_lock(&lock);
	spin_lock(&lock);
	return count;
}

static struct file_operations fops = {
	.read  = spinlock_read,
	.write = spinlock_write,
};

static int __init spinlock_init(void)
{
	dentry_spinlock = debugfs_create_file("spinlock", 0666, NULL, &value, &fops);
	if (!dentry_spinlock) {
		return -ENODEV;
	}
	return 0;
}

static void __exit spinlock_exit(void)
{
	debugfs_remove(dentry_spinlock);
}

module_init(spinlock_init);
module_exit(spinlock_exit);
