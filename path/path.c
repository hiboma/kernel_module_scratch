#include <linux/module.h>
#include <linux/namei.h>
#include <linux/mount.h>
#include <linux/fs.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("path test");
MODULE_LICENSE("GPL");

static int __init path_init(void)
{
	int res = 0;
	struct path path;
	struct vfsmount *mnt;
	struct dentry *dentry;
	struct nameidata nd;

	res = kern_path("/", LOOKUP_FOLLOW, &path);
	if (res) {
		pr_err("failed kern_path: %d\n", res);
		return res;
	}

	mnt = mntget(path.mnt);
	dentry = dget(path.dentry);
	path_put(&path);

	pr_info("devname: %s\n", mnt->mnt_devname);
	pr_info("inode: %lu\n", dentry->d_inode->i_ino);

	mntput(mnt);
	dput(dentry);

	res = path_lookup("/tmp", LOOKUP_PARENT, &nd);
	if (res) {
		pr_err("failed path_lookup: %d\n", res);
		return res;
	}
	pr_info("path_lookup: %s\n", nd.saved_names);
	
	return 0;
}

static void __exit path_exit(void)
{
}

module_init(path_init);
module_exit(path_exit);
