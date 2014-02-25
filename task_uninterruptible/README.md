# TASK_UNINTERRUPTIBLE

## Q. ロードアベレージに加算されるか?

yes. サンプルモジュールで下記の様に read まくると TASK_UNINTERRUPTIBLE の分だけ LA が上がるのが観測できる

```
$ cat /mnt/debugfs/task_uninterruptible/file &
$ cat /mnt/debugfs/task_uninterruptible/file &
$ cat /mnt/debugfs/task_uninterruptible/file &
$ cat /mnt/debugfs/task_uninterruptible/file &
```

## 2.6.32

```
/*
 * calc_load - update the avenrun load estimates 10 ticks after the
 * CPUs have updated calc_load_tasks.
 */
void calc_global_load(void)
{
	unsigned long upd = calc_load_update + 10;
	long active;

	if (time_before(jiffies, upd))
		return;

	active = atomic_long_read(&calc_load_tasks);
	active = active > 0 ? active * FIXED_1 : 0;

	avenrun[0] = calc_load(avenrun[0], EXP_1, active);
	avenrun[1] = calc_load(avenrun[1], EXP_5, active);
	avenrun[2] = calc_load(avenrun[2], EXP_15, active);

	calc_load_update += LOAD_FREQ;
}
```

rq に nr_running と nr_uninterruptible があるのは古いスケジューラと一緒


```
/*
 * Either called from update_cpu_load() or from a cpu going idle
 */
static void calc_load_account_active(struct rq *this_rq)
{
	long nr_active, delta;

    // TASK_RUNNING + TASK_UNINTERRUPTIBLE
	nr_active = this_rq->nr_running;
	nr_active += (long) this_rq->nr_uninterruptible;

	if (nr_active != this_rq->calc_load_active) {
		delta = nr_active - this_rq->calc_load_active;
		this_rq->calc_load_active = nr_active;
		atomic_long_add(delta, &calc_load_tasks);
	}
}
```

## 2.6.15

O(1)スケジューラの場合は runqueue の nr_running と nr_uninterruptible を見てるだけでシンプルだった

```c
/*
 * calc_load - given tick count, update the avenrun load estimates.
 * This is called while holding a write_lock on xtime_lock.
 */
static inline void calc_load(unsigned long ticks)
{
	unsigned long active_tasks; /* fixed-point */
	static int count = LOAD_FREQ;

	count -= ticks;
	if (count < 0) {
		count += LOAD_FREQ;
		active_tasks = count_active_tasks();
		CALC_LOAD(avenrun[0], EXP_1, active_tasks);
		CALC_LOAD(avenrun[1], EXP_5, active_tasks);
		CALC_LOAD(avenrun[2], EXP_15, active_tasks);
	}
}
```

count_active_tasks の中身が正解を示している

```c
/*
 * Nr of active tasks - counted in fixed-point numbers
 */
static unsigned long count_active_tasks(void)
{
    // TASK_RUNNING + TASK_UNINTERRUPTIBLE + ?
	return (nr_running() + nr_uninterruptible()) * FIXED_1;
}
```

```c
/*
 * nr_running, nr_uninterruptible and nr_context_switches:
 *
 * externally visible scheduler statistics: current number of runnable
 * threads, current number of uninterruptible-sleeping threads, total
 * number of context switches performed since bootup.
 */
unsigned long nr_running(void)
{
	unsigned long i, sum = 0;

    // 各CPUの runqueue->nr_running を加算
	for_each_online_cpu(i)
		sum += cpu_rq(i)->nr_running;

	return sum;
}

unsigned long nr_uninterruptible(void)
{
	unsigned long i, sum = 0;

	for_each_cpu(i)
		sum += cpu_rq(i)->nr_uninterruptible;

	/*
	 * Since we read the counters lockless, it might be slightly
	 * inaccurate. Do not allow it to go below zero though:
	 */
	if (unlikely((long)sum < 0))
		sum = 0;

	return sum;
}
```