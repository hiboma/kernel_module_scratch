#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/sched.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("wait_queue_head_t test");
MODULE_LICENSE("GPL");

static int file_value = 0;
static struct dentry *dentry_queue;
static wait_queue_head_t queue;

// 入力と出力で sleep/wakeup をさせたいだけで、 read/write である必要は無い
// 他になんか適切なインタフェースないかな

static ssize_t queue_read(struct file *file, char __user *userbuf,
				 size_t count, loff_t *ppos)
{
	// sleep_on(&queue);
	// sleep_on_timeout(&queue, 10 * HZ);
	// interruptible_sleep_on_timeout(&queue, 10 * HZ);
	interruptible_sleep_on(&queue);
	return 0;
} 

static ssize_t queue_write(struct file *file, const char __user *buf,
				  size_t count, loff_t *ppos)
{
	// TASK_UNINTERRUPTIBLE は起床させない
	// wake_up(&queue)
	// wake_up_all(&queue);
	// wake_up_interruptible_all(&queue);
	wake_up_interruptible(&queue);
	return count;
}

static struct file_operations fops = {
	.read  = queue_read,
	.write = queue_write,
};

static int __init wait_queue_head_t_init(void)
{
	init_waitqueue_head(&queue);

	dentry_queue = debugfs_create_file("queue", 0666, NULL, &file_value, &fops);
	if (!dentry_queue) {
		printk("Error failed debugfs_create_u8");
		return -ENODEV;
	}

	return 0;
}

static void __exit wait_queue_head_t_exit(void)
{
	debugfs_remove(&queue);
}

module_init(wait_queue_head_t_init);
module_exit(wait_queue_head_t_exit);
