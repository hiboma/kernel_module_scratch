#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/gfp.h>
#include <linux/mm.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("alloc_pages test");
MODULE_LICENSE("GPL");

static int value;
static struct dentry *dentry_alloc_pages;

static ssize_t alloc_pages_write(struct file *file, const char __user *buf,
			 size_t count, loff_t *pos)
{
	unsigned long addr;
	void *p;
	struct page *page;

	page = alloc_pages(GFP_KERNEL, 0);
	if (!page)
		return -ENOMEM;

	p = page_address(page);
	pr_info("GFP_KERNEL - %p\n", &p);
	__free_pages(page, 0);

	page = alloc_pages(GFP_HIGHUSER, 0);
	if (!page)
		return -ENOMEM;

	p = page_address(page);
	pr_info("GFP_HIGHUSER - %p\n", &p);
	__free_pages(page, 0);

	page = alloc_pages(GFP_ATOMIC, 0);
	if (!page)
		return -ENOMEM;

	p = page_address(page);
	pr_info("GFP_ATOMIC - %p\n", &p);
	__free_pages(page, 0);

	/* unsigned long */
	addr = get_zeroed_page(GFP_KERNEL);
	if (!addr)
		return -ENOMEM;

	pr_info("get_zeroed_page - GFP_KERNEL %lx\n", addr);
	free_pages(addr, 0);
	return count;
}

static ssize_t alloc_pages_read(struct file *file, char __user *buf,
				size_t count, loff_t *pos)
{
	return 0;
}

static const struct file_operations fops = {
	.read  = alloc_pages_read,
	.write = alloc_pages_write,
};

static int __init alloc_pages_init(void)
{
	 dentry_alloc_pages = debugfs_create_file("alloc_pages",
						0666, NULL, &value, &fops);
	return 0;
}

static void __exit alloc_pages_exit(void)
{
	debugfs_remove(dentry_alloc_pages);
}

module_init(alloc_pages_init);
module_exit(alloc_pages_exit);
