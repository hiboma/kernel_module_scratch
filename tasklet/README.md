## tasklet

 * Bottom Half
 * Interrupt Context

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

複数の tasklet が TASKLET_SOFTIRQ を共有するので制約が大きい

## HI_SOFTIRQ だと優先度高い tasklet になる

優先度は下記の通り

```c
enum
{
	HI_SOFTIRQ=0,
	TIMER_SOFTIRQ,
	NET_TX_SOFTIRQ,
	NET_RX_SOFTIRQ,
	BLOCK_SOFTIRQ,
	BLOCK_IOPOLL_SOFTIRQ,
	TASKLET_SOFTIRQ,
	SCHED_SOFTIRQ,
	HRTIMER_SOFTIRQ,
	RCU_SOFTIRQ,	/* Preferable RCU should always be the last softirq */

	NR_SOFTIRQS
};
```