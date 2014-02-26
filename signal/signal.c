#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/jiffies.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("signal test");
MODULE_LICENSE("GPL");

static struct task_struct *killer;
static int sleep_interval = 5;
static char *victim = "sleep";

/* /sys/module/signal/parameters/sleep_interval で参照できる */
module_param(sleep_interval, int, 0644);
module_param(victim, charp, 0644);

static int interval_killer(void *arg)
{
	struct task_struct *p;

	while (!kthread_should_stop()) {
		for_each_process(p) {

			/* Ignore init task */
			if (p->pid == 1)
				continue;
			/* Ignore kernel thread */
			if (!p->mm)
				continue;

			if (strncmp(p->comm, victim, TASK_COMM_LEN) ==0) {
				printk("SIGKILL pid = %d, comm=%s\n", p->pid, p->comm);
				force_sig(SIGKILL, p);
			}
		}
		
		set_current_state(TASK_INTERRUPTIBLE);
		schedule_timeout(sleep_interval * HZ);
	}

	return 0;
}

static int __init signal_init(void)
{
	killer = kthread_run(interval_killer, NULL, "killer");
	if (IS_ERR(killer))
		return -1;

	printk("sleep_interval=%d, victim=%s\n", sleep_interval, victim);
	return 0;
}

static void __exit signal_exit(void)
{
	kthread_stop(killer);
}

module_init(signal_init);
module_exit(signal_exit);
