//
// base code is take from http://wiki.bit-hive.com/north/pg/kthread_run%A5%DE%A5%AF%A5%ED
//
#include <linux/module.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/jiffies.h>

MODULE_AUTHOR("hiorya");
MODULE_DESCRIPTION("kthread test");
MODULE_LICENSE("GPL");

static struct task_struct *kthread_tsk;
static void my_kthread_main(void)
{
       set_current_state(TASK_INTERRUPTIBLE);
       // sleep に相当
       schedule_timeout(1 * HZ);

       // 起床後何かをする
       printk("kthread:%ld\n", jiffies);
}

// thread のエントリポイント
static int my_kthread(void *arg)
{
       printk("HZ:%d\n", HZ);
       while (!kthread_should_stop()) {
               my_kthread_main();
       }
       return 0;
}

static int __init kthread_test_init(void)
{
       kthread_tsk = kthread_run(my_kthread, NULL, "kthread %s&%d", "hiboma", 123);
       if (IS_ERR(kthread_tsk))
               return -1;
       printk("pid->%d:prio->%d:comm->%s\n",
               kthread_tsk->pid,
               kthread_tsk->static_prio,
               kthread_tsk->comm);
       return 0;
}

static void __exit kthread_test_exit(void)
{
       kthread_stop(kthread_tsk);
}

module_init(kthread_test_init);
module_exit(kthread_test_exit);
