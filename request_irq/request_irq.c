#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/sched.h>
#include <linux/interrupt.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("request_irq test");
MODULE_LICENSE("GPL");

static int value;
static unsigned long oreore;
static wait_queue_head_t queue;
static struct dentry *oreore_dentry;

static ssize_t oreore_irq_write(struct file *file, const char __user *buf,
			 size_t count, loff_t *pos)
{
	sleep_on(&queue);
	return count;
}

static const struct file_operations fops = {
	.write = oreore_irq_write,
};

static irqreturn_t oreore_irq_handler(int irq, void *dev_id)
{
	/* call pr_info in_interrupt() is ok ? */
	pr_info("IRQ [%d] in_interrupt? %c\n", irq, in_interrupt() ? 'Y' : 'N');
	wake_up(&queue);
	/* or return IRQ_NONE */
	return IRQ_HANDLED;
}

static int __init request_irq_init(void)
{
	int ret = 0;

	init_waitqueue_head(&queue);
	oreore_dentry = debugfs_create_file("oreore",
					    0666, NULL, &value, &fops);
	if (!oreore_dentry) {
		pr_err("failed debugfs_create_file");
		return -EINVAL;
	}

	/* Share IRQ (1) line with keyboard device */
	ret = request_irq(1, oreore_irq_handler,
			  IRQF_SHARED, "oreore", (void *)&oreore);
	if (ret) {
		debugfs_remove(oreore_dentry);
		pr_err("failed request_irq");
		return ret;
	}


	return 0;
}

static void __exit request_irq_exit(void)
{
	debugfs_remove(oreore_dentry);
	free_irq(1, (void *)&oreore);
}

module_init(request_irq_init);
module_exit(request_irq_exit);
