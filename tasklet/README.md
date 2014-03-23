## tasklet

 * Bottom Half
 * Interrupt Context

see also http://www.ibm.com/developerworks/jp/linux/library/l-tasklets/

## API

```c
struct tasklet_struct
{
	struct tasklet_struct *next;
	unsigned long state;
	atomic_t count;
	void (*func)(unsigned long);
	unsigned long data;
};
```

```c
#define DECLARE_TASKLET(name, func, data) \
struct tasklet_struct name = { NULL, 0, ATOMIC_INIT(0), func, data }
```

```c
static inline void tasklet_schedule(struct tasklet_struct *t);
```

```c
void tasklet_kill(struct tasklet_struct *t)
```

## softirq と tasklet

 * softirq の TASKLET_SOFTIRQ を利用しているのが tasklet
   * 複数の tasklet が TASKLET_SOFTIRQ を共有するので制約が大きい
 * 割り込みコンテキスト ( in_interrupt() が true ) として実行される
   * ksoftirqd で実行される場合は別?
 * tasklet_schedule から呼ばれる __tasklet_schedule を見ると raise_softirq_irqoff を使うのが分かる
   * softirq で実行されるので、実行 CPU もsoftirq を出したCPU に縛られる
   * `/proc/irq/<IRQ>/smp_affinity` で変更可能
```c
// tasklet のリスト
// TASKLET_SOFTIRQ を受けたら順番に実行されていく?
static DEFINE_PER_CPU(struct tasklet_head, tasklet_vec);
static DEFINE_PER_CPU(struct tasklet_head, tasklet_hi_vec);

void __tasklet_schedule(struct tasklet_struct *t)
{
	unsigned long flags;

	local_irq_save(flags);
	t->next = NULL;
    // tasklet_struct を リンクリストの末尾に繋ぐ
    // リストの先頭に繋ぐ tasklet_hi_schedule_first なんてのも
    // リストの挿入位置で優先度を変更可能    
	*__get_cpu_var(tasklet_vec).tail = t;
	__get_cpu_var(tasklet_vec).tail = &(t->next);
	raise_softirq_irqoff(TASKLET_SOFTIRQ);
	local_irq_restore(flags);
}
```

 * 割り込みコンテキストでなければ raise_softirq_irqoff で softirqd (ksoftirqd) を起床させる
```c
/*
 * This function must run with irqs disabled!
 */
inline void raise_softirq_irqoff(unsigned int nr)
{
	__raise_softirq_irqoff(nr);

	/*
	 * If we're in an interrupt or softirq, we're done
	 * (this also catches softirq-disabled code). We will
	 * actually run the softirq once we return from
	 * the irq or softirq.
	 *
	 * Otherwise we wake up ksoftirqd to make sure we
	 * schedule the softirq soon.
	 */
	if (!in_interrupt())
		wakeup_softirqd(); // wake_up_process
}
```

 * 割り込みコンテキストだったらどこで実行されるんだっけ?
   * do_IRQ の中の irq_exit
```c
/*
 * Exit an interrupt context. Process softirqs if needed and possible:
 */
void irq_exit(void)
{
	account_system_vtime(current);
	trace_hardirq_exit();
	sub_preempt_count(IRQ_EXIT_OFFSET);
	if (!in_interrupt() && local_softirq_pending())
		invoke_softirq();

#ifdef CONFIG_NO_HZ
	/* Make sure that timer wheel updates are propagated */
	rcu_irq_exit();
	if (idle_cpu(smp_processor_id()) && !in_interrupt() && !need_resched())
		tick_nohz_stop_sched_tick(0);
#endif
	preempt_enable_no_resched();
}
```

合わせて https://github.com/hiboma/kernel_module_scratch/tree/master/request_irq も読もう

## HI_SOFTIRQ だと優先度高い tasklet になる

優先度は下記の通りになる

```c
enum
{
	HI_SOFTIRQ=0,         // 最高優先度
	TIMER_SOFTIRQ,        // タイマ
	NET_TX_SOFTIRQ,       // ネットワーク送信
	NET_RX_SOFTIRQ,       //     ..      受信
	BLOCK_SOFTIRQ,        // ブロックデバイス?
	BLOCK_IOPOLL_SOFTIRQ, // ブロックデバイスの polling 。NAPI的な
	TASKLET_SOFTIRQ, 
	SCHED_SOFTIRQ,        // スケジューラ用 IPI?
	HRTIMER_SOFTIRQ,      // High Resolution Timer 
	RCU_SOFTIRQ,	/* Preferable RCU should always be the last softirq */

	NR_SOFTIRQS
};
```