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

typedef struct {
	struct delayed_work work;
} delayed_job_t;

static job_t *job;
static delayed_job_t *delayed_job;

// ワーカーのコールバック
static void callback_function(struct work_struct *work)
{
	job_t *current_job;
	current_job = (job_t *)work;
	printk("[%d] Hello world\n", current->pid);
}

static ssize_t wq_write(struct file *file, const char __user *buf,
			 size_t count, loff_t *pos)
{
    // 遅延してキューイング
    if (!queue_delayed_work(queue, (struct delayed_work *)delayed_job, 5 * HZ)) {
        printk("delayed_job is pending\n");
        return -EAGAIN;
    }

    // 遅延無しでキューイング
    if (!queue_work(queue, (struct work_struct *)job)) {
        printk("work is pending\n");
        return -EAGAIN;
    }

    if (schedule_work((struct work_struct *)job) == 0) {
	pr_info("schedule_work is pending\n");
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
	queue = create_workqueue("workqueue");
	if (!queue)
		goto remove_debugfs;

	job = (job_t *)kmalloc(sizeof(job_t), GFP_KERNEL);
	if (!job)
		goto destroy_queue;

	delayed_job = (delayed_job_t *)kmalloc(sizeof(delayed_job_t), GFP_KERNEL);
	if (!delayed_job)
		goto free_job;

	/* ワーカーの初期化 */
	INIT_WORK((struct work_struct *)job, callback_function);
    INIT_DELAYED_WORK((struct delayed_work *)delayed_job, callback_function);
	return 0;

destroy_queue:
	destroy_workqueue(queue);

remove_debugfs:
	debugfs_remove_recursive(dir_dentry);

free_job:
	kfree(job);

failed:
	return ret;
}

static void __exit workqueue_exit(void)
{
	debugfs_remove_recursive(dir_dentry);
	destroy_workqueue(queue);
	kfree(job);
	kfree(delayed_job);
}

module_init(workqueue_init);
module_exit(workqueue_exit);
