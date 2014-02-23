# wait_queue_head_t

 * http://wiki.bit-hive.com/linuxkernelmemo/pg/%A5%D7%A5%ED%A5%BB%A5%B9%A4%CEBlock%A4%C8Wakeup
   * wake_up_process(task_t *)
 * struct list_head と schedule_timeout, __set_current_state を使って待ちキューが実装されている
 * wake_up はちょっと複雑
   * try_to_wake_up で起床するけど、ロードバランシングなど

## API

**wait_queue_head_t**

 * wait_queue_t task_list に wait_queue_t を繋ぐ ( __add_wait_queue )
 * タスクのポイタを保持するのは wait_queue_t

```c
struct __wait_queue_head {
	spinlock_t lock;
	struct list_head task_list;
};
typedef struct __wait_queue_head wait_queue_head_t;
```

**wait_queue_t**

 * private に task_struct のポインタをいれとく
 * flags ???

```c
struct __wait_queue {
	unsigned int flags;
#define WQ_FLAG_EXCLUSIVE	0x01
	void *private;
	wait_queue_func_t func;
	struct list_head task_list;
};
```

 * DECLARE_WAIT_QUEUE_HEAD
 * init_waitqueue_head
 * interruptible_sleep_on_timeout
 * interruptible_sleep_on
 * sleep_on_timeout
 * sleep_on
 * wake_up_interruptible_all
 * wake_up_all
 * wake_up

interruptible がついてないのは TASK_UNINTERRUPTIBLE で事象待ち

**sleep_on_common**

```c
static long __sched
sleep_on_common(wait_queue_head_t *q, int state, long timeout)
{
	unsigned long flags;
	wait_queue_t wait;

	init_waitqueue_entry(&wait, current);

	__set_current_state(state);

	spin_lock_irqsave(&q->lock, flags);
	__add_wait_queue(q, &wait); wait_queue_head_t -> wait_queue_t -> wait_queue_t ...
	spin_unlock(&q->lock);
	timeout = schedule_timeout(timeout); // -> contex switch

    // wake_up もしくは スケジューリングタイムアウト後に再開
	spin_lock_irq(&q->lock);
	__remove_wait_queue(q, &wait);
	spin_unlock_irqrestore(&q->lock, flags);

	return timeout;
}
```

### wait_for_completion との違いは?

 * wait_for_completion はスケジューリングして条件が満たされるかを確認している
   * http://www.kerneldesign.info/wiki/index.php?wait_for_completion()/linux2.6
 * wait_queue_head_t は スケジューリングされない
   * 明示的に起床されるまで永遠に待ち続ける?
   * このまま眠り続けて死ぬ

## TODO

 * wake_up 後のスケジューリングについて
 * wait_queue_head_t の削除 API は?
 * プロセスコンテキスト/割り込みコンテキストでの使用方法
 * デバイスからの割り込みを擬似的に再現して wake_up 呼び出しできないかな

## wchan

/proc/<pid>/wchan は wait_queue_head_t で待ちに入る関数名を指していた

```
$ ps axf -opid,cmd,wchan

 1948  |           \_ cat /mnt/de debugfs_test_read
 1951  |           \_ cat /mnt/de piyo_read
```