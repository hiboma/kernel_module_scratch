#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/sched.h>

MODULE_AUTHOR("hiroya");
MODULE_DESCRIPTION("tasklet test");
MODULE_LICENSE("GPL");

static unsigned long oreore;
char oreore_data[] = "hello,tasklet\n";

void oreore_tasklet_callback(unsigned long data)
{
	struct task_struct *p = current;
	pr_info("%c [%d] %s cpu:%d %s",
		in_interrupt() ? 'Y' : 'N',
		p->pid,
		p->comm,
		smp_processor_id(),
		(char *)data);
}

DECLARE_TASKLET(oreore_tasklet, oreore_tasklet_callback,
		(unsigned long)&oreore_data);

static irqreturn_t oreore_irq_handler(int irq, void *dev_id)
{
	tasklet_schedule(&oreore_tasklet);
	return IRQ_HANDLED;
}

static int __init oreore_tasklet_init(void)
{
	int ret = 0;
	ret = request_irq(1, oreore_irq_handler,
		    IRQF_SHARED, "oreore", (void *)&oreore);
	if (ret)
		pr_err("failed request_irq: %d", ret);

	return ret;
}

static void __exit oreore_tasklet_exit(void)
{
	free_irq(1, (void *)&oreore);
	tasklet_kill(&oreore_tasklet);
}

module_init(oreore_tasklet_init);
module_exit(oreore_tasklet_exit);
