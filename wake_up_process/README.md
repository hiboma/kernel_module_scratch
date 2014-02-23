# wake_up_process, wake_up_state

 * 指定したタスクを起床させる
 * wait_queue_head_t と違って、task_sruct のポインタは自分で退避しておく
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