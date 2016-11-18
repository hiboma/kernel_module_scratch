#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/anon_inodes.h>
#include <linux/fs.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("anon_inode_getfile test");
MODULE_LICENSE("GPL");

#define HIBOMA_MINOR     250 /* should not be hardcoded */
#define HIBOMA_VERSION   110
#define HIBOMA_GET_VERSION 0
#define HIBOMA_OPEN_FD     1

/* anon_inode hiboma-anon */
static struct file_operations hiboma_anon_fops = {
/* 	.release        = kvm_vm_release, */
/* 	.unlocked_ioctl = kvm_vm_ioctl, */
/* #ifdef CONFIG_COMPAT */
/* 	.compat_ioctl   = kvm_vm_compat_ioctl, */
/* #endif */
	.llseek		= noop_llseek,
};

static long hiboma_dev_ioctl(struct file *filp,
			  unsigned int ioctl, unsigned long arg)
{
	long r = -EINVAL;

	switch(ioctl) {
	case HIBOMA_GET_VERSION:
		r = HIBOMA_VERSION;
		break;
	case HIBOMA_OPEN_FD:
		r = anon_inode_getfd("hiboma-anon", &hiboma_anon_fops,
				     NULL, O_RDWR | O_CLOEXEC);
	}
	
	return r;
}

/* character device - /dev/hiboma */
static struct file_operations hiboma_chardev_ops = {
	.unlocked_ioctl = hiboma_dev_ioctl,
	.llseek		= noop_llseek,
};

static struct miscdevice hiboma_dev = {
	KVM_MINOR,
	"hiboma",
	&hiboma_chardev_ops,
};

static int __init anon_inode_getfile_init(void)
{
	int r;

	r = misc_register(&hiboma_dev);
	if (r) {
		pr_err("hiboma: misc device register failed\n");
		return r;
	}
	pr_info("registerd /dev/hiboma \n");

	return 0;
}

static void __exit anon_inode_getfile_exit(void)
{
	int r;
	r = misc_deregister(&hiboma_dev);
	if (r)
		pr_err("hiboma: misc device deregister failed\n");

	pr_info("deregisterd /dev/hiboma \n");
}

module_init(anon_inode_getfile_init);
module_exit(anon_inode_getfile_exit);
