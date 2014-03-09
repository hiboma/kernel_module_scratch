# wake_up_process, wake_up_state

 * 指定したタスクを起床させる
 * wait_queue_head_t と違って、task_sruct のポインタは自分で退避しておく
   * ksoftirqd の例を見よう
 * state, schedule() の呼び出しも自分でやる

## API

  * wake_up_process
  * schedule, schedule_timeout

```c  
/**
 * wake_up_process - Wake up a specific process
 * @p: The process to be woken up.
 *
 * Attempt to wake up the nominated process and move it to the set of runnable
 * processes.  Returns 1 if the process was woken up, 0 if it was already
 * running.
 *
 * It may be assumed that this function implies a write memory barrier before
 * changing the task state if and only if any tasks are woken up.
 */
int wake_up_process(struct task_struct *p)
{
	return try_to_wake_up(p, TASK_ALL, 0);
}
EXPORT_SYMBOL(wake_up_process);
```

 * TASK_ALL is ?
 * try_to_wake_up を頑張って読まないといけない

## wake_up_process と ksoftirqd

raise_softirq が ksoftirqd を起床させる際に wake_up_process を呼ぶ

```
void raise_softirq(unsigned int nr)
{
	unsigned long flags;

	local_irq_save(flags);
	raise_softirq_irqoff(nr);
	local_irq_restore(flags);
}

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
    // 割り込みコンテキストでなければ ksoftirqd を起床させる
	if (!in_interrupt())
		wakeup_softirqd();
}
```

wakeup_softirqd の中身は下記の通り

 * cpu_var に ksoftirqd の task_struct ポインタが格納されている
   * softirq は ハードウェア割り込みを受けた CPUごとで実行されるAPI なので per_cpu / __get_cpu_var を使っている
 * wake_up_process で ksoftirqd を起床させて pending している softirq を消化する

```
/*
 * we cannot loop indefinitely here to avoid userspace starvation,
 * but we also don't want to introduce a worst case 1/HZ latency
 * to the pending events, so lets the scheduler to balance
 * the softirq load for us.
 */
void wakeup_softirqd(void)
{
	/* Interrupts are disabled: no need to stop preemption */
	struct task_struct *tsk = __get_cpu_var(ksoftirqd);

	if (tsk && tsk->state != TASK_RUNNING)
		wake_up_process(tsk);
}
```