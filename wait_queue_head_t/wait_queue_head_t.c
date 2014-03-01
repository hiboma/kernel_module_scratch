#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/sched.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("wait_queue_head_t test");
MODULE_LICENSE("GPL");

static int use_define_wait = 0;
static int file_value = 0;
static struct dentry *dentry_queue, *dentry_exclusive, *dentry_wait_event;

/* 待ち行列 wait_queue_head_t -> wait_queue_t -> wait_queue_t -> ... */
static wait_queue_head_t queue;
static int wait_event_condition = 0;

module_param(use_define_wait, int, 0644);

static ssize_t wait_event_read(struct file *file, char __user *buf,
			 size_t count, loff_t *pos)
{
	wait_event(queue, wait_event_condition > 0);
	return 0;
}

static ssize_t wait_event_write(struct file *file, const char __user *buf,
			 size_t count, loff_t *pos)
{
	wait_event_condition++;
	wake_up(&queue);
	return count;
}

static struct file_operations wait_event_fops = {
	.read  = wait_event_read,
	.write = wait_event_write,
};

/*
 * How to change value of 'use_define_wait'
 *   echo 1 | sudo tee /sys/module/wait_queue_head_t/parameters/use_define_wait
 *
 */
static ssize_t exclusive_read(struct file *file, char __user *buf,
			 size_t count, loff_t *pos)
{
	if  (use_define_wait) {
		DEFINE_WAIT(wait);

		printk(KERN_INFO "use DEFINE_WAIT()\n");
		prepare_to_wait_exclusive(&queue, &wait, TASK_UNINTERRUPTIBLE);
		/*
		 * avoid race condition
		 * if ( check some_condition is true ) {
		 *    schedule();
		 * }
		 */
		schedule();
		finish_wait(&queue, &wait);
	} else {
		wait_queue_t wait;

		printk(KERN_INFO "use init_waitqueue_entry()\n");
		init_waitqueue_entry(&wait, current);
		set_current_state(TASK_UNINTERRUPTIBLE);
		add_wait_queue_exclusive(&queue, &wait);
		schedule();
		remove_wait_queue(&queue, &wait);
		set_current_state(TASK_RUNNING);
	}

	return 0;
}

static ssize_t exclusive_write(struct file *file, const char __user *buf,
			 size_t count, loff_t *pos)
{
	/* add_wait_queue_exclusive で追加されたタスクを一個ずつ起床する */
	wake_up(&queue);
	printk(KERN_INFO "Sleepers, Wakeup\n");
	return count;
}

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
	wake_up_interruptible_nr(&queue, 1);
	printk(KERN_INFO "Sleepers, Wakeup\n");
	return count;
}

static struct file_operations fops = {
	.read  = queue_read,
	.write = queue_write,
};

static struct file_operations exclusive_fops = {
	.read  = exclusive_read,
	.write = exclusive_write,
};

static int __init wait_queue_head_t_init(void)
{
	init_waitqueue_head(&queue);

	dentry_queue = debugfs_create_file("queue", 0666, NULL, &file_value, &fops);
	if (!dentry_queue) {
		printk("Error failed debugfs_create_u8");
		return -ENODEV;
	}

	dentry_exclusive = debugfs_create_file("exclusive", 0666, NULL,
					       &file_value, &exclusive_fops);

	dentry_wait_event = debugfs_create_file("wait_event", 0666, NULL,
					       &file_value, &wait_event_fops);
	return 0;
}

static void __exit wait_queue_head_t_exit(void)
{
	debugfs_remove(dentry_queue);
	debugfs_remove(dentry_exclusive);
	debugfs_remove(dentry_wait_event);
}

module_init(wait_queue_head_t_init);
module_exit(wait_queue_head_t_exit);
