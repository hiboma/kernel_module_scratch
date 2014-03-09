# wait_queue_head_t

 * http://wiki.bit-hive.com/linuxkernelmemo/pg/%A5%D7%A5%ED%A5%BB%A5%B9%A4%CEBlock%A4%C8Wakeup
   * wake_up_process(task_t *)
 * struct list_head と schedule_timeout, __set_current_state を使って待ちキューが実装されている
 * wake_up はちょっと複雑
   * try_to_wake_up で起床するけど、ロードバランシングなど

## API

### wait_queue_head_t

ただのリスト

 * wait_queue_t task_list に wait_queue_t を繋ぐ ( __add_wait_queue )
   * task_truct のポインタを保持するのは wait_queue_t
 * DECLARE_WAIT_QUEUE_HEAD
 * init_waitqueue_head

```c
struct __wait_queue_head {
	spinlock_t lock;
	struct list_head task_list;
};
typedef struct __wait_queue_head wait_queue_head_t;
```

### wait_queue_t

 * private に task_struct のポインタをいれとく
   * flags ???
   * WQ_FLAG_EXCLUSIVE を立てると wake_up, wake_up_nr が 1 から Nプロセスずつの起床になる
 * init_waitqueue_entry で初期化できる(メモリ確保はしない)

```c
struct __wait_queue {
	unsigned int flags;
#define WQ_FLAG_EXCLUSIVE	0x01
	void *private;
	wait_queue_func_t func;
	struct list_head task_list;
};
```

### 起床

 * wake_up
 * wake_up_nr
 * wake_up_all
 * wake_up_interruptible
 * wake_up_interruptible_nr
 * wake_up_interruptible_all
 * wake_up_interruptible_sync

いずれも __wake_up_common を呼び出す

 * wake_up WQ_FLAG_EXCLUSIVE がたってる場合にのみ 1〜Nプロセスずつ起床させる
   * WQ_FLAG_EXCLUSIVE を指定してないならどれも一緒?

```c
/*
 * The core wakeup function. Non-exclusive wakeups (nr_exclusive == 0) just
 * wake everything up. If it's an exclusive wakeup (nr_exclusive == small +ve
 * number) then we wake all the non-exclusive tasks and one exclusive task.
 *
 * There are circumstances in which we can try to wake a task which has already
 * started to run but is not in state TASK_RUNNING. try_to_wake_up() returns
 * zero in this (rare) case, and we handle it by continuing to scan the queue.
 */
static void __wake_up_common(wait_queue_head_t *q, unsigned int mode,
			int nr_exclusive, int wake_flags, void *key)
{
	wait_queue_t *curr, *next;

    // 待ち行列の先頭か起床でよいよね? = queue
    // flgas にWQ_FLAG_EXCLUSIVE がたってないと全部起床するぞ
	list_for_each_entry_safe(curr, next, &q->task_list, task_list) {
		unsigned flags = curr->flags;

		if (curr->func(curr, mode, wake_flags, key) &&
				(flags & WQ_FLAG_EXCLUSIVE) && !--nr_exclusive)
			break;
	}
}
```

特に指定が無ければ curr->func は default_wake_function を呼び出している

```c
int default_wake_function(wait_queue_t *curr, unsigned mode, int wake_flags,
			  void *key)
{
	return try_to_wake_up(curr->private, mode, wake_flags);
}
EXPORT_SYMBOL(default_wake_function);
```

try_to_wake_up は複雑なので割愛する

### 事象待ち

 * sleep_on
 * sleep_on_timeout
 * interruptible_sleep_on
 * interruptible_sleep_on_timeout

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

## wait_for_completion との違いは?

 * wait_for_completion はスケジューリングして条件が満たされるかを確認している
   * http://www.kerneldesign.info/wiki/index.php?wait_for_completion()/linux2.6
 * wait_queue_head_t は スケジューリングされない
   * 明示的に起床されるまで永遠に待ち続ける?
   * このまま眠り続けて死ぬ

## prepare_to_wait, add_wait_queue の使い方

 * prepare_to_wait, prepare_to_wait_exclusive は DEFINE_WAIT, finish_wait との組で使う
 * add_wait_queue, add_wait_queue_exclusive は init_waitqueue_entry, remove_wait_queue との組で使う
   * 起床の関数が autoremove_wake_function

http://wiki.bit-hive.com/linuxkernelmemo/pgdiff/?pg=%A5%D7%A5%ED%A5%BB%A5%B9%A4%CEBlock%A4%C8Wakeup&rev=7 を参考にしよう

## wait_event, wait_event_interruptible

 * wait_event(wait_queue_head_t *, condition) で condition が true になるまで待機
 * 起床すると condition を評価して false なら再度待機に入る
   * completion と似ているけど、他プロセスからの起床が必要なのが違う

## TODO

 * wake_up 後のスケジューリングについて
   * 優先度が ___待機から起床したプロセス > 起床させたプロセス(current) ___ の場合にプリエンプト要求を出す
     * プリエンプト要求 = need_resched() こと
   * wake_up_*_sync の場合はプリエンプトをしない => WF_FLAGS が立っている
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