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
	pid_t pid;
} job_t;

job_t *job;

// ワーカーのコールバック
static void callback_function(struct work_struct *work)
{
	job_t *j;

	printk("I'm waiting %d\n", current->pid);
	set_current_state(TASK_INTERRUPTIBLE);
	schedule_timeout(3 * HZ);

	j = (job_t *)work;
	printk("Hello,world %d, caller is %d\n", current->pid, j->pid);
}

static ssize_t wq_write(struct file *file, const char __user *buf,
			 size_t count, loff_t *pos)
{
	// writeするとワーカーにキューイングする
	if (!queue_work(queue, (struct work_struct *)job)) {
		printk("work is pending\n");
	}
	return count;
}

static struct file_operations fops = {
	.write = wq_write,
};

static int __init workqueue_init(void)
{
	// キューイング用に debugfs でエントリポイントを作る
	dir_dentry = debugfs_create_dir("workqueue", NULL);
	if (!dir_dentry) {
		printk("failed debugfs_create_dir");
		return -ENODEV;
	}
	file_dentry = debugfs_create_file("enqueue", 0666, dir_dentry, &value, &fops);

	// work_struct 用のキューを作成する
	queue = create_workqueue("workqueue-sample");
	if (!queue) {
		debugfs_remove_recursive(dir_dentry);
		return -ENOMEM;
	}

	job = (job_t *)kmalloc(sizeof(job_t), GFP_KERNEL);
	if (!job) {
		debugfs_remove_recursive(dir_dentry);
		destroy_workqueue(queue);
		return -ENOMEM;
	}

	// ワーカーの初期化
	INIT_WORK((struct work_struct *)job, callback_function);
	job->pid = current->pid;
	return 0;
}

static void __exit workqueue_exit(void)
{
	debugfs_remove_recursive(dir_dentry);
	destroy_workqueue(queue);
}

module_init(workqueue_init);
module_exit(workqueue_exit);
