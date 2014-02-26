# signal

## API

 * send_sig
 * force_sig
   * force が突くとどうなる?
   * シグナルハンドラを SIG_DFL にしてハンドラを解除、強制的に届ける

```c
int
send_sig(int sig, struct task_struct *p, int priv)
{
	return send_sig_info(sig, __si_special(priv), p);
}

void
force_sig(int sig, struct task_struct *p)
{
	force_sig_info(sig, SEND_SIG_PRIV, p);
}
```

force_sig_info の中身はこんなん

```c
/*
 * Force a signal that the process can't ignore: if necessary
 * we unblock the signal and change any SIG_IGN to SIG_DFL.
 *
 * Note: If we unblock the signal, we always reset it to SIG_DFL,
 * since we do not want to have a signal handler that was blocked
 * be invoked when user space had explicitly blocked it.
 *
 * We don't want to have recursive SIGSEGV's etc, for example,
 * that is why we also clear SIGNAL_UNKILLABLE.
 */
int
force_sig_info(int sig, struct siginfo *info, struct task_struct *t)
{
	unsigned long int flags;
	int ret, blocked, ignored;
	struct k_sigaction *action;

	spin_lock_irqsave(&t->sighand->siglock, flags);
	action = &t->sighand->action[sig-1];
    // シグナルハンドラに SIG_IGN をセットしているかどうかを見る
	ignored = action->sa.sa_handler == SIG_IGN;
	blocked = sigismember(&t->blocked, sig);
	if (blocked || ignored) {
        // ハンドラで制御できない
		action->sa.sa_handler = SIG_DFL;
		if (blocked) {
            // シグナルを削除?
			sigdelset(&t->blocked, sig);
			recalc_sigpending_and_wake(t);
		}
	}
    // ? 
	if (action->sa.sa_handler == SIG_DFL)
		t->signal->flags &= ~SIGNAL_UNKILLABLE;
	ret = specific_send_sig_info(sig, info, t); // => send_signal
	spin_unlock_irqrestore(&t->sighand->siglock, flags);

	return ret;
}
````
   
 * force_sigsegv
 * kill_pgrp
 * kill_pid
   * struct pid の扱い方が分からん
 * SIGNAL_UNKILLABLE

__extra__
 
 * for_each_process
   * task_struct をイテレート

```c
#define next_task(p) \
	list_entry_rcu((p)->tasks.next, struct task_struct, tasks)

#define for_each_process(p) \
	for (p = &init_task ; (p = next_task(p)) != &init_task ; )
```   

