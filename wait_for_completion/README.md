# wait_for_completion

 * 内部では wait_queue_t, schedule_timeout, set_current_state を組み合わて実装
 * スケジューリングされるのが wait_queue_t API との違い

## API

struct completion

```c
/**
 * struct completion - structure used to maintain state for a "completion"
 *
 * This is the opaque structure used to maintain the state for a "completion".
 * Completions currently use a FIFO to queue threads that have to wait for
 * the "completion" event.
 *
 * See also:  complete(), wait_for_completion() (and friends _timeout,
 * _interruptible, _interruptible_timeout, and _killable), init_completion(),
 * and macros DECLARE_COMPLETION(), DECLARE_COMPLETION_ONSTACK(), and
 * INIT_COMPLETION().
 */
struct completion {
	unsigned int done;
	wait_queue_head_t wait;
};
```

wait_queue_head_t を利用した実装

 * DECLARE_COMPLETION
 * DECLARE_COMPLETION_ONSTACK
 * INIT_COMPLETION
 * wait_for_completion
   * TASK_UNINTERRUPTIBLE
 * wait_for_completion_timeout
   * TASK_UNINTERRUPTIBLE
 * wait_for_completion_interruptible
   * TASK_INTERRUPTIBLE
 * wait_for_completion_interruptible_timeout
   * TASK_INTERRUPTIBLE
 * wait_for_completion_killable
   * TASK_KILLABLE
   * ps の STATE は D だけど、シグナルは受け付ける
   * TASK_INTERRUPTIBLE, TASK_UNINTERRUPTIBLE との使い分けはなんなのだろう
 * complete, complete_all
   * `x->done += UINT_MAX/2;`
   * done に めちゃくちゃ大きな数値を入れて無理矢理感のある終了?
 * schedule_timeout

```
vagrant   4852  0.0  0.0   6076   560 pts/2    D+   01:36   0:00              \_ cat /mnt/debugfs/wait_for_completion/completion
```

 *_timeout 群は内部で long の最大値 (MAX_SCHEDULE_TIMEOUT = LONG_MAX = ((long)(~0UL>>1))) をタイムアウトに指定している
   * MAX_SCHEDULE_TIMEOUT を渡すとタイマをセットせず schedule() する

```c
/**
 * wait_for_completion: - waits for completion of a task
 * @x:  holds the state of this particular completion
 *
 * This waits to be signaled for completion of a specific task. It is NOT
 * interruptible and there is no timeout.
 *
 * See also similar routines (i.e. wait_for_completion_timeout()) with timeout
 * and interrupt capability. Also see complete().
 */
void __sched wait_for_completion(struct completion *x)
{
	wait_for_common(x, MAX_SCHEDULE_TIMEOUT, TASK_UNINTERRUPTIBLE);
}
EXPORT_SYMBOL(wait_for_completion);

/**
 * wait_for_completion_timeout: - waits for completion of a task (w/timeout)
 * @x:  holds the state of this particular completion
 * @timeout:  timeout value in jiffies
 *
 * This waits for either a completion of a specific task to be signaled or for a
 * specified timeout to expire. The timeout is in jiffies. It is not
 * interruptible.
 */
unsigned long _sched
wait_for_completion_timeout(struct completion *x, unsigned long timeout)
{
	return wait_for_common(x, timeout, TASK_UNINTERRUPTIBLE);
}
EXPORT_SYMBOL(wait_for_completion_timeout);

/**
 * wait_for_completion_interruptible: - waits for completion of a task (w/intr)
 * @x:  holds the state of this particular completion
 *
 * This waits for completion of a specific task to be signaled. It is
 * interruptible.
 */
int __sched wait_for_completion_interruptible(struct completion *x)
{
	long t = wait_for_common(x, MAX_SCHEDULE_TIMEOUT, TASK_INTERRUPTIBLE);
	if (t == -ERESTARTSYS)
		return t;
	return 0;
}
EXPORT_SYMBOL(wait_for_completion_interruptible);

/**
 * wait_for_completion_interruptible_timeout: - waits for completion (w/(to,intr))
 * @x:  holds the state of this particular completion
 * @timeout:  timeout value in jiffies
 *
 * This waits for either a completion of a specific task to be signaled or for a
 * specified timeout to expire. It is interruptible. The timeout is in jiffies.
 */
unsigned long __sched
wait_for_completion_interruptible_timeout(struct completion *x,
					  unsigned long timeout)
{
	return wait_for_common(x, timeout, TASK_INTERRUPTIBLE);
}
EXPORT_SYMBOL(wait_for_completion_interruptible_timeout);

/**
 * wait_for_completion_killable: - waits for completion of a task (killable)
 * @x:  holds the state of this particular completion
 *
 * This waits to be signaled for completion of a specific task. It can be
 * interrupted by a kill signal.
 */
int __sched wait_for_completion_killable(struct completion *x)
{
	long t = wait_for_common(x, MAX_SCHEDULE_TIMEOUT, TASK_KILLABLE);
	if (t == -ERESTARTSYS)
		return t;
	return 0;
}
EXPORT_SYMBOL(wait_for_completion_killable);
```

## wait_for_common の中味

```c
static long __sched
wait_for_common(struct completion *x, long timeout, int state)
{
	might_sleep();

	spin_lock_irq(&x->wait.lock);
	timeout = do_wait_for_common(x, timeout, state);
	spin_unlock_irq(&x->wait.lock);
	return timeout;
}
```

## do_wait_for_common の中身

 * wait_queue_head_, wait_queue_t を使っている

```c
static inline long __sched
do_wait_for_common(struct completion *x, long timeout, int state)
{
	if (!x->done) {
		DECLARE_WAITQUEUE(wait, current);

        // ? 
		wait.flags |= WQ_FLAG_EXCLUSIVE;
		__add_wait_queue_tail(&x->wait, &wait);

        // 条件完了待ちのループ
		do {
            // シグナルを受信していたら ERESTARTSYS
			if (signal_pending_state(state, current)) {
				timeout = -ERESTARTSYS;
				break;
			}
            // ここで 引き数の state を明示的にセット
			__set_current_state(state);
			spin_unlock_irq(&x->wait.lock);
            // タイムアウト付きスケジュール
			timeout = schedule_timeout(timeout);

            // 起床
			spin_lock_irq(&x->wait.lock);
        // 条件 or timeout に至るまでループ
		} while (!x->done && timeout);
		__remove_wait_queue(&x->wait, &wait);
		if (!x->done)
			return timeout;
	}
	x->done--;
    
    // 呼び出し側でタイムアウトかどうかを判定できる
	return timeout ?: 1;
}
```