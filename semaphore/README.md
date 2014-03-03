# semaphore

 * signal_pending_state, schedule_timeout,

## API

 * struct semaphore
   * list で待ち行列

```c 
/* Please don't access any members of this structure directly */
struct semaphore {
	spinlock_t		lock;
	unsigned int		count;
	struct list_head	wait_list;
};
```

 * down
 * down_interruptible
 * down_killable
 * down_timeout

```c
static noinline void __sched __down(struct semaphore *sem)
{
	__down_common(sem, TASK_UNINTERRUPTIBLE, MAX_SCHEDULE_TIMEOUT);
}

static noinline int __sched __down_interruptible(struct semaphore *sem)
{
	return __down_common(sem, TASK_INTERRUPTIBLE, MAX_SCHEDULE_TIMEOUT);
}

static noinline int __sched __down_killable(struct semaphore *sem)
{
	return __down_common(sem, TASK_KILLABLE, MAX_SCHEDULE_TIMEOUT);
}

static noinline int __sched __down_timeout(struct semaphore *sem, long jiffies)
{
	return __down_common(sem, TASK_UNINTERRUPTIBLE, jiffies);
}
``` 

```c 
/*
 * Because this function is inlined, the 'state' parameter will be
 * constant, and thus optimised away by the compiler.  Likewise the
 * 'timeout' parameter for the cases without timeouts.
 */
static inline int __sched __down_common(struct semaphore *sem, long state,
								long timeout)
{
	struct task_struct *task = current;
	struct semaphore_waiter waiter;

    // 末尾に追加
	list_add_tail(&waiter.list, &sem->wait_list);
	waiter.task = task;
	waiter.up = 0;

	for (;;) {
        // TASK_KILLABLE, TASK_INTERRUPTIBLE であればシグナルを pending しているかを見る
		if (signal_pending_state(state, task))
			goto interrupted;
		if (timeout <= 0)
			goto timed_out;
		__set_task_state(task, state);
        
		spin_unlock_irq(&sem->lock);
		timeout = schedule_timeout(timeout);
		spin_lock_irq(&sem->lock);
		if (waiter.up)
			return 0;
	}

 timed_out:
	list_del(&waiter.list);
	return -ETIME;

 interrupted:
	list_del(&waiter.list);
	return -EINTR;
}
```

 * up
