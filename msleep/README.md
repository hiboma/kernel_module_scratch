## msleep

 * msleep TASK_UNINTERRUPTIBLE でブロック(?)する
 * schedule_timeout_uninterruptible が実態

```c
/**
 * msleep - sleep safely even with waitqueue interruptions
 * @msecs: Time in milliseconds to sleep for
 */
void msleep(unsigned int msecs)
{
	unsigned long timeout = msecs_to_jiffies(msecs) + 1;

	while (timeout)
		timeout = schedule_timeout_uninterruptible(timeout);
}

EXPORT_SYMBOL(msleep);
```

## msleep_interruptible

 * TASK_INTERRUPTIBLE でブロック(?)する
 * シグナルを受けたら、タイムアウトの残り時間を返す?

```c
/**
 * msleep_interruptible - sleep waiting for signals
 * @msecs: Time in milliseconds to sleep for
 */
unsigned long msleep_interruptible(unsigned int msecs)
{
	unsigned long timeout = msecs_to_jiffies(msecs) + 1;

	while (timeout && !signal_pending(current))
		timeout = schedule_timeout_interruptible(timeout);
	return jiffies_to_msecs(timeout);
}
```

## サンプル

```
root      2086  0.0  0.2  66224  1216 ?        Ss   05:49   0:00 /usr/sbin/sshd
root      2093  0.0  0.8  97900  3832 ?        Ss   05:50   0:00  \_ sshd: vagrant [priv]
vagrant   2097  0.0  0.4  97900  2240 ?        S    05:50   0:00  |   \_ sshd: vagrant@pts/0
vagrant   2098  0.0  0.3  67040  1876 pts/0    Ss   05:50   0:00  |       \_ -bash
vagrant   3446  0.0  0.1 100932   580 pts/0    D+   05:58   0:00  |           \_ cat /sys/kernel/debug/msleep
```

```
[vagrant@localhost ~]$ cat /proc/3446/wchan 
msleep
```
