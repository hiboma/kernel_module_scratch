#include <linux/module.h>
#include <linux/fs.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("register_chrdev test");
MODULE_LICENSE("GPL");

static int major;

static ssize_t chrdev_read(struct file *file, char __user *buf,
			 size_t count, loff_t *pos)
{
	return 0;
}

static ssize_t chrdev_write(struct file *file, const char __user *buf,
			 size_t count, loff_t *pos)
{
	return count;
}

static struct file_operations fops = {
	.read	= chrdev_read,
	.write	= chrdev_write,
};

static int __init register_chrdev_init(void)
{
	/* 0 is ? */
	major = register_chrdev(0, "register_chrdev", &fops);
	if (major < 0) {
		printk("failed to register_chrdev failed with %d\n", major);
		/* should follow 0/-E convention ... */
		return major;
	}
	printk("/dev/register_chrdev assigned major %d\n", major);
	printk("create node with mknod /dev/register_chrdev c %d 0\n", major);
	return 0;
}

static void __exit register_chrdev_exit(void)
{
	unregister_chrdev(major, "register_chrdev");
}

module_init(register_chrdev_init);
module_exit(register_chrdev_exit);
