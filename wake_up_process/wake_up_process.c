#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/sched.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("wake_up_process test");
MODULE_LICENSE("GPL");

static struct dentry *debugfs_file, *debugfs_dir;
static int value;
static struct task_struct *p;
static spinlock_t lock = SPIN_LOCK_UNLOCKED;

static ssize_t wup_read(struct file *file, char __user *buf,
			size_t count, loff_t *pos)
{
	int want_to_sleep = 0;

	spin_lock(&lock);
	if (!p) {
		p = current;
		want_to_sleep = 1;
	}
	spin_unlock(&lock);

	if (want_to_sleep) {
		set_task_state(current, TASK_UNINTERRUPTIBLE);
		schedule();
		/* wake_up_process が呼ばれるまで眠る */

		spin_lock(&lock);
		p = NULL;
		spin_unlock(&lock);
	}

	return 0;
}

static ssize_t wup_write(struct file *file, const char __user *buf,
			 size_t count, loff_t *pos)
{
	struct task_struct *sleeper = NULL;

	spin_lock(&lock);
	sleeper = p;
	spin_unlock(&lock);

	if (sleeper) {
		wake_up_process(sleeper);
		printk("Sleepers, Wake");
	}

	return count;
}

static struct file_operations fops = {
	.read  = wup_read,
	.write = wup_write,
};

static int __init wake_up_process_init(void)
{
	debugfs_dir = debugfs_create_dir("wake_up_process", NULL);
	if (!debugfs_dir) {
		printk("failed to debugfs_create_dir('wake_up_process')");
		return -ENODEV;
	}

	debugfs_file = debugfs_create_file("fuga", 0666,
					   debugfs_dir, &value, &fops);
	if (!debugfs_file) {
		printk("failed to debugfs_create_dir('wake_up_process')");
		return -ENODEV;
	}
	return 0;
}

static void __exit wake_up_process_exit(void)
{
	debugfs_remove(debugfs_file);
	debugfs_remove(debugfs_dir);
}

module_init(wake_up_process_init);
module_exit(wake_up_process_exit);
