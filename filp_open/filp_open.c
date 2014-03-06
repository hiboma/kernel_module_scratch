#include <linux/module.h>
#include <linux/fs.h>
#include <linux/bio.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("filp_open test");
MODULE_LICENSE("GPL");

static const char *path = "/tmp/hoge.txt";
static struct file *f;

static int __init filp_open_init(void)
{
	int ret = 0;
	int rc = 0;
	loff_t pos = 0;
	struct inode *inode;
	mm_segment_t old_fs;

	f = filp_open(path, O_RDWR|O_LARGEFILE|O_CREAT|O_APPEND, 0666);
	if (IS_ERR(f)) {
		ret = PTR_ERR(f);
		pr_err("filp_open(%s) failed, errno = %d\n", path, ret);
		return ret;
	}

	inode = f->f_mapping->host;
	pr_info("%d %d\n", inode->i_uid, inode->i_gid);

	old_fs = get_fs();
	set_fs(get_ds());
	ret = vfs_write(f, "1234567890\n", 10+1, &pos);
	set_fs(old_fs);

	if (rc < 0) {
		pr_err("vfs_write failed %d", rc);
		return -rc;
	}

	return ret;
}

static void __exit filp_open_exit(void)
{
	int ret;
	ret = filp_close(f, NULL);
	if (ret)
		pr_err("filp_close %d\n", ret);
}

module_init(filp_open_init);
module_exit(filp_open_exit);
