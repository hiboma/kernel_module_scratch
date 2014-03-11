#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/mm.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("kmem_cache test");
MODULE_LICENSE("GPL");

static LIST_HEAD(caches);
static unsigned long current_id;

struct foo_bar {
	unsigned long id;
	/* I know that this is odd */
	char name[PAGE_SIZE];
	struct list_head next;
};

static struct kmem_cache *cachep;
struct dentry *dentry_kmem_cache;
static int value;

static int shrink_foo_bar(struct shrinker *shrink, int nr, gfp_t gfp_mask)
{
	struct foo_bar *foo;
	int trial = nr;
	if (nr) {
		while (!list_empty(&caches) || trial > 0) {
			foo = list_entry(caches.next, struct foo_bar, next);
			list_del(&foo->next);
			kmem_cache_free(cachep, foo);
			trial--;
		}
	}

	return 128;
}

static struct shrinker foo_bar_shrinker = {
	.shrink = shrink_foo_bar,
	.seeks  = 2,
};

static void init_once(void *p)
{
	struct foo_bar *foo = (struct foo_bar *)p;
	foo->id = current_id++;
}

static ssize_t cachep_write(struct file *file, const char __user *buf,
			 size_t count, loff_t *pos)
{
	struct foo_bar *foo;

	foo = (struct foo_bar *)kmem_cache_alloc(cachep, GFP_KERNEL);
	if (!foo)
		return -ENOMEM;

	/* need lock */
	list_add_tail(&foo->next, &caches);
	return count;
}

static const struct file_operations fops = {
	.write = cachep_write,
};

static int __init cachep_init(void)
{
	cachep = kmem_cache_create("foo_bar",
			  sizeof(struct foo_bar),
			  0, SLAB_RECLAIM_ACCOUNT|SLAB_PANIC, init_once);
	if (!cachep)
		return -ENOMEM;

	dentry_kmem_cache = debugfs_create_file("kmem_cache", 0666,
						NULL, &value, &fops);
	if (!dentry_kmem_cache) {
		kmem_cache_destroy(cachep);
		return -ENODEV;
	}

	register_shrinker(&foo_bar_shrinker);
	return 0;
}

static void __exit cachep_exit(void)
{
	struct foo_bar *foo;

	unregister_shrinker(&foo_bar_shrinker);

	/* need lock */
	while (!list_empty(&caches)) {
		foo = list_entry(caches.next, struct foo_bar, next);
		list_del(&foo->next);
		kmem_cache_free(cachep, foo);
	}

	kmem_cache_destroy(cachep);
	debugfs_remove(dentry_kmem_cache);
}

module_init(cachep_init);
module_exit(cachep_exit);
