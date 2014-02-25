#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/sched.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("workqueue test");
MODULE_LICENSE("GPL");

static int value;
static struct dentry *dir_dentry, *file_dentry;
static struct workqueue_struct *queue;

typedef struct {
	struct work_struct work;
} job_t;

static job_t *job;

// ワーカーのコールバック
static void callback_function(struct work_struct *work)
{
	job_t *current_job;

	printk("[%d] I'm waiting for schedule_timeout\n", current->pid);
	set_current_state(TASK_INTERRUPTIBLE);
	/* 3秒待つ */
	schedule_timeout(3 * HZ);

	current_job = (job_t *)work;
	printk("[%d] Hello world\n", current->pid);
}

static ssize_t wq_write(struct file *file, const char __user *buf,
			 size_t count, loff_t *pos)
{
	// writeするとワーカーにキューイングする
	// 非同期で処理される
	if (!queue_work(queue, (struct work_struct *)job)) {
		printk("work is pending\n");
		return -EAGAIN;
	}
	return count;
}

static struct file_operations fops = {
	.write = wq_write,
};

static int __init workqueue_init(void)
{
	int ret = -ENODEV;
	
	/* キューイング用に debugfs でエントリポイントを作る */
	dir_dentry = debugfs_create_dir("workqueue", NULL);
	if (!dir_dentry) {
		printk("failed debugfs_create_dir");
		goto failed;
	}
	file_dentry = debugfs_create_file("enqueue", 0666, dir_dentry, &value, &fops);

	/* work_struct 用のキューを作成する */
	ret = -ENODEV;
	queue = create_workqueue("workqueue-sample");
	if (!queue)
		goto remove_debugfs;

	job = (job_t *)kmalloc(sizeof(job_t), GFP_KERNEL);
	if (!job)
		goto destroy_queue;

	/* ワーカーの初期化 */
	INIT_WORK((struct work_struct *)job, callback_function);
	return 0;

destroy_queue:
	destroy_workqueue(queue);

remove_debugfs:
	debugfs_remove_recursive(dir_dentry);

failed:
	return ret;
}

static void __exit workqueue_exit(void)
{
	debugfs_remove_recursive(dir_dentry);
	destroy_workqueue(queue);
}

module_init(workqueue_init);
module_exit(workqueue_exit);
